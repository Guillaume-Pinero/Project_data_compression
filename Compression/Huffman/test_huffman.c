#include "functions_huffman.h"
#include <assert.h>
#include <stdio.h>  // NULL
#include <stdlib.h> // free
#include <string.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)         \
    ((byte) & 0x80 ? '1' : '0'),     \
        ((byte) & 0x40 ? '1' : '0'), \
        ((byte) & 0x20 ? '1' : '0'), \
        ((byte) & 0x10 ? '1' : '0'), \
        ((byte) & 0x08 ? '1' : '0'), \
        ((byte) & 0x04 ? '1' : '0'), \
        ((byte) & 0x02 ? '1' : '0'), \
        ((byte) & 0x01 ? '1' : '0')

void createEndNodeTest()
{
    // createEndNode
    o_huffmanNode *po_node_c = createEndNode('c', 1);
    assert(po_node_c->o_letter == 'c');
    assert(po_node_c->ui32_freq == 1);

    o_huffmanNode *po_node_a = createEndNode('a', 1);
    assert(po_node_a != po_node_c);

    o_huffmanNode *po_node_u = createEndNode('u', 45);

    free(po_node_c);
    free(po_node_a);
    free(po_node_u);

    printf("createEndNode successful !\n");
}

void createMiddleNodeTest()
{
    // createMiddleNode
    o_huffmanNode *po_node_c = createEndNode('c', 1);
    o_huffmanNode *po_node_a = createEndNode('a', 1);
    o_huffmanNode *po_node_u = createEndNode('u', 45);

    o_huffmanNode *po_node_middle = createMiddleNode(po_node_a, po_node_c);
    assert(po_node_middle->ui32_freq == po_node_a->ui32_freq + po_node_c->ui32_freq);
    assert(po_node_middle->po_left == po_node_a);
    assert(po_node_middle->po_right == po_node_c);

    o_huffmanNode *po_root_node = createMiddleNode(po_node_middle, po_node_u);
    assert(po_node_middle != po_root_node);
    assert(po_root_node->ui32_freq == po_node_middle->ui32_freq + po_node_u->ui32_freq);

    free(po_node_c);
    free(po_node_a);
    free(po_node_u);
    free(po_node_middle);
    free(po_root_node);
    printf("createMiddleNode successful !\n");
}

void readFileTest()
{
    // Input file AAAAbbbcccdd
    o_dictionnary *po_test_dictionnary = readFileCompress("file_to_compress.txt");
    assert(po_test_dictionnary->ui16_lenght_table == 4);

    assert(po_test_dictionnary->po_table_node[0]->o_letter == 'A');
    assert(po_test_dictionnary->po_table_node[1]->o_letter == 'b');
    assert(po_test_dictionnary->po_table_node[2]->o_letter == 'c');
    assert(po_test_dictionnary->po_table_node[3]->o_letter == 'd');

    assert(po_test_dictionnary->po_table_node[0]->ui32_freq == 4);
    assert(po_test_dictionnary->po_table_node[1]->ui32_freq == 3);
    assert(po_test_dictionnary->po_table_node[2]->ui32_freq == 3);
    assert(po_test_dictionnary->po_table_node[3]->ui32_freq == 2);

    for (int i = 0; i < po_test_dictionnary->ui16_lenght_table; i++)
    {
        free(po_test_dictionnary->po_table_node[i]);
    }
    free(po_test_dictionnary->po_table_node);
    free(po_test_dictionnary);
    printf("readFileTest successful !\n");
}

void generateHuffmanTreeTest()
{
    o_dictionnary *po_test_dictionnary = readFileCompress("file_to_compress.txt");
    o_huffmanNode *po_root = generateHuffmanTree(po_test_dictionnary);
    assert(po_test_dictionnary->ui16_lenght_table == 4);
    assert(po_root->po_left);
    assert(po_root->po_right);
    assert(po_root->po_left->po_left);
    assert(po_root->po_left->po_right);
    assert(po_root->po_right->po_left);
    assert(po_root->po_right->po_right);
    printf("%c\n", po_root->po_left->po_left->o_letter);   // A
    printf("%c\n", po_root->po_right->po_right->o_letter); // b
    printf("%c\n", po_root->po_left->po_right->o_letter);  // c
    printf("%c\n", po_root->po_right->po_left->o_letter);  // d

    free(po_root->po_left->po_left);
    free(po_root->po_left->po_right);
    free(po_root->po_right->po_left);
    free(po_root->po_right->po_right);
    free(po_root->po_right);
    free(po_root->po_left);
    free(po_test_dictionnary->po_table_node);
    free(po_test_dictionnary);
    printf("generateHuffmanTreeTest successful !\n");
}

void codeHuffmanLettersTest()
{
    o_dictionnary *po_test_dictionnary = readFileCompress("file_to_compress.txt");
    o_huffmanNode *po_root = generateHuffmanTree(po_test_dictionnary);
    codeHuffmanLetters(po_root);

    assert(!strcmp(po_root->po_left->po_left->po_huffman_path, "00"));   // A
    assert(!strcmp(po_root->po_right->po_right->po_huffman_path, "11")); // b
    assert(!strcmp(po_root->po_left->po_right->po_huffman_path, "01"));  // c
    assert(!strcmp(po_root->po_right->po_left->po_huffman_path, "10"));  // d

    recursiveFreeHuffmanTree(po_root);
    free(po_test_dictionnary->po_table_node);
    free(po_test_dictionnary);

    printf("codeHuffmanLettersTest successful !\n");
}

void printFileBinary()
{
    FILE *po_file;
    po_file = fopen("poeme.txt.huffman", "r");
    uint8_t ui8_ascii;
    while ((ui8_ascii = fgetc(po_file)) != EOF && ui8_ascii != 255)
    {
        printf(BYTE_TO_BINARY_PATTERN "%c\n", BYTE_TO_BINARY(ui8_ascii), ui8_ascii);
    }
}

int main()
{
    if (0)
    {
        createEndNodeTest();

        createMiddleNodeTest();

        readFileTest();

        generateHuffmanTreeTest();

        codeHuffmanLettersTest();

        printf("All tests passed compression!\n");

        
    }
    else
    {
        printFileBinary();
    }
    return 0;
}