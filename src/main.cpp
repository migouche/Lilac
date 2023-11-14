#include "compiler/compiler.h"
#include "lexer/tokenizer.h"
#include <casm/casm.h>
#include <iostream>

int main() {

    //Parser parser("data/function.llc");

    try {
        Compiler c("data/function.llc");

        auto tree = c.get_tree();

        tree.print();
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "Compilation failed:\n" << e.what() << std::endl; // TODO: better errors xd
    }
    compile((char *) "data/hello_amd64.asm", (char *) "data/hello_amd64.bin");

    std::cout << "out" << std::endl;

    Tokenizer tok("data/function.llc");

    while(!tok.end_of_tokens()) {
        std::cout << tok.get_token() << " ";
    }

    return 0;

}
