#include "expression.h"

#define cur(str) str->buffer[str->index] // current string symbol

#define inc(str) ++str->index // increase index by 1


Node* createNode(NodeType type, NodeData data, Node* left, Node* right){
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (node == NULL) {
        printErr("Unable to calloc const node\n");
        return NULL;
    }
    node->type = type;
    node->data = data;
    node->left = left;
    node->right = right;
    return node;
}

Node* createConstNode(double value){
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (node == NULL) {
        printErr("Unable to calloc const node\n");
        return NULL;
    }
    node->type = CONST_TYPE;
    node->data.constValue = value;
    return node;
}

Node* createVariableNode(char variableName){
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (node == NULL) {
        printErr("Unable to calloc variable node\n");
        return NULL;
    }
    node->type = VARIABLE_TYPE;
    node->data.variableName = variableName;
    return node;
}

Node* createOperatorNode(BinaryOperator operatorType){
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (node == NULL) {
        printErr("Unable to calloc operator node\n");
        return NULL;
    }
    node->type = BINARY_OPERATOR;
    node->data.operatorType = operatorType;
    return node;
}

Node* createFunctionNode(UnaryFunction functionType){
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (node == NULL) {
        printErr("Unable to calloc function node\n");
        return NULL;
    }
    node->type = UNARY_FUNCTION;
    node->data.functionType = functionType;
    return node;
}

void destroyNode(Node* node){
    if (node == NULL) return;
    destroyNode(node->left);
    destroyNode(node->right);
    free(node);
}

Node* createNodeCopy(Node* node){
    if (node == NULL) return NULL;
    Node* newNode = (Node*)calloc(1, sizeof(Node));
    newNode->data = node->data;
    newNode->type = node->type;
    newNode->left = createNodeCopy(node->left);
    newNode->right = createNodeCopy(node->right);
    return newNode;
}

Node* readExpressionFromFile(const char* filename){
    ReaderData str;
    str.buffer = readTextFromFile(filename);
    if (str.buffer == NULL) return NULL;
    str.index = 0;
    Node* tree = getExpression(&str);
    if (str.buffer[str.index] != '$') {
        free((void*)str.buffer);
        printErr("Expression must be terminated by '$' symbol\n");
        return NULL;
    }
    free((void*)str.buffer);
    return tree;
}

Node* getExpression(ReaderData* str){
    Node* first = getSummand(str);
    while (cur(str) == '+' || cur(str) == '-') {
        NodeData unionSucks;
        if (cur(str) == '+') unionSucks.operatorType = ADD;
        else unionSucks.operatorType = SUB;
        inc(str);
        Node* second = getSummand(str);
        first = createNode(BINARY_OPERATOR, unionSucks, first, second);
    }
    return first;
}

Node* getSummand(ReaderData* str){
    Node* first = getMultiplier(str);
    while (cur(str) == '*' || cur(str) == '/') {
        NodeData unionSucks;
        if (cur(str) == '*') unionSucks.operatorType = MUL;
        else unionSucks.operatorType = DIV;
        inc(str);
        Node* second = getMultiplier(str);
        first = createNode(BINARY_OPERATOR, unionSucks, first, second);
    }
    return first;
}

Node* getMultiplier(ReaderData* str){
    Node* first = getBrackets(str);
    while (cur(str) == '^') {
        NodeData unionSucks;
        unionSucks.operatorType = POW;
        inc(str);
        Node* second = getBrackets(str);
        first = createNode(BINARY_OPERATOR, unionSucks, first, second);
    }
    return first;
}

Node* getBrackets(ReaderData* str){
    if (cur(str) == '('){
        inc(str);
        Node* node = getExpression(str);
        if (cur(str) != ')') {
            printErr("The closing bracket was not found\n");
            return NULL;
        }
        inc(str);
        return node;
    } else {
        inc(str);




    }
}


Node* getFunction(ReaderData* str){
    int prevIndex = str->index;
    Node* node = getNumber(str);
    if (str->index != prevIndex) return node;
    if (cur(str) >= 'a' && cur(str) <= 'z' &&
      !(str->buffer[str->index + 1] >= 'a' &&
        str->buffer[str->index + 1] <= 'z')) 
    {
        return createVariableNode(cur(str));
    }
}

Node* getNumber(ReaderData* str){
    int number = 0;
    while (cur(str) >= '0' && cur(str) <= '9') {
        number = number * 10 + (cur(str) - '0');
        inc(str);
    }
    return createConstNode(number);
}