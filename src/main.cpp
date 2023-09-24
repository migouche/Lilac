#include "parser/parser.h"
#include <iostream>

int main() {
    Parser parser("data/function.llc");

    auto tree = parser.get_tree();

    tree.print();

    /*Tokenizer tok("data/function.llc");

    while(!tok.end_of_tokens()) {
        std::cout << tok.get_token() << " ";
    }*/

    return 0;

}
