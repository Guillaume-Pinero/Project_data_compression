/**
 * @file huffman_main.c
 * @brief Main file for the Huffman compression project.
 *
 * This file contains the main function which serves as the entry point for the program.
 * It currently prints "Hello, World!" to the console.
 *
 * @author Guillaume Pinero
 * @date 2025
 */

#include <stdio.h>   // Standard input/output library for printing to the console
#include <unistd.h>  // system call wrapper functions such as fork, pipe, read, write, etc
#include <string.h>  // Using strings
#include <stdbool.h> // Using booleans
#include <stdlib.h>  // exit

#include "functions_huffman.h" // headers for Huffman compression

/// @brief Check if the command line is correct with an existing file
/// @param argc
/// @param argv
void checkFilename(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Write the name of a file you want to compress using Huffman compression.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Assert the file exists
        if (access(argv[1], F_OK) == 0)
        {
            // file exists
            printf("%s exists !\n", argv[1]);
        }
        else
        {
            // file doesn't exist
            printf("%s  : file doesn't exist !\n",argv[1]);
            exit(EXIT_FAILURE);
        }
    }
}

/// @brief Check if the file is already compressed with a .huffman suffix
/// @param po_file_name Name of the file
/// @return true if file already compressed
bool isFileCompressed(char *po_file_name)
{
    // If filname too short, it's not already compressed
    const char *po_extension_name = ".huffman";
    if (strlen(po_file_name) <= strlen(po_extension_name))
    {
        return false;
    }
    else
    {
        // We check if the extension name is here
        char *p_string = strstr(po_file_name, po_extension_name);
        if (p_string == NULL)
        {
            printf("Compression of %s\n", po_file_name);
            return false;
        }
        else
        {
            printf("Decompression of %s\n", po_file_name);
            return true;
        }
    }
}

int main(int argc, char **argv)
{
    checkFilename(argc, argv);
    bool b_file_compressed = isFileCompressed(argv[1]);
    if (b_file_compressed)
    {
        // Decompression
        decompressHuffman(argv[1]);
    }
    else
    {
        // Compression
        compressHuffman(argv[1]);
    }
    return EXIT_SUCCESS;
}