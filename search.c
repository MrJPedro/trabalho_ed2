#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void kmp_compute_lps(const char *pat, int m, int *lps) {
    int len = 0;
    lps[0] = 0;
    for (int i = 1; i < m;) {
        if (pat[i] == pat[len]) {
            lps[i++] = ++len;
        } else if (len != 0) {
            len = lps[len - 1];
        } else {
            lps[i++] = 0;
        }
    }
}

int search_file(const char *file_path, const char *pattern) {
    FILE *fp = fopen(file_path, "rb");
    if (!fp) {
        perror("Erro abrindo arquivo para busca");
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

    const size_t chunk = 4096;
    size_t overlap = (m > 0) ? (size_t)(m - 1) : 0;
    uint8_t *buf = (uint8_t *)malloc(chunk + overlap);
    if (!buf) {
        free(lps);
        fclose(fp);
        return -1;
    }

    size_t total_read = 0;
    size_t carry = 0;
    int count = 0;
    int j = 0;

    while (1) {
        size_t readn = fread(buf + carry, 1, chunk, fp);
        size_t window = carry + readn;
        uint64_t base_offset = (uint64_t)(total_read >= carry ? (total_read - carry) : 0);
        if (window == 0) break;

        size_t i = 0;
        while (i < window) {
            while (j > 0 && (j >= m || pattern[j] != buf[i])) {
                j = lps[j - 1];
            }
            if (pattern[j] == buf[i]) j++;
            if (j == m) {
                uint64_t pos = base_offset + i + 1 - m; // zero-based
                printf("Encontrado em offset %llu\n", (unsigned long long)pos);
                count++;
                j = lps[j - 1];
            }
            i++;
        }

        total_read += readn;
        if (readn < chunk) break;
        size_t keep = overlap < window ? overlap : window;
        memmove(buf, buf + window - keep, keep);
        carry = keep;
    }

    free(buf);
    free(lps);
    fclose(fp);
    return count;
}
