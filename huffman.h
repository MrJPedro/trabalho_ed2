#ifndef HUFFMAN_H
#define HUFFMAN_H

// Estrutura para um nó da árvore de Huffman
typedef struct HuffmanNode
{
    char data;
    unsigned freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

// Funções para a lógica de Huffman
HuffmanNode *build_huffman_tree(char data[], int freq[], int size);
void print_codes(HuffmanNode *root, int arr[], int top);

#endif
