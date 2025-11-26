#ifndef COMPRESSED_SEARCH_H
#define COMPRESSED_SEARCH_H

#include <stdint.h>

// Busca substring em arquivo compactado; com hint_offset >= 0 limita a janela [hint_offset - radius, hint_offset + radius].
int search_compressed_file(const char *file_path, const char *pattern, int64_t hint_offset, uint32_t radius);

#endif
