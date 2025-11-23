#include "compress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para contar a frequência de caracteres em um arquivo
void count_frequencies(const char *file_path, int frequencies[256])
{
    FILE *file = fopen(file_path, "rb"); // Abrir em modo binário
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo para contagem de frequência");
        exit(EXIT_FAILURE);
    }

    // Inicializar todas as frequências em zero
    memset(frequencies, 0, 256 * sizeof(int));

    int ch;
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch > 255){
            perror("Caractere %c não suportado.", ch);
            exit(EXIT_FAILURE);
        }
        frequencies[ch]++;
    }

    fclose(file);
}

void compress_file(const char *file_path)
{
    printf("Função de compactar chamada para o arquivo: %s\n", file_path);

    int frequencies[256];
    count_frequencies(file_path, frequencies);

    printf("Frequências dos caracteres no arquivo:\n");
    for (int i = 0; i < 256; i++)
    {
        if (frequencies[i] > 0)
        {
            // Exibe o caractere se for imprimível, senão exibe o código ASCII
            if (i >= 32 && i <= 126)
            { // Caracteres imprimíveis ASCII
                printf("  Caractere '%c' (ASCII %d): %d vezes\n", (char)i, i, frequencies[i]);
            }
            else
            {
                printf("  Caractere (ASCII %d): %d vezes\n", i, frequencies[i]);
            }
        }
    }
}
