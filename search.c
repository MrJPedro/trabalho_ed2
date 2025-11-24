#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Fills lps[] for given pattern pat
void computeLPSArray(const char* pat, int M, int* lps){
    // Length of the previous longest prefix suffix
    int len = 0;

    // lps[0] is always 0
    lps[0] = 0;

    // Loop calculates lps[i] for i = 1 to M-1
    int i = 1;
    while (i < M) {
        if (pat[i] == pat[len]) {     // abab   lps[2]=1 lps[3]=2
            len++;
            lps[i] = len;
            i++;
        }
        else {
            if (len != 0) {
                len = lps[len - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

// Prints occurrences of pat in txt and returns an array of
// occurrences
int* search_file(const char* pattern, const char* file_path, int* count){

    printf("Função de busca chamada para o arquivo: %s com o padrão: %s\n", file_path, pattern);

    int M = strlen(pattern);
    int N = strlen(file_path);

    // Create lps[] that will hold the longest prefix suffix
    // values for pattern
    int* lps = (int*)malloc(M * sizeof(int));

    // Preprocess the pattern (calculate lps[] array)
    computeLPSArray(pattern, M, lps);                   // abab   lps[2]=1 lps[3]=2

    int* result = (int*)malloc(N * sizeof(int));

    // Number of occurrences found
    *count = 0;

    int i = 0; // index for txt
    int j = 0; // index for pat
  
    while ((N - i) >= (M - j)) {
        if (pattern[j] == file_path[i]) {
            j++;
            i++;
        }

        if (j == M) {

            // Record the occurrence (1-based index)
            result[*count] = i - j + 1;
            (*count)++;
            j = lps[j - 1];
        }
        else if (i < N && pattern[j] != file_path[i]) {
            if (j != 0) {
                j = lps[j - 1];
            }
            else {
                i = i + 1;
            }
        }
    }
    free(lps);
    return result;
}

int main(){
    const char txt[] = "geeksforgeeks";
    const char pat[] = "geeks";
    int count;

    // Call KMPSearch and get the array of occurrences
    int* result = KMPSearch(pat, txt, &count);

    // Print all the occurrences (1-based indices)
    for (int i = 0; i < count; i++) {
       printf("Pattern found at index: %d ", result[i]);
       printf("\n");
  
    }
    printf("\n");

    // Free the allocated memory
    free(result);

    return 0;
}