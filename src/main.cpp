#include "parser/parser.h"
#include <casm/casm.h>
#include <iostream>

int main() {
    Parser parser("data/function.llc");

    auto tree = parser.get_tree();

    tree.print();

    compile("data/hello_amd64.asm", "data/hello_amd64.bin");

    std::cout << "out" << std::endl;

    /*Tokenizer tok("data/function.llc");

    while(!tok.end_of_tokens()) {
        std::cout << tok.get_token() << " ";
    }*/

    return 0;

}
