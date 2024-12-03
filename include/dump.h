#ifndef DUMP_H
#define DUMP_H

#include <stdio.h>
#include <ctype.h>

#include "expression.h"

#ifdef DEBUG
    #define ASSERT(condition, text_error) myAssert(condition, text_error, __FILE__, __func__, __LINE__);
#else
    #define ASSERT(condition, text_error) {}
#endif

void myAssert(bool condition, const char* text_error, const char* file, const char* func, int line);


void printInLogger(const char* str);

bool initializeLogger();

void closeLogger();

bool dumpTree(Node* node);

#endif