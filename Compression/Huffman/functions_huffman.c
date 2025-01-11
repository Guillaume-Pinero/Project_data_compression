#include "functions_huffman.h"
#include <stdio.h>  // Null, I/O
#include <stdlib.h> // exit
#include <unistd.h>
#include <stdbool.h>
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

////////////////
/// COMPRESS ///
////////////////

o_huffmanNode *createEndNode(char o_letter, uint32_t ui32_freq)
{
    o_huffmanNode *po_Node = malloc(sizeof(o_huffmanNode));
    po_Node->o_letter = o_letter;
    po_Node->ui32_freq = ui32_freq;
    po_Node->po_huffman_path = "";
    po_Node->po_left = NULL;
    po_Node->po_right = NULL;
    // printf("%c %d\n",o_letter,ui32_freq);
    return po_Node;
};

o_huffmanNode *createMiddleNode(o_huffmanNode *po_left, o_huffmanNode *po_right)
{
    o_huffmanNode *po_Node = malloc(sizeof(o_huffmanNode));
    po_Node->o_letter = '0';
    po_Node->po_right = po_right;
    po_Node->po_left = po_left;
    if (po_left && po_right)
    {
        po_Node->ui32_freq = po_left->ui32_freq + po_right->ui32_freq;
    }
    else
    {
        po_Node->ui32_freq = 0;
        perror("At least one of the branch is NULL");
    }
    po_Node->po_huffman_path = "";
    return po_Node;
}

bool isEndTableOrLetterFound(uint16_t ui16_current_index, o_dictionnary o_dictionnary, int i_ascii_letter)
{
    if (ui16_current_index >= o_dictionnary.ui16_lenght_table)
    {
        return false;
    }
    else if (o_dictionnary.po_table_node[ui16_current_index]->o_letter == i_ascii_letter)
    {
        return false;
    }
    else
    {
        return true;
    }
}

o_dictionnary *readFileCompress(char *po_file_name)
{
    // Open file read mode
    FILE *po_file;
    po_file = fopen(po_file_name, "r");
    if (po_file)
    {
        // Instanciate dictionnary
        o_huffmanNode **po_node_table = NULL;
        o_dictionnary *o_dic = malloc(sizeof(o_dic));
        o_dic->po_table_node = po_node_table;
        o_dic->ui16_lenght_table = 0;
        int o_ascii_character;
        // Every iteration parses the file
        while ((o_ascii_character = fgetc(po_file)) != EOF)
        {
            // Going throught table
            uint16_t i = 0;
            while (isEndTableOrLetterFound(i, *o_dic, o_ascii_character))
            {
                i = i + 1;
            }
            // The letter isn't registered yet
            if (i >= o_dic->ui16_lenght_table)
            {
                o_huffmanNode *po_newNode = createEndNode(o_ascii_character, 1);
                static int a = 0;
                printf("New character : %c "BYTE_TO_BINARY_PATTERN" %d\n",o_ascii_character,BYTE_TO_BINARY(o_ascii_character),a++);
                o_dic->ui16_lenght_table++;
                o_huffmanNode **temp = (o_huffmanNode **)realloc(o_dic->po_table_node, o_dic->ui16_lenght_table * sizeof(o_huffmanNode *));
                o_dic->po_table_node = temp;
                o_dic->po_table_node[o_dic->ui16_lenght_table - 1] = po_newNode;
            }
            // Increment frequency of the letter found
            else if (o_dic->po_table_node[i]->o_letter == o_ascii_character)
            {
                o_dic->po_table_node[i]->ui32_freq++;
            }
        }
        printf("File read successfully\n");
        /*for(int i = 0; i < o_dictionnary.ui8_lenght_table; i++){
            printf("%p %c %d\n",o_dictionnary.po_table_node[i],o_dictionnary.po_table_node[i]->o_letter,o_dictionnary.po_table_node[i]->ui32_freq);
        }*/
        fclose(po_file);
        return o_dic;
    }
    else
    {
        printf("Impossible to read the file, this shall not be displayed.\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_FAILURE);
}

o_dictionnary *copyDictionnary(o_dictionnary *po_dictionnary_to_copy)
{
    // Create independent pointer and copy lenght
    o_dictionnary *po_new_dictionnary = malloc(sizeof(o_dictionnary));
    po_new_dictionnary->ui16_lenght_table = po_dictionnary_to_copy->ui16_lenght_table;

    // Tricky part is we want a dictionnary completly independant except for the pointers for the letters
    o_huffmanNode **po_new_table_node = malloc(po_dictionnary_to_copy->ui16_lenght_table * sizeof(o_huffmanNode *));
    po_new_dictionnary->po_table_node = po_new_table_node;
    for (int i = 0; i < po_dictionnary_to_copy->ui16_lenght_table; i++)
    {
        po_new_table_node[i] = po_dictionnary_to_copy->po_table_node[i];
    }

    return po_new_dictionnary;
}

o_huffmanNode *generateHuffmanTree(o_dictionnary *po_dictionnary)
{
    // Loop while there is at least 2 elements
    o_huffmanNode **po_table = po_dictionnary->po_table_node;
    uint8_t ui8_len = po_dictionnary->ui16_lenght_table;
    while (ui8_len >= 2)
    {
        // Find the 2 minimas of frequencies
        uint8_t ui8_min1 = 0;
        uint8_t ui8_min2 = 1;
        // First minimum
        for (uint8_t i = 2; i < ui8_len; i++)
        {
            if (po_table[ui8_min1]->ui32_freq > po_table[i]->ui32_freq)
            {
                ui8_min1 = i;
            }
        }
        // Second minimum
        for (uint8_t i = 2; i < ui8_len; i++)
        {
            if (po_table[ui8_min2]->ui32_freq > po_table[i]->ui32_freq && ui8_min1 != i)
            {
                ui8_min2 = i;
            }
        }

        //printf("min1 : %d min2 :%d\n", ui8_min1, ui8_min2);
        // Create a middle node to merge them
        o_huffmanNode *po_middle_node = createMiddleNode(po_table[ui8_min1], po_table[ui8_min2]);

        // Replace left node by middle node and remove right node
        po_table[ui8_min1] = po_middle_node;
        for (int8_t i = ui8_min2; i < ui8_len - 1; i++)
        {
            po_table[i] = po_table[i + 1];
        }

        // Decrement
        ui8_len--;

        // Reallocation of dynamic memory
        po_table = realloc(po_table, ui8_len * sizeof(o_huffmanNode *));
    }
    return po_table[0];
}

// Direction must be 0 or 1
void recursivePathLetters(o_huffmanNode *po_node, char *po_direction)
{
    // Middle node
    // printf("%s\n",po_direction);
    if (po_node->po_left && po_node->po_right)
    {
        char po_copy_direction[32] = "";
        strcpy(po_copy_direction, po_direction);
        strcat(po_direction, "0");
        strcat(po_copy_direction, "1");
        recursivePathLetters(po_node->po_left, po_direction);
        recursivePathLetters(po_node->po_right, po_copy_direction);
        // End node
    }
    else
    {
        // Dynamic pointers to save in the dictionnary
        char *po_dynamic_str = malloc((strlen(po_direction) + 1) * sizeof(char));
        strcpy(po_dynamic_str, po_direction);
        strcat(po_dynamic_str, "\0");
        po_node->po_huffman_path = po_dynamic_str;
        printf("NOEUD %s %c\n", po_node->po_huffman_path, po_node->o_letter);
    }
}

void codeHuffmanLetters(o_huffmanNode *po_root)
{
    // Middle node
    if (po_root->po_left && po_root->po_right)
    {
        char o_left_path[32] = "0";
        char o_right_path[32] = "1";
        recursivePathLetters(po_root->po_left, o_left_path);
        recursivePathLetters(po_root->po_right, o_right_path);
        // Case there is just one letter
    }
    else
    {
        po_root->po_huffman_path = "0";
        printf("0");
    }
}

void writeBitsFile(FILE *po_file_to_write, void *p_data, uint8_t ui8_number_bits)
{
    static uint8_t ui8_byte_buffer = 0;
    static uint8_t ui8_lenght_buffer = 0;
    if (ui8_number_bits > 0)
    {
        uint8_t *pui8_cast_data;
        uint8_t data_to_write;
        if (p_data)
        {
            pui8_cast_data = (uint8_t *)p_data;
            data_to_write = *pui8_cast_data;
        }
        else
        {
            printf("Trying to write with a null pointer.");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < ui8_number_bits; i++)
        {
            // Push the leftest byte of data to right side of buffer
            uint8_t ui8_mask_data = 1 << (ui8_number_bits - i - 1);
            uint8_t ui8_bit = data_to_write & ui8_mask_data;
            // printf("%d",ui8_bit);

            ui8_byte_buffer <<= 1;
            ui8_lenght_buffer++;
            if (ui8_bit > 0)
            {
                ui8_byte_buffer++;
            }

            // If lenght = 8 write the byte and reset buffet
            if (ui8_lenght_buffer == 8)
            {
                fwrite(&ui8_byte_buffer, 1, 1, po_file_to_write);
                ui8_byte_buffer = 0;
                ui8_lenght_buffer = 0;
                // printf("\nBit écrit\n");
            }
        }
    }
    else
    {
        if (ui8_lenght_buffer > 0)
        {
            // Insert last byte of body, adding the number of bits to the header
            uint8_t ui8_padding = 8 - ui8_lenght_buffer;
            ui8_byte_buffer <<= ui8_padding;
            fwrite(&ui8_byte_buffer, 1, 1, po_file_to_write);

            // Add prefix to indicate number of padding to complete the byte
            rewind(po_file_to_write);                     // Back to first byte, MUST BE LAST THING WRITTEN
            fwrite(&ui8_padding, 1, 1, po_file_to_write); // Replace padding = 0 to the correct value n
        }
        else
        {
            printf("\nbyte buffer lenght is 0.\n");
        }
    }
}

void generateFillCompressed(char *po_file_name, o_dictionnary *o_dictionnary_full)
{

    // Open file to read for compression of main body
    FILE *fo_file_to_read;
    fo_file_to_read = fopen(po_file_name, "r");

    // Create a file
    strcat(po_file_name, ".huffman");
    FILE *fo_new_file;
    fo_new_file = fopen(po_file_name, "w");
    printf("File %s generated successfully\n", po_file_name);

    // Generating HEADER

    // Bit padding to change
    uint8_t ui8_padding = 0;
    writeBitsFile(fo_new_file, &ui8_padding, 8);
    // Header : number of bit padding | ASCII letter + ui8 number of bits + encoded letter ... repeat last char for end of header
    for (uint8_t i = 0; i < o_dictionnary_full->ui16_lenght_table; i++)
    {
        // Letter
        writeBitsFile(fo_new_file, &(o_dictionnary_full->po_table_node[i]->o_letter), 8);
        // Number of bits for encodeded char
        char *po_path_letter = o_dictionnary_full->po_table_node[i]->po_huffman_path;
        uint8_t po_size_letter = strlen(po_path_letter);
        writeBitsFile(fo_new_file, &po_size_letter, 8);

        for (int j = 0; j < po_size_letter; j++)
        {
            uint8_t ui8_bit = po_path_letter[j] - '0';
            writeBitsFile(fo_new_file, &ui8_bit, 1);
            // printf("%d",ui8_bit);
        }
        // printf("\n");
    }
    // Repeat last character to indicate it's the main body !
    writeBitsFile(fo_new_file, &(o_dictionnary_full->po_table_node[o_dictionnary_full->ui16_lenght_table - 1]->o_letter), 8);

    // MAIN BODY
    // Read each ASCII characters to write in int compressed file
    int o_ascii_character;
    while ((o_ascii_character = fgetc(fo_file_to_read)) != EOF)
    {
        // Find in the dictionnary the letter
        int i = 0;
        bool isLetterFound = false;
        while ((i < o_dictionnary_full->ui16_lenght_table) && !isLetterFound)
        {
            if ((char) o_ascii_character == o_dictionnary_full->po_table_node[i]->o_letter)
            {
                isLetterFound = true;
            }
            else
            {
                i++;
            }
        }
        if (!isLetterFound)
        {
            printf("Letter not found.");
            exit(EXIT_FAILURE);
        }
        else
        {
            char *po_path_letter = o_dictionnary_full->po_table_node[i]->po_huffman_path;
            uint8_t po_size_letter = strlen(po_path_letter);

            for (int j = 0; j < po_size_letter; j++)
            {
                uint8_t ui8_bit = po_path_letter[j] - '0';
                writeBitsFile(fo_new_file, &ui8_bit, 1);
                printf("%d",ui8_bit);
            }
            printf("%d written \n",o_ascii_character);
        }
    }
    // Sending last chunk of buffer data
    writeBitsFile(fo_new_file, NULL, 0);
    fclose(fo_new_file);
    fclose(fo_file_to_read);
}

void recursiveFreeHuffmanTree(o_huffmanNode *po_to_free)
{
    if (po_to_free->po_left)
    {
        recursiveFreeHuffmanTree(po_to_free->po_left);
    }
    if (po_to_free->po_right)
    {
        recursiveFreeHuffmanTree(po_to_free->po_right);
    }
    else
    {
        free(po_to_free->po_huffman_path);
    }
    free(po_to_free);
}

void compressHuffman(char *po_file_name)
{
    o_dictionnary *o_dictionnary_full = readFileCompress(po_file_name);
    o_dictionnary *po_dictionnary_copy = copyDictionnary(o_dictionnary_full);
    o_huffmanNode *po_root = generateHuffmanTree(o_dictionnary_full);
    codeHuffmanLetters(po_root);
    generateFillCompressed(po_file_name, po_dictionnary_copy);
    // Free pointers
    recursiveFreeHuffmanTree(po_root);
    free(o_dictionnary_full->po_table_node);
    free(po_dictionnary_copy->po_table_node);
    free(o_dictionnary_full);
    free(po_dictionnary_copy);
}

//////////////////
/// DECOMPRESS ///
//////////////////

int readBitsFile(FILE *po_file, uint8_t ui8_number_of_bits)
{
    static uint8_t ui8_byte = 0;
    static int i_next_byte = 0;
    static uint8_t ui8_remaining_bits = 0;
    static bool isReadBitsFileInit = false;
    static uint8_t ui8_padding = 0;
    static int compteur = 0;
    if (!isReadBitsFileInit)
    {
        i_next_byte = fgetc(po_file);
        printf(" %d",compteur++);
        ui8_padding = i_next_byte;
        isReadBitsFileInit = true;
    }
    if (ui8_remaining_bits >= ui8_number_of_bits)
    {
        if (i_next_byte == EOF && ui8_remaining_bits == ui8_padding)
        {
            return EOF;
        }
        uint8_t ui8_bits_return = ui8_byte;
        ui8_bits_return >>= 8 - ui8_number_of_bits;
        ui8_byte <<= ui8_number_of_bits;
        ui8_remaining_bits = ui8_remaining_bits - ui8_number_of_bits;
        /*if (ui8_number_of_bits == 1)
        {
            printf("%c\n", (ui8_bits_return) & 0x01 ? '1' : '0');
        }
        else
        {
            printf("" BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(ui8_bits_return));
        }*/
        return ui8_bits_return;
    }
    else if (ui8_number_of_bits <= 8)
    {
        // Read a new byte
        int i_test = i_next_byte;
        i_next_byte = fgetc(po_file);
                printf(" %d\n",compteur++);
        if (i_test != EOF)
        {
            uint8_t ui8_new_ascii = i_test;

            uint8_t ui8_bits_return = ui8_new_ascii;
            ui8_bits_return >>= (8 - (ui8_number_of_bits - ui8_remaining_bits));

            ui8_byte >>= (8 - (ui8_number_of_bits));
            ui8_bits_return += ui8_byte;
            ui8_new_ascii <<= (ui8_number_of_bits - ui8_remaining_bits);
            ui8_byte = ui8_new_ascii;
            ui8_remaining_bits = ui8_remaining_bits + 8 - ui8_number_of_bits;
            /*if (ui8_number_of_bits == 1)
            {
                printf("%c\n", (ui8_bits_return) & 0x01 ? '1' : '0');
            }
            else
            {
                printf(BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(ui8_bits_return));
            }*/
            return ui8_bits_return;
        }
        else
        {
            return EOF;
        }
    }
    else
    {
        printf("More than 8 bits asked, abort.\n");
        exit(EXIT_FAILURE);
    }
}

o_dictionnary *readFileDecompressHeader(FILE *po_file)
{
    // Create dictionnary
    o_dictionnary *po_new_dictionnary = malloc(sizeof(o_dictionnary));
    o_huffmanNode **po_table = malloc(sizeof(o_huffmanNode *));
    po_new_dictionnary->ui16_lenght_table = 0;
    po_new_dictionnary->po_table_node = po_table;

    // Read buffer padding
    uint8_t o_ascii_character;
    o_ascii_character = readBitsFile(po_file, 8);
    printf("padding = %d\n", o_ascii_character);

    // Loop Letters
    bool b_letter_redundancy = false;
    uint8_t ui8_i = 0;
    while (!b_letter_redundancy)
    {
        // Read Letter
        char o_letter = readBitsFile(po_file, 8);
        if (ui8_i == 0)
        {
            // REALLOC + dynamic string + init end node
            po_new_dictionnary->po_table_node = realloc(po_new_dictionnary->po_table_node, (ui8_i + 1) * sizeof(o_huffmanNode *));
            o_huffmanNode *po_node = createEndNode(o_letter, 1);
            po_new_dictionnary->ui16_lenght_table++;

            // Read nb of bit lenght
            uint8_t ui8_letter_lenght = readBitsFile(po_file, 8);
            char *po_dynamic_string = malloc((ui8_letter_lenght + 1) * sizeof(char));
            po_new_dictionnary->po_table_node[ui8_i] = po_node;
            po_new_dictionnary->po_table_node[ui8_i]->po_huffman_path = po_dynamic_string;
            // Read code
            for (int8_t j = 0; j < ui8_letter_lenght; j++)
            {
                int8_t ui8_bit = readBitsFile(po_file, 1);
                po_dynamic_string[j] = ui8_bit + '0';
            }
            printf("new letter %s %c\n", po_dynamic_string, po_new_dictionnary->po_table_node[ui8_i]->o_letter);
        }
        else if (ui8_i > 0 && (po_new_dictionnary->po_table_node[ui8_i - 1]->o_letter != o_letter))
        {
            // REALLOC + dynamic string + init end node
            po_new_dictionnary->po_table_node = realloc(po_new_dictionnary->po_table_node, (ui8_i + 1) * sizeof(o_huffmanNode *));
            o_huffmanNode *po_node = createEndNode(o_letter, 1);
            po_new_dictionnary->ui16_lenght_table++;

            // Read nb of bit lenght
            uint8_t ui8_letter_lenght = readBitsFile(po_file, 8);
            char *po_dynamic_string = malloc((ui8_letter_lenght + 1) * sizeof(char));
            po_new_dictionnary->po_table_node[ui8_i] = po_node;
            po_new_dictionnary->po_table_node[ui8_i]->po_huffman_path = po_dynamic_string;
            // Read code
            for (int8_t j = 0; j < ui8_letter_lenght; j++)
            {
                int8_t ui8_bit = readBitsFile(po_file, 1);
                po_dynamic_string[j] = ui8_bit + '0';
            }
            printf("new letter %s %c\n", po_dynamic_string, po_new_dictionnary->po_table_node[ui8_i]->o_letter);
        }
        else
        {
            b_letter_redundancy = true;
            printf("Letter redundancy : %c",o_letter);
        }
        ui8_i++;
    }
    return po_new_dictionnary;
}

void readFileDecompressBody(FILE *po_file, o_dictionnary *po_dictionnary_full)
{
    char po_string_read[32] = "";
    bool isEOF = false;

    FILE *po_file_original;
    po_file_original = fopen("poeme.txt.huffman.txt", "w");

    while (!isEOF)
    {
        // Read next bit
        int i_next_bit = readBitsFile(po_file, 1);
        if (i_next_bit != EOF)
        {
            uint8_t ui8_bit = i_next_bit;
            char o_bit = ui8_bit + '0';
            strcat(po_string_read, &o_bit);
            // Letter found in dictionnary
            bool isLetterFound = false;
            uint8_t ui8_i = 0;
            while (ui8_i < po_dictionnary_full->ui16_lenght_table && !isLetterFound)
            {
                if (!strcmp(po_dictionnary_full->po_table_node[ui8_i]->po_huffman_path, po_string_read))
                {
                    isLetterFound = true;
                    printf("letter %c found\n", po_dictionnary_full->po_table_node[ui8_i]->o_letter);
                    strcpy(po_string_read, "");
                }
                else
                {
                    ui8_i++;
                }
            }
            if (isLetterFound)
            {
                // Write in file the letter
                fwrite(&po_dictionnary_full->po_table_node[ui8_i]->o_letter, 1, 1, po_file_original);
                printf("letter %c wrote\n", po_dictionnary_full->po_table_node[ui8_i]->o_letter);
            }
            // Not found a letter
        }
        else
        {
            isEOF = true;
            fclose(po_file_original);
        }
    }
}

void decompressHuffman(char *po_filename)
{
    // Open file
    FILE *po_file_unzip;
    po_file_unzip = fopen(po_filename, "r");

    o_dictionnary *po_dictionnary = readFileDecompressHeader(po_file_unzip);
    readFileDecompressBody(po_file_unzip, po_dictionnary);
    printf("Decompression success\n");

    fclose(po_file_unzip);
    for (uint8_t ui8_i = 0; ui8_i < po_dictionnary->ui16_lenght_table; ui8_i++)
    {
        free(po_dictionnary->po_table_node[ui8_i]->po_huffman_path);
        free(po_dictionnary->po_table_node[ui8_i]);
    }
    free(po_dictionnary->po_table_node);
    free(po_dictionnary);
}