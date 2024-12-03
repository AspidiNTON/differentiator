#include "dump.h"

#define TREE_FUNCTIONS_DUMP

static FILE* htmlFilePtr;
static int htmlImageCount;
const char dotFilename[80] = "logger/result.dot";
const char imageFilename[80] = "logger/output%d.svg";

bool createTreeDotFile(const Node* node, const char* outFilename);

void printTreeInDotFormat(const Node* node, FILE* file);

bool createSvgFromDot(const char* inFilename, const char* outFilename);

void myAssert(bool condition, const char *text_error, const char *file, const char *func, int line) {
    if (!(condition)) {
        printErr("%s in: %s -> %s -> %d\n", text_error, file, func, line);
        abort();
    }
}

bool initializeLogger(){
    //createSvgFromDot("123.txt", "234.txt & ");
    htmlImageCount = 0;
    htmlFilePtr = fopen("listLog.html", "w");
    if (htmlFilePtr == NULL) {
        printErr("Unable to open listLog.html...\n");
        return false;
    }
    fprintf(htmlFilePtr, "<pre style=\"font-size: 20px\">\n");
    return true;
}

void printInLogger(const char* str){
    if (htmlFilePtr == NULL){
        printErr("Logger is not open!\n");
        return;
    }
    fprintf(htmlFilePtr, str);
}

void closeLogger(){
    fprintf(htmlFilePtr, "</pre>\n");
    fclose(htmlFilePtr);
}

bool dumpTree(Node* node){
    if (node == NULL) {
        printErr("Nullptr tree recieved\n");
        return false;
    }
    #ifdef TREE_FUNCTIONS_DUMP
    if (htmlFilePtr == NULL) {
        printErr("htmlFilePtr is uninitialized\n");
        return false;
    }
    printExpression(htmlFilePtr, node, false);
    putc('\n', htmlFilePtr);
    if (!createTreeDotFile(node, dotFilename)) return false;
    char currentImageFilename[200] = {};
    snprintf(currentImageFilename, 199, imageFilename, htmlImageCount);
    if (!createSvgFromDot(dotFilename, currentImageFilename)) return false;
    fprintf(htmlFilePtr, "<img src=\"%s\">\n\n\n\n", currentImageFilename);
    ++htmlImageCount;
    #endif
    return true;
}

void printNodeInFile(FILE* filePtr, const Node* node){
    if (node == NULL || filePtr == NULL) {
        printErr("Nullptr node/fileptr recieved\n");
        return;
    }
    switch (node->type){
    case CONST_TYPE:
        fprintf(filePtr, "\tx%p [color=black,shape=record,style=rounded,label=\"%lg\"]\n", node, node->data.constValue);
        break;
    case VARIABLE_TYPE:
        fprintf(filePtr, "\tx%p [color=purple,shape=record,style=rounded,label=\"%c\"]\n", node, node->data.variableName);
        break;
    case BINARY_OPERATOR:
        fprintf(filePtr, "\tx%p [color=blue,shape=record,style=rounded,label=\"%c\"]\n", node, operatorNames[node->data.operatorType]);
        break;
    case UNARY_FUNCTION:
        fprintf(filePtr, "\tx%p [color=red,shape=record,style=rounded,label=\"%s\"]\n", node, functionNames[node->data.functionType]);
        break;
    }
}

bool createTreeDotFile(const Node* node, const char* outFilename){
    if (outFilename == NULL) {
        printErr("Filename nullptr recieved\n");
        return false;
    }
    FILE* filePtr = fopen(outFilename, "w");
    if (filePtr == NULL) {
        printErr("Unable to open file: %s\n", outFilename);
        return false;
    }
    fprintf(filePtr, "digraph list{\n");
    printNodeInFile(filePtr, node);
    printTreeInDotFormat(node, filePtr);
    putc('}', filePtr);
    fclose(filePtr);
    return true;
}

void printTreeInDotFormat(const Node* node, FILE* file){
    if (node == NULL) {
        printErr("Nullptr node recieved\n");
        return;
    }
    if (node->left != NULL) {
        printNodeInFile(file, node->left);
        fprintf(file, "\tx%p->x%p\n", node, node->left);
        printTreeInDotFormat(node->left, file);
    }
    if (node->right != NULL) {
        printNodeInFile(file, node->right);
        fprintf(file, "\tx%p->x%p\n", node, node->right);
        printTreeInDotFormat(node->right, file);
    }
}

bool isFilenamaValid(const char* str){
    const char* tmp = str;
    while (*str != '\0') {
        if (isalpha(*str) || isdigit(*str) || *str == '/' || *str == ' ' || (*str == '.' && *(str + 1) != '.')) ++str;
        else{
            printErr("Invalid filename \"%s\", character: %c (char code: %d)\n", tmp, *str, (int)*str);
            return false;
        }
    }
    return true;
}

bool createSvgFromDot(const char* inFilename, const char* outFilename){
    if (inFilename == NULL || outFilename == NULL) {
        printErr("Filename nullptr recieved\n");
        return false;
    }
    char command[200] = {};
    if (!isFilenamaValid(inFilename) || !isFilenamaValid(outFilename)) return false;

    snprintf(command, 199, "dot -Tsvg %s -o %s", inFilename, outFilename); // snprintf FIXME:
    if (system(command) != 0) {
        printErr("Image creation command failed\n");
        return false;
    }
    return true;
}