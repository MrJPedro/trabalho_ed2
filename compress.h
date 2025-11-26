#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdint.h>
#include <stdio.h>

#define MIN_BLOCK_SIZE (1024U)
#define MAX_BLOCK_SIZE (8U * 1024U * 1024U)

// Bloco indexado no arquivo compactado.
typedef struct {
    uint64_t orig_offset; // deslocamento no arquivo original
    uint32_t orig_size;   // tamanho do bloco original
    uint64_t comp_offset; // deslocamento para os dados compactados
    uint32_t comp_size;   // tamanho dos dados compactados
} BlockIndex;

// Le cabecalho CMP1; retorna 0 em sucesso.
int cmp_read_header(FILE *in, uint32_t *block_size, uint32_t *block_count, uint64_t *index_offset);

// Limites validos de block_size.
int cmp_block_size_valid(uint32_t block_size);

// Ajusta block_size para dentro dos limites.
uint32_t cmp_clamp_block_size(uint32_t block_size);

// Compacta arquivo em blocos e grava indice.
int compress_file(const char *input_path, const char *output_path, uint32_t block_size);

// Descompacta um bloco Huffman em buffer fornecido.
int decompress_block(FILE *fp, const BlockIndex *entry, uint8_t *out);

// Reconstrui o arquivo original.
int decompress_file(const char *input_path, const char *output_path);

#endif
