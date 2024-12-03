#include "expression.h"


int main(){
    ReaderData rd;
    rd.buffer = "25*x/x^2$";
    rd.index = 0;

    Node* expression = getExpression(&rd);
    Node* diff = getDerivative(expression, 'x');

    printExpression(stdin, expression);
    putchar('\n');
    printExpression(stdin, diff);

    destroyNode(expression);
    destroyNode(diff);
    return 0;
}