#include "expression.h"

#define cur(str) str->buffer[str->index] // current string symbol

#define inc(str) ++str->index // increase index by 1

Function functions[] = {
    {"exp", EXP},
    {"ln", LN},
    {"sin", SIN},
    {"cos", COS},
    {"tan", TAN},
};

const char *const functionNames[] = {
    "exp",
    "ln",
    "sin",
    "cos",
    "tan",
    /*"cot"
    "arcsin",
    "arccos",
    "arctan",
    "arccot",
    "sinh",
    "cosh",
    "tanh",
    "coth",
    "arcsinh",
    "arccosh",
    "arctanh",
    "arccoth"*/
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
        return getFunction(str);
    }
}


Node* getFunction(ReaderData* str){
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
        case COS:
            result = createOperatorNode(MUL);
            result->left = createFunctionNode(SIN);
            result->left->right = createNodeCopy(node->right);
            result->right = createOperatorNode(MUL);
            result->right->left = createConstNode(-1);
            result->right->right = getDerivative(node->right, variableName);
            return result;
        case TAN:
            result = createOperatorNode(MUL);
            result->left = getDerivative(node, variableName);
            result->right = createOperatorNode(POW);
            result->right->left = createNodeCopy(node->right);
            result->right->right = createConstNode(-2);
            return result;
        default:
            printErr("I ain't even got a derivative for this one man what the hell\n");
            return NULL;
    }
}



int getOperationPriority(Node* node){
    if (node == NULL) return -1;
    if (node->type != BINARY_OPERATOR) return 0;
    else {
        if (node->data.operatorType == POW) return 1;
        if (node->data.operatorType == MUL || node->data.operatorType == DIV) return 2;
        if (node->data.operatorType == ADD || node->data.operatorType == SUB) return 3;
    }
    return -1;
}

void printNode(FILE* filePtr, Node* node, bool latex){
    if (node == NULL || filePtr == NULL) return;
    switch (node->type){
        case CONST_TYPE:
            fprintf(filePtr, "%lg", node->data.constValue);
            break;
        case VARIABLE_TYPE:
            putc(node->data.variableName, filePtr);
            break;
        case BINARY_OPERATOR:
            putc(operatorNames[node->data.operatorType], filePtr);
            break;
        case UNARY_FUNCTION:
            fprintf(filePtr, "%s", functionNames[node->data.functionType]);
            break;
        default:
            printErr("Erm whatthesigma\n");
    }
}


void printExpression(FILE* filePtr, Node* node, bool latex){
    if (node == NULL || filePtr == NULL) return;
    if (node->type == VARIABLE_TYPE || node->type == CONST_TYPE) {
        printNode(filePtr, node, latex);
        return;
    }
    if (node->type == UNARY_FUNCTION) {
        printNode(filePtr, node, latex);
        if (node->right->type != CONST_TYPE && node->right->type != VARIABLE_TYPE) {
            putc('(', filePtr);
            printExpression(filePtr, node->right, latex);
            putc(')', filePtr);
        } else printExpression(filePtr, node->right, latex);
        return;
    }

    if (latex && node->data.operatorType == DIV){
        fprintf(filePtr, "\\frac{");
        printExpression(filePtr, node->left, latex);
        fprintf(filePtr, "}{");
        printExpression(filePtr, node->right, latex);
        putc('}', filePtr);
        return;
    }

    if (getOperationPriority(node->left) > getOperationPriority(node) || (getOperationPriority(node) == 1 && getOperationPriority(node->left) == 1)) {
        putc('(', filePtr);
        printExpression(filePtr, node->left, latex);
        putc(')', filePtr);
    } else printExpression(filePtr, node->left, latex);

    printNode(filePtr, node, latex);

    if (getOperationPriority(node->right) > getOperationPriority(node) || (getOperationPriority(node) == 1 && getOperationPriority(node->right) == 1)) {
        putc('(', filePtr);
        printExpression(filePtr, node->right, latex);
        putc(')', filePtr);
    } else printExpression(filePtr, node->right, latex);
}




bool isZero(Node* node) {
    if (node == NULL) return false;
    return node->type == CONST_TYPE && node->data.constValue == 0;
}

bool isOne(Node* node) {
    if (node == NULL) return false;
    return node->type == CONST_TYPE && node->data.constValue == 1;
}

bool isNumber(Node* node){
    if (node == NULL) return false;
    return node->type == CONST_TYPE;
}

#define value(node) node->data.constValue

Node* simplifyExpression(Node* node){
    if (node == NULL) return NULL;

    if (node->type == CONST_TYPE || node->type == VARIABLE_TYPE) return createNodeCopy(node);

    // TODO calculate actual value if function argument is const
    if (node->type == UNARY_FUNCTION) return createNodeCopy(node);


    Node* result = NULL;
    Node* left = simplifyExpression(node->left);
    Node* right = simplifyExpression(node->right);
    bool positive = false;
    switch (node->data.operatorType) {
    case ADD:
        positive = true;
    case SUB:
        if (isNumber(left) && isNumber(right)) {
            if (positive) result = createConstNode(value(left) + value(right));
            else result = createConstNode(value(left) - value(right));
            destroyNode(left);
            destroyNode(right);
        }
        else if (isZero(left)) {
            if (positive) result = right;
            else {
                result = createOperatorNode(MUL);
                result->left = createConstNode(-1);
                result->right = right;
            }
            destroyNode(left);
        }
        else if (isZero(right)) {
            result = left;
            destroyNode(right);
        }
        break;
    case MUL:
        if (isNumber(left) && isNumber(right)) {
            result = createConstNode(left->data.constValue * right->data.constValue);
            destroyNode(left);
            destroyNode(right);
        } else if (isZero(left)){
            result = left;
            destroyNode(right);
        } else if (isZero(right)){
            result = right;
            destroyNode(left);
        } else if (isOne(left)){
            result = right;
            destroyNode(left);
        } else if (isOne(right)){
            result = left;
            destroyNode(right);
        }
        break;
    case DIV:
        if (isZero(right)) {
            printErr("Erm... division by zero\n");
            break;
        }
        if (isNumber(left) && isNumber(right)) {
            result = createConstNode(left->data.constValue / right->data.constValue);
            destroyNode(left);
            destroyNode(right);
        } else if (isZero(left)){
            result = left;
            destroyNode(right);
        } else if (isOne(right)){
            result = left;
            destroyNode(right);
        }
        break;
    case POW:
        if (isZero(left) || isOne(left) || isOne(right)){
            destroyNode(right);
            result = left;  
        } else if (isZero(right)){
            destroyNode(left);
            destroyNode(right);
            result = createConstNode(1);
        }
        break;
    default:
        printErr("That operator is pretty bad yk?\n");
        break;
    }
    if (result == NULL) {
        result = createOperatorNode(node->data.operatorType);
        result->left = left;
        result->right = right;
    }
    return result;
}