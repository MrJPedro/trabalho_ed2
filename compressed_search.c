#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include "compressed_search.h"
#include "compress.h"
#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

int search_compressed_file(const char *file_path, const char *pattern, int64_t hint_offset, uint32_t radius) {
    FILE *fp = fopen(file_path, "rb");
    if (!fp) {
        perror("Erro abrindo compactado");
        return -1;
    }

    if (fseeko(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }
    off_t file_size_off = ftello(fp);
    if (file_size_off < 0) {
        fclose(fp);
        return -1;
    }
    uint64_t file_size = (uint64_t)file_size_off;
    if (fseeko(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    uint32_t block_size = 0, block_count = 0;
    uint64_t index_offset = 0;
    if (cmp_read_header(fp, &block_size, &block_count, &index_offset) != 0) {
        fprintf(stderr, "Arquivo compactado invalido\n");
        fclose(fp);
        return -1;
    }

    if (block_size == 0 || block_count == 0 || !cmp_block_size_valid(block_size)) {
        fprintf(stderr, "Arquivo compactado invalido (tamanho de bloco ou contagem)\n");
        fclose(fp);
        return -1;
    }

    uint64_t index_bytes = (uint64_t)block_count * sizeof(BlockIndex);
    if (index_offset < 4 + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint64_t) ||
        index_offset > file_size || index_bytes > file_size || index_offset + index_bytes > file_size) {
        fprintf(stderr, "Indice invalido ou corrompido\n");
        fclose(fp);
        return -1;
    }

    int m = (int)strlen(pattern);
    if (m == 0) {
        printf("Padrao vazio nao e suportado\n");
        fclose(fp);
        return 0;
    }
    int *lps = (int *)malloc(sizeof(int) * m);
    if (!lps) {
        fclose(fp);
        return -1;
    }
    kmp_compute_lps(pattern, m, lps);

    size_t overlap = (m > 0) ? (size_t)(m - 1) : 0;
    size_t buf_size = (size_t)block_size + overlap;
    uint8_t *buffer = (uint8_t *)malloc(buf_size);
    if (!buffer) {
        free(lps);
        fclose(fp);
        return -1;
    }

    // Janela opcional: se hint_offset >= 0, limita aos blocos que cruzam [start, end]; senao varre tudo.
    uint64_t start = 0;
    uint64_t end = (uint64_t)-1;
    if (hint_offset >= 0) {
        uint64_t h = (uint64_t)hint_offset;
        start = (h > radius) ? (h - radius) : 0;
        end = h + radius + (uint64_t)m;
    }

    size_t carry = 0;
    int j = 0;
    int count = 0;

    for (uint32_t b = 0; b < block_count; b++) {
        if (fseeko(fp, (off_t)(index_offset + (uint64_t)b * sizeof(BlockIndex)), SEEK_SET) != 0) {
            fprintf(stderr, "Erro ao posicionar indice\n");
            break;
        }
        BlockIndex entry;
        if (fread(&entry, sizeof(BlockIndex), 1, fp) != 1) {
            fprintf(stderr, "Falha lendo indice (entrada %u)\n", b);
            break;
        }

        if (entry.orig_size == 0 || entry.orig_size > block_size) {
            fprintf(stderr, "Entrada de indice invalida (orig_size)\n");
            break;
        }
        if (entry.comp_size == 0 ||
            entry.comp_offset < 4 + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint64_t) ||
            entry.comp_offset >= index_offset ||
            (entry.comp_offset + (uint64_t)entry.comp_size) > index_offset ||
            (entry.comp_offset + (uint64_t)entry.comp_size) > file_size) {
            fprintf(stderr, "Entrada de indice invalida (comp_offset/comp_size)\n");
            break;
        }

        uint64_t block_start = entry.orig_offset;
        uint64_t block_end = block_start + entry.orig_size;

        if (hint_offset >= 0 && block_end <= start) {
            carry = 0; // zera overlap ao pular blocos
            j = 0;     // evita atravessar lacunas
            continue;
        }
        if (hint_offset >= 0 && block_start >= end) {
            break;
        }

        if (decompress_block(fp, &entry, buffer + carry) != 0) {
            fprintf(stderr, "Erro ao descompactar bloco %u\n", b);
            break;
        }

        size_t window = carry + entry.orig_size;
        uint64_t base_offset = entry.orig_offset;
        if (carry > 0 && base_offset >= carry) {
            base_offset -= carry;
        }

        for (size_t i = 0; i < window; i++) {
            uint64_t pos_now = base_offset + i;
            if (hint_offset >= 0 && (pos_now < start || pos_now > end)) {
                j = 0; // descarta prefixos fora da janela
                continue;
            }
            while (j > 0 && (j >= m || pattern[j] != buffer[i])) {
                j = lps[j - 1];
            }
            if (pattern[j] == buffer[i]) j++;
            if (j == m) {
                uint64_t pos = pos_now + 1 - m;
                printf("Encontrado em offset %llu\n", (unsigned long long)pos);
                count++;
                j = lps[j - 1];
            }
        }

        if (m > 1) {
            size_t keep = window < (size_t)(m - 1) ? window : (size_t)(m - 1);
            memmove(buffer, buffer + window - keep, keep);
            carry = keep;
        } else {
            carry = 0;
        }
    }

    free(buffer);
    free(lps);
    fclose(fp);
    return count;
}
