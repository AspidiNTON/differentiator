#ifndef DEFAULT_H
#define DEFAULT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>

#define printErr(s, ...) fprintf(stderr, "\033[0;31m" s "\033[1;0m", ## __VA_ARGS__)

char* readTextFromFile(const char* filename);

#endif