#include <stdio.h>
#include <string>
#include <iostream>
#include "search.h"

int search(std::string& word) {
    FILE *keys = fopen("keys.bin", "rb");
    if (! keys) {
        perror("keys");
        return -1;
    }

    FILE *values = fopen("values.bin", "rb");
    if (! values) {
        perror("values");
        return -1;
    }

    size_t max_length_word;
    char *buffer;
    unsigned long values_addr;

    if (fread(&max_length_word, sizeof(size_t), 1, keys) != 1) {
        perror("1");
        return -1;
    }

    buffer = new char [max_length_word];
    fseek(keys, (max_length_word + sizeof(unsigned long)) * 700, SEEK_CUR);

    if (fread(buffer, sizeof(char), max_length_word, keys) != max_length_word) {
        perror("2");
        return -1;
    }
    if (fread(&values_addr, sizeof(unsigned long), 1, keys) != 1) {
        perror("3");
        return -1;
    }
    std::cout << buffer << std::endl;

    // values.bin file
    fseek(values, values_addr, SEEK_SET);
    size_t urls_quantity;
    if (fread(&urls_quantity, sizeof(size_t), 1, values) != 1) {
        perror("");
        return -1;
    }
    std::cout << urls_quantity << std::endl;

    delete buffer;
    fclose(values);
    fclose(keys);
    return 0;
}
