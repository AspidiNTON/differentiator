#include "expression.h"

int main(){
    ReaderData rd;
    rd.buffer = "1+2$";
    rd.index = 0;
    getExpression(&rd);
    return 0;
}