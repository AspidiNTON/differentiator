#include "default.h"

char* readTextFromFile(const char* filename){
    if (filename == NULL) {
        printErr("Filename is nullptr\n");
        return NULL;
    }
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        printErr("Failed to read file stats\n");
        return NULL;
    }
    char* buff = (char*)calloc(sb.st_size + 1, sizeof(char));
    if (buff == NULL) {
        printErr("Failed to calloc buffer\n");
        return NULL;
    }
    FILE *filePtr = fopen(filename, "r");
    if (filePtr == NULL) {
        free(buff);
        printErr("Unable to open file\n");
        return NULL;
    }
    fread(buff, sizeof(char), sb.st_size, filePtr);
    fclose(filePtr);
    return buff;
}