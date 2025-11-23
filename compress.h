#ifndef COMPRESS_H
#define COMPRESS_H

// Função para contar a frequência de caracteres em um arquivo
void count_frequencies(const char *file_path, int frequencies[256]);

void compress_file(const char *file_path);

#endif
