#include <stdio.h>
#include <string.h>
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
        if (argc < 3) {
            fprintf(stderr, "Uso: %s compactar <arquivo>\n", argv[0]);
            return 1;
        }
        compress_file(argv[2]);
    } else if (strcmp(command, "busca") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Uso: %s busca <arquivo> <padrao>\n", argv[0]);
            return 1;
        }
        search_file(argv[2], argv[3]);
    } else if (strcmp(command, "busca_compactada") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Uso: %s busca_compactada <arquivo> <padrao>\n", argv[0]);
            return 1;
        }
        search_compressed_file(argv[2], argv[3]);
    } else {
        fprintf(stderr, "Comando desconhecido: %s\n", command);
        return 1;
    }

    return 0;
}
