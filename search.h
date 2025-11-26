#ifndef SEARCH_H
#define SEARCH_H

// Tabela de falhas do KMP para um padrao.
void kmp_compute_lps(const char *pat, int m, int *lps);

int search_file(const char *file_path, const char *pattern);

#endif
