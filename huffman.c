#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>

// Função para criar um novo nó da árvore
HuffmanNode *newNode(char data, unsigned freq)
{
    HuffmanNode *temp = (HuffmanNode *)malloc(sizeof(HuffmanNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

   int isLeaf(struct HuffmanNode* node) {
        return !(node->left) && !(node->right);
   }


HuffmanNode *build_huffman_tree(char data[], int freq[], int size)
{
    printf("Construindo a árvore de Huffman...\n");



    return newNode(data[0], freq[0]);
}

void print_codes(HuffmanNode *root, int arr[], int top)
{
    printf("Imprimindo códigos de Huffman...\n");
}
