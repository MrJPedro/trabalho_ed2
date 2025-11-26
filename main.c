#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "compress.h"
#include "search.h"
#include "compressed_search.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <comando> [argumentos]\n", argv[0]);
        return 1;
    }

    const char *command = argv[1];

    if (strcmp(command, "compactar") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Uso: %s compactar <arquivo_original> <arquivo_compactado> [block_size]\n", argv[0]);
            return 1;
        }
        uint32_t block_size = 4096;
        if (argc >= 5) {
            block_size = (uint32_t)strtoul(argv[4], NULL, 10);
            if (block_size == 0) block_size = 4096;
            if (block_size < MIN_BLOCK_SIZE) block_size = MIN_BLOCK_SIZE;
            if (block_size > MAX_BLOCK_SIZE) block_size = MAX_BLOCK_SIZE;
        }
        if (compress_file(argv[2], argv[3], block_size) != 0) {
            fprintf(stderr, "Falha na compactacao\n");
            return 1;
        }
    } else if (strcmp(command, "descompactar") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Uso: %s descompactar <arquivo_compactado> <arquivo_saida>\n", argv[0]);
            return 1;
        }
        if (decompress_file(argv[2], argv[3]) != 0) {
            fprintf(stderr, "Falha na descompactacao\n");
            return 1;
        }
    } else if (strcmp(command, "busca") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Uso: %s busca <arquivo> <padrao>\n", argv[0]);
            return 1;
        }
        if (search_file(argv[2], argv[3]) < 0) {
            fprintf(stderr, "Falha na busca\n");
            return 1;
        }
    } else if (strcmp(command, "busca_compactada") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Uso: %s busca_compactada <arquivo> <padrao> [offset_hint] [raio]\n", argv[0]);
            return 1;
        }
        int64_t hint = -1;
        uint32_t raio = 0;
        if (argc >= 5) {
            hint = (int64_t)strtoll(argv[4], NULL, 10);
        }
        if (argc >= 6) {
            raio = (uint32_t)strtoul(argv[5], NULL, 10);
        }
        if (search_compressed_file(argv[2], argv[3], hint, raio) < 0) {
            fprintf(stderr, "Falha na busca em compactado\n");
            return 1;
        }
    } else {
        fprintf(stderr, "Comando desconhecido: %s\n", command);
        return 1;
    }

    return 0;
}
