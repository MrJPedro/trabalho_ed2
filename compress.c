#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include "compress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

static const uint64_t HEADER_SIZE = 4 + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint64_t);

typedef struct {
    int left;
    int right;
    uint32_t freq;
    uint8_t byte;
    uint8_t is_leaf;
} HuffNode;

typedef struct {
    uint64_t bits;
    uint8_t len;
} HuffCode;

uint32_t cmp_clamp_block_size(uint32_t block_size) {
    if (block_size < MIN_BLOCK_SIZE) return MIN_BLOCK_SIZE;
    if (block_size > MAX_BLOCK_SIZE) return MAX_BLOCK_SIZE;
    return block_size;
}

int cmp_block_size_valid(uint32_t block_size) {
    return block_size >= MIN_BLOCK_SIZE && block_size <= MAX_BLOCK_SIZE;
}

static int write_header(FILE *out, uint32_t block_size, uint32_t block_count, uint64_t index_offset) {
    const char magic[4] = {'C', 'M', 'P', '1'};
    if (fwrite(magic, 1, 4, out) != 4) return -1;
    if (fwrite(&block_size, sizeof(block_size), 1, out) != 1) return -1;
    if (fwrite(&block_count, sizeof(block_count), 1, out) != 1) return -1;
    if (fwrite(&index_offset, sizeof(index_offset), 1, out) != 1) return -1;
    return 0;
}

int cmp_read_header(FILE *in, uint32_t *block_size, uint32_t *block_count, uint64_t *index_offset) {
    char magic[4];
    if (fread(magic, 1, 4, in) != 4) return -1;
    if (memcmp(magic, "CMP1", 4) != 0) return -1;
    if (fread(block_size, sizeof(*block_size), 1, in) != 1) return -1;
    if (fread(block_count, sizeof(*block_count), 1, in) != 1) return -1;
    if (fread(index_offset, sizeof(*index_offset), 1, in) != 1) return -1;
    return 0;
}

// Min-heap de indices de nos para construir a arvore.
static void heapify_down(int *heap, int size, int idx, HuffNode *nodes) {
    while (1) {
        int l = idx * 2 + 1;
        int r = idx * 2 + 2;
        int smallest = idx;
        if (l < size && nodes[heap[l]].freq < nodes[heap[smallest]].freq) smallest = l;
        if (r < size && nodes[heap[r]].freq < nodes[heap[smallest]].freq) smallest = r;
        if (smallest == idx) break;
        int tmp = heap[idx];
        heap[idx] = heap[smallest];
        heap[smallest] = tmp;
        idx = smallest;
    }
}

static int build_huffman_tree(uint32_t freq[256], HuffNode *nodes) {
    int heap[512];
    int heap_size = 0;
    int node_count = 0;

    for (int i = 0; i < 256; i++) {
        if (freq[i] == 0) continue;
        nodes[node_count].left = nodes[node_count].right = -1;
        nodes[node_count].freq = freq[i];
        nodes[node_count].byte = (uint8_t)i;
        nodes[node_count].is_leaf = 1;
        heap[heap_size++] = node_count;
        node_count++;
    }

    if (heap_size == 0) return -1; // bloco vazio

    // Heapify inicial
    for (int i = (heap_size / 2) - 1; i >= 0; i--) {
        heapify_down(heap, heap_size, i, nodes);
    }

    // Caso especial: so um simbolo.
    if (heap_size == 1) {
        return heap[0];
    }

    while (heap_size > 1) {
        int a = heap[0];
        heap[0] = heap[--heap_size];
        heapify_down(heap, heap_size, 0, nodes);

        int b = heap[0];
        heap[0] = heap[--heap_size];
        heapify_down(heap, heap_size, 0, nodes);

        nodes[node_count].left = a;
        nodes[node_count].right = b;
        nodes[node_count].freq = nodes[a].freq + nodes[b].freq;
        nodes[node_count].is_leaf = 0;
        nodes[node_count].byte = 0;

        heap[heap_size++] = node_count;
        // Sobe o novo no
        int idx = heap_size - 1;
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (nodes[heap[parent]].freq <= nodes[heap[idx]].freq) break;
            int tmp = heap[parent];
            heap[parent] = heap[idx];
            heap[idx] = tmp;
            idx = parent;
        }
        node_count++;
    }

    return heap[0];
}

static void build_codes(int node_idx, HuffNode *nodes, HuffCode codes[256], uint64_t path, int depth) {
    if (nodes[node_idx].is_leaf) {
        codes[nodes[node_idx].byte].bits = path;
        codes[nodes[node_idx].byte].len = (depth == 0) ? 1 : depth; // se so um simbolo, usa 1 bit
        return;
    }
    if (nodes[node_idx].left != -1) {
        build_codes(nodes[node_idx].left, nodes, codes, path << 1, depth + 1);
    }
    if (nodes[node_idx].right != -1) {
        build_codes(nodes[node_idx].right, nodes, codes, (path << 1) | 1ULL, depth + 1);
    }
}

static int huffman_compress_block(const uint8_t *data, size_t len, FILE *out) {
    uint32_t freq[256] = {0};
    for (size_t i = 0; i < len; i++) {
        freq[data[i]]++;
    }

    HuffNode nodes[512] = {0};
    int root = build_huffman_tree(freq, nodes);
    if (root == -1) return -1;

    HuffCode codes[256] = {0};
    build_codes(root, nodes, codes, 0, 0);

    // Grava tabela de frequencia (1024 bytes).
    if (fwrite(freq, sizeof(uint32_t), 256, out) != 256) return -1;

    uint8_t byte = 0;
    int bitpos = 0;
    for (size_t i = 0; i < len; i++) {
        HuffCode c = codes[data[i]];
        for (int b = c.len - 1; b >= 0; b--) {
            uint8_t bit = (uint8_t)((c.bits >> b) & 1U);
            byte |= (uint8_t)(bit << (7 - bitpos));
            bitpos++;
            if (bitpos == 8) {
                if (fwrite(&byte, 1, 1, out) != 1) return -1;
                byte = 0;
                bitpos = 0;
            }
        }
    }
    if (bitpos > 0) {
        if (fwrite(&byte, 1, 1, out) != 1) return -1;
    }
    return 0;
}

static int huffman_decompress_block(FILE *in, uint64_t start, uint32_t comp_size, uint8_t *out, uint32_t out_size) {
    if (fseeko(in, (off_t)start, SEEK_SET) != 0) return -1;

    uint32_t freq[256];
    size_t freq_bytes = sizeof(uint32_t) * 256;
    if (comp_size < freq_bytes) return -1;
    if (fread(freq, sizeof(uint32_t), 256, in) != 256) return -1;

    HuffNode nodes[512] = {0};
    int root = build_huffman_tree(freq, nodes);
    if (root == -1) return -1;

    uint32_t produced = 0;
    uint64_t data_bytes = comp_size - (uint32_t)freq_bytes;

    // Caso especial: so um simbolo
    if (nodes[root].is_leaf) {
        memset(out, nodes[root].byte, out_size);
        return 0;
    }

    int current = root;
    for (uint64_t i = 0; i < data_bytes && produced < out_size; i++) {
        uint8_t byte = 0;
        if (fread(&byte, 1, 1, in) != 1) return -1;
        for (int bit = 0; bit < 8 && produced < out_size; bit++) {
            uint8_t b = (byte >> (7 - bit)) & 1U;
            current = (b == 0) ? nodes[current].left : nodes[current].right;
            if (current == -1) return -1;
            if (nodes[current].is_leaf) {
                out[produced++] = nodes[current].byte;
                current = root;
            }
        }
    }

    return produced == out_size ? 0 : -1;
}

int compress_file(const char *input_path, const char *output_path, uint32_t block_size) {
    block_size = cmp_clamp_block_size(block_size);

    FILE *in = fopen(input_path, "rb");
    if (!in) {
        perror("Erro abrindo arquivo de entrada");
        return -1;
    }
    FILE *out = fopen(output_path, "wb+");
    if (!out) {
        perror("Erro criando arquivo de saida");
        fclose(in);
        return -1;
    }

    if (write_header(out, block_size, 0, 0) != 0) {
        fclose(in);
        fclose(out);
        fprintf(stderr, "Falha ao escrever cabecalho\n");
        return -1;
    }

    FILE *idx = tmpfile();
    if (!idx) {
        fclose(in);
        fclose(out);
        fprintf(stderr, "Falha criando arquivo temporario de indice\n");
        return -1;
    }

    uint32_t count = 0;
    uint8_t *buffer = (uint8_t *)malloc(block_size);
    if (!buffer) {
        fclose(in);
        fclose(out);
        fclose(idx);
        return -1;
    }

    while (1) {
        size_t readn = fread(buffer, 1, block_size, in);
        if (readn == 0) break;
        BlockIndex entry;
        entry.orig_offset = (uint64_t)count * block_size;
        entry.orig_size = (uint32_t)readn;
        off_t before = ftello(out);
        if (before < 0) {
            fprintf(stderr, "Falha obtendo offset antes da compressao\n");
            free(buffer);
            fclose(in);
            fclose(out);
            fclose(idx);
            return -1;
        }
        entry.comp_offset = (uint64_t)before;

        if (huffman_compress_block(buffer, readn, out) != 0) {
            fprintf(stderr, "Falha ao compactar bloco %u\n", count);
            free(buffer);
            fclose(in);
            fclose(out);
            fclose(idx);
            return -1;
        }
        off_t after = ftello(out);
        if (after < 0 || after < before) {
            fprintf(stderr, "Falha obtendo offset apos compressao\n");
            free(buffer);
            fclose(in);
            fclose(out);
            fclose(idx);
            return -1;
        }
        entry.comp_size = (uint32_t)(after - before);

        if (fwrite(&entry, sizeof(BlockIndex), 1, idx) != 1) {
            fprintf(stderr, "Falha ao gravar entrada do indice\n");
            free(buffer);
            fclose(in);
            fclose(out);
            fclose(idx);
            return -1;
        }
        count++;
    }

    off_t index_offset = ftello(out);
    if (index_offset < 0) {
        fprintf(stderr, "Falha obtendo offset do indice\n");
        free(buffer);
        fclose(in);
        fclose(out);
        fclose(idx);
        return -1;
    }
    if (fseeko(idx, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Erro reposicionando indice temporario\n");
        free(buffer);
        fclose(in);
        fclose(out);
        fclose(idx);
        return -1;
    }

    BlockIndex entry;
    for (uint32_t i = 0; i < count; i++) {
        if (fread(&entry, sizeof(BlockIndex), 1, idx) != 1) {
            fprintf(stderr, "Erro lendo indice temporario\n");
            free(buffer);
            fclose(in);
            fclose(out);
            fclose(idx);
            return -1;
        }
        if (fwrite(&entry, sizeof(BlockIndex), 1, out) != 1) {
            fprintf(stderr, "Erro ao gravar indice\n");
            free(buffer);
            fclose(in);
            fclose(out);
            fclose(idx);
            return -1;
        }
    }

    if (fseeko(out, 0, SEEK_SET) != 0 || write_header(out, block_size, count, (uint64_t)index_offset) != 0) {
        fprintf(stderr, "Erro atualizando cabecalho\n");
        free(buffer);
        fclose(in);
        fclose(out);
        fclose(idx);
        return -1;
    }

    printf("Compactacao concluida. Blocos: %u. Indice em offset %lld\n", count, (long long)index_offset);

    free(buffer);
    fclose(in);
    fclose(out);
    fclose(idx);
    return 0;
}

int decompress_block(FILE *fp, const BlockIndex *entry, uint8_t *out) {
    return huffman_decompress_block(fp, entry->comp_offset, entry->comp_size, out, entry->orig_size);
}

int decompress_file(const char *input_path, const char *output_path) {
    FILE *in = fopen(input_path, "rb");
    if (!in) {
        perror("Erro abrindo compactado");
        return -1;
    }

    if (fseeko(in, 0, SEEK_END) != 0) {
        fclose(in);
        return -1;
    }
    off_t file_size_off = ftello(in);
    if (file_size_off < 0) {
        fclose(in);
        return -1;
    }
    uint64_t file_size = (uint64_t)file_size_off;
    if (fseeko(in, 0, SEEK_SET) != 0) {
        fclose(in);
        return -1;
    }

    uint32_t block_size = 0, block_count = 0;
    uint64_t index_offset = 0;
    if (cmp_read_header(in, &block_size, &block_count, &index_offset) != 0 || block_size == 0 || block_count == 0) {
        fprintf(stderr, "Arquivo compactado invalido\n");
        fclose(in);
        return -1;
    }
    if (!cmp_block_size_valid(block_size)) {
        fprintf(stderr, "Tamanho de bloco fora dos limites suportados\n");
        fclose(in);
        return -1;
    }

    // Validacao basica de offsets para evitar leituras fora do arquivo.
    uint64_t index_bytes = (uint64_t)block_count * sizeof(BlockIndex);
    if (index_offset < HEADER_SIZE || index_offset > file_size || index_bytes > file_size || index_offset + index_bytes > file_size) {
        fprintf(stderr, "Indice invalido ou corrompido\n");
        fclose(in);
        return -1;
    }

    FILE *out = fopen(output_path, "wb");
    if (!out) {
        perror("Erro criando saida");
        fclose(in);
        return -1;
    }

    uint8_t *buffer = (uint8_t *)malloc(block_size);
    if (!buffer) {
        fclose(in);
        fclose(out);
        return -1;
    }

    for (uint32_t i = 0; i < block_count; i++) {
        if (fseeko(in, (off_t)(index_offset + (uint64_t)i * sizeof(BlockIndex)), SEEK_SET) != 0) {
            fprintf(stderr, "Erro ao posicionar indice\n");
            free(buffer);
            fclose(in);
            fclose(out);
            return -1;
        }
        BlockIndex entry;
        if (fread(&entry, sizeof(BlockIndex), 1, in) != 1) {
            fprintf(stderr, "Falha lendo indice (entrada %u)\n", i);
            free(buffer);
            fclose(in);
            fclose(out);
            return -1;
        }
        if (entry.orig_size == 0 || entry.orig_size > block_size) {
            fprintf(stderr, "Entrada de indice invalida (orig_size)\n");
            free(buffer);
            fclose(in);
            fclose(out);
            return -1;
        }
        if (entry.comp_size == 0 ||
            entry.comp_offset < HEADER_SIZE ||
            entry.comp_offset >= index_offset ||
            (entry.comp_offset + (uint64_t)entry.comp_size) > index_offset ||
            (entry.comp_offset + (uint64_t)entry.comp_size) > file_size) {
            fprintf(stderr, "Entrada de indice invalida (comp_offset/comp_size)\n");
            free(buffer);
            fclose(in);
            fclose(out);
            return -1;
        }
        if (decompress_block(in, &entry, buffer) != 0) {
            fprintf(stderr, "Erro ao descompactar bloco %u\n", i);
            free(buffer);
            fclose(in);
            fclose(out);
            return -1;
        }
        if (fwrite(buffer, 1, entry.orig_size, out) != entry.orig_size) {
            fprintf(stderr, "Erro escrevendo bloco %u\n", i);
            free(buffer);
            fclose(in);
            fclose(out);
            return -1;
        }
    }

    printf("Descompactacao concluida em %s\n", output_path);
    free(buffer);
    fclose(in);
    fclose(out);
    return 0;
}
