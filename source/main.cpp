#include "expression.h"
#include "dump.h"



void computeExpression(const char* filename){
    Node* expression = readExpressionFromFile(filename);
    Node* diff = getDerivative(expression, 'x');
    Node* simplified = simplifyExpression(diff);

    printInLogger("Initial expression:\n");
    dumpTree(expression);
    printInLogger("After differentiation:\n");
    dumpTree(diff);
    printInLogger("After simplification:\n");
    dumpTree(simplified);
    printInLogger("\n\n\n");

    printExpression(stdout, simplified, true);
    putchar('\n');

    destroyNode(expression);
    destroyNode(diff);
    destroyNode(simplified);
}

int main(){
    initializeLogger();

    computeExpression("1.txt");
    computeExpression("xdd.txt");

    
    closeLogger();
    return 0;
}