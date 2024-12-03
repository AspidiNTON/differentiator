#include "expression.h"

#define cur(str) str->buffer[str->index] // current string symbol

#define inc(str) ++str->index // increase index by 1

const char* functionNames[] = {
    "exp",
    "ln",
    "sin",
    "cos",
    "tan",
    "cot"
};

Function functions[] = {
    {"exp", EXP},
    {"ln", LN},
    {"sin", SIN},
    {"cos", COS}
};

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
    printf("%c\n", str.buffer[str.index]);
    if (str.buffer[str.index] != '$') {
        free((void*)str.buffer);
        printErr("Expression must be terminated by '$' symbol\n");
        return NULL;
    }
    free((void*)str.buffer);
    return tree;
}

Node* getExpression(ReaderData* str){
    //printf("%s, %d\n", __func__, str->index);
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
    //printf("%s, %d\n", __func__, str->index);
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
    //printf("%s, %d\n", __func__, str->index);
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
    //printf("%s, %d\n", __func__, str->index);
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
        return getFunction(str);
    }
}


Node* getFunction(ReaderData* str){
    //printf("%s, %d\n", __func__, str->index);
    Node* node = getNumber(str);
    if (node != NULL) return node;
    if (cur(str) >= 'a' && cur(str) <= 'z' &&
      !(str->buffer[str->index + 1] >= 'a' &&
        str->buffer[str->index + 1] <= 'z')) return createVariableNode(str->buffer[str->index++]);
    else {
        for (int i = 0; i < (int)(sizeof(functions) / sizeof(functions[0])); ++i) {
            int j = 0;
            while (str->buffer[str->index + j] == functions[i].name[j] && functions[i].name[j] != '\0') ++j;
            if (functions[i].name[j] == '\0'){
                str->index += j;
                node = createFunctionNode(functions[i].functionType);
                node->right = getBrackets(str);
                return node;
            }
        }
        printErr("Invalid function at: %10s...\n", str->buffer + str->index);
        return NULL;
    }
}

Node* getNumber(ReaderData* str){
    //printf("%s, %d\n", __func__, str->index);
    int prevIndex = str->index;
    int number = 0;
    while (cur(str) >= '0' && cur(str) <= '9') {
        number = number * 10 + (cur(str) - '0');
        inc(str);
    }
    if (prevIndex == str->index) return NULL;
    return createConstNode(number);
}

Node* getDerivative(Node* node, char variableName){
    if (node == NULL) {
        printErr("Nullptr node recieved\n");
        return NULL;
    }
    switch (node->type){
        case CONST_TYPE:
            return createConstNode(0);
        case VARIABLE_TYPE:
            if (node->data.variableName == variableName) return createConstNode(1);
            else return createConstNode(0);
        case BINARY_OPERATOR:
            return getOperatorDerivative(node, variableName);
        case UNARY_FUNCTION:    
            return getFunctionDerivative(node, variableName);
        default:
            printErr("How did we get here?\n");
            return NULL;
    }
}

Node* getOperatorDerivative(Node* node, char variableName){
    Node* result = NULL;
    switch (node->data.operatorType){   
        case ADD:
            result = createOperatorNode(ADD);
            result->left = getDerivative(node->left, variableName);
            result->right = getDerivative(node->right, variableName);
            return result;
        case SUB:
            result = createOperatorNode(SUB);
            result->left = getDerivative(node->left, variableName);
            result->right = getDerivative(node->right, variableName);
            return result;
        case MUL:
            result = createOperatorNode(ADD);
            result->left = createOperatorNode(MUL);
            result->right = createOperatorNode(MUL);
            result->left->left = getDerivative(node->left, variableName);
            result->left->right = createNodeCopy(node->right);
            result->right->left = createNodeCopy(node->left);
            result->right->right = getDerivative(node->right, variableName);
            return result;
        case DIV:
            result = createOperatorNode(DIV);
            result->right = createOperatorNode(POW);
            result->right->left = createNodeCopy(node->right);
            result->right->right = createConstNode(2);
            result->left = createOperatorNode(SUB);
            result->left->left = createOperatorNode(MUL);
            result->left->right = createOperatorNode(MUL);
            result->left->left->left = getDerivative(node->left, variableName);
            result->left->left->right = createNodeCopy(node->right);
            result->left->right->left = createNodeCopy(node->left);
            result->left->right->right = getDerivative(node->right, variableName);
            return result;
        case POW:
            if (node->right->type == CONST_TYPE) {
                if (node->right->data.constValue == 0) return createConstNode(0);
                result = createOperatorNode(MUL);
                result->left = createOperatorNode(POW);
                result->left->left = createNodeCopy(node->left);
                result->left->right = createConstNode(node->right->data.constValue - 1);
                result->right = createOperatorNode(MUL);
                result->right->left = createConstNode(node->right->data.constValue);
                result->right->right = getDerivative(node->left, variableName);
            }
            else {
                Node* tmp = createFunctionNode(EXP);
                tmp->right = createOperatorNode(MUL);
                tmp->right->left = createFunctionNode(LN);
                tmp->right->left->right = createNodeCopy(node->left);
                tmp->right->right = createNodeCopy(node->right);
                result = getDerivative(tmp, variableName);
                destroyNode(tmp);
            }
            return result;
        default:
            printErr("hell nah bro wtf do you want from me\n");
            return NULL;
    }
}

Node* getFunctionDerivative(Node* node, char variableName){
    //printExpression(stdout, node); putchar('\n');
    Node* result;
    switch (node->data.functionType){
        case EXP:
            result = createOperatorNode(MUL);
            result->left = createNodeCopy(node);
            result->right = getDerivative(node->right, variableName);
            return result;
        case LN:
            result = createOperatorNode(DIV);
            result->left = getDerivative(node->right, variableName);
            result->right = createNodeCopy(node->right);
            return result;
        case SIN:
            result = createOperatorNode(MUL);
            result->left = createFunctionNode(COS);
            result->left->right = createNodeCopy(node->right);
            result->right = getDerivative(node->right, variableName);
            return result;
        default:
            printErr("I ain't even got a derivative for this one man what the hell\n");
            return NULL;
    }
}

void printNode(FILE* filePtr, Node* node){
    if (node == NULL || filePtr == NULL) return;
    switch (node->type){
        case CONST_TYPE:
            printf("%lg", node->data.constValue);
            break;
        case VARIABLE_TYPE:
            putchar(node->data.variableName);
            break;
        case BINARY_OPERATOR:
            putchar(operatorNames[node->data.operatorType]);
            break;
        case UNARY_FUNCTION:
            printf("%s", functionNames[node->data.functionType]);
            break;
        default:
            printErr("Erm whatthesigma\n");
    }
}

void printExpression(FILE* filePtr, Node* node){
    if (node == NULL || filePtr == NULL) return;
    if (node->left != NULL && node->right != NULL) putchar('(');
    if (node->left != NULL) printExpression(filePtr, node->left);
    printNode(filePtr, node);
    if (node->right != NULL) printExpression(filePtr, node->right);
    if (node->left != NULL && node->right != NULL) putchar(')');
}


