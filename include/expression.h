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

const char operatorNames[] = "+-*/^";

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

Node* createNodeCopy(Node*);

Node* readExpressionFromFile(const char* filename);

Node* getExpression(ReaderData*);

Node* getSummand(ReaderData* str);

Node* getMultiplier(ReaderData* str);

Node* getBrackets(ReaderData* str);

Node* getFunction(ReaderData* str);

Node* getNumber(ReaderData* str);

Node* getDerivative(Node* node, char variableName);

Node* getOperatorDerivative(Node* node, char variableName);

Node* getFunctionDerivative(Node* node, char variableName);

void printExpression(FILE* filePtr, Node* node);



#endif