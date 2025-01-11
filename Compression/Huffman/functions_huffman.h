// functions_huffman.h

#ifndef FUNCTIONS_HUFFMAN_H
#define FUNCTIONS_HUFFMAN_H

#include <stdint.h> // For all ints uint...
#include <stdio.h>  // Null, I/O

//////////////
/// STRUCT ///
//////////////

// Define the structure for a Huffman tree node or end of branch
typedef struct o_huffmanNode o_huffmanNode;
struct o_huffmanNode
{
    char o_letter;         
    uint32_t ui32_freq;    // set frequency of a letter, if = 0, it's a not a letter at the end of the branch
    char * po_huffman_path; // string that shows the path to the letter e.g. "010" = left right left
    o_huffmanNode *po_left, *po_right;
};

typedef struct o_dictionnary o_dictionnary;
struct o_dictionnary
{
    o_huffmanNode ** po_table_node;
    uint16_t ui16_lenght_table;
};


////////////////
/// COMPRESS ///
////////////////

/// @brief Create an end of a tree branch (so null ptr for right left)
/// @param o_letter letter of the text
/// @param ui32_freq frequence of the letter
/// @return a pointer on a node that is suppose to be a part of a middle node
o_huffmanNode *createEndNode(char o_letter, uint32_t ui32_freq);

/// @brief create a middle node to sum up frequencies for Huffman algorithm
/// @param right node that is not null
/// @param left node that is not null
/// @return a pointer of node
o_huffmanNode *createMiddleNode(o_huffmanNode *po_left, o_huffmanNode *po_right);

/// @brief return a table of nodes for the frequency of the letter
/// @param file_name 
/// @return table of end nodes
o_dictionnary * readFileCompress(char * po_file_name);

/// @brief turns the table of end nodes to the huffman tree
/// @param po_file_name 
/// @return the ROOT of the Huffman tree
o_huffmanNode * generateHuffmanTree(o_dictionnary * o_dictionnary);

/// @brief Write for every letter its code, recursive function for simplicity
/// @param po_root root of the tree
void codeHuffmanLetters(o_huffmanNode * po_root);

void generateFillCompressed(char * po_file_name, o_dictionnary * o_dictionnary_full);

/// @brief Use predefined fonctions for the step of Huffman algorithm
/// @param po_file_name 
void compressHuffman(char *po_file_name);

//////////////////
/// DECOMPRESS ///
//////////////////

/// @brief Read file header and stops before main body
/// @param po_file_name 
/// @return full dictionnary
o_dictionnary * readFileDecompressHeader(FILE * po_file);

/// @brief Read main body and generate a decompress file
/// @param po_file file with its cursor at the begining of main body
/// @param po_dictionnary_full 
void readFileDecompressBody(FILE * po_file, o_dictionnary * po_dictionnary_full);

/// @brief Main fonction to unzip
/// @param po_filename 
void decompressHuffman(char * po_filename);

/////////////
/// TOOLS ///
/////////////

/// @brief Useful fonction for debugging
/// @param root central node of the tree
void printHuffmanTree(o_huffmanNode *root);

/// @brief Every pointer must be deleted
/// @param root central node of the tree
void recursiveFreeHuffmanTree(o_huffmanNode *root);

#endif // FUNCTIONS_HUFFMAN_H