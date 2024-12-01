#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdlib.h>

#include "default.h"

enum NodeType{
    CONST_TYPE,
    VARIABLE_TYPE,
    BINARY_OPERATOR,
    UNARY_FUNCTION
};

enum BinaryOperator{
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    //LOG
};

enum UnaryFunction{
    EXP,
    LN,
    SIN,
    COS,
    TAN,
    COT,
    ARCSIN,
    ARCCOS,
    ARCTAN,
    ARCCOT,
    SINH,
    COSH,
    TANH,
    COTH,
    ARCSINH,
    ARCCOSH,
    ARCTANH,
    ARCCOTH
};

union NodeData{
    double constValue;
    char variableName;
    BinaryOperator operatorType;
    UnaryFunction functionType;
};

struct Node{
    NodeType type;
    NodeData data;
    Node* left;
    Node* right;
};

/*struct KnownVariables{
    uint32_t bitmask;
    double values[32];
};*/


//typedef Node* (*CalculationFunction)(Node*, KnownVariables*);

struct Function{
    char name[8];
    UnaryFunction functionType;
    //CalculationFunction getValue;
};



Function functions[] = {
    {"exp", EXP},
    {"ln", LN}
};

struct ReaderData{
    const char* buffer;
    int index;
};

Node* createNode(NodeType type, NodeData data, Node* left, Node* right);

Node* createConstNode(double value);

Node* createVariableNode(char variableName);

Node* createOperatorNode(BinaryOperator operatorType);

Node* createFunctionNode(UnaryFunction functionType);

void destroyNode(Node* node);

Node* readExpressionFromFile(const char* filename);

Node* getExpression(ReaderData*);

Node* createNodeCopy(Node*);

#endif