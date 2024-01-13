#include "compiler/compiler.h"
#include "lexer/tokenizer.h"
#include <iostream>

#include "debugmode.h" // remove this line to disable "debug mode"


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


#ifdef DEBUG_MODE

    std::cout << "out" << std::endl;

    Tokenizer tok("data/function.llc");

    while(!tok.end_of_tokens()) {
        std::cout << tok.get_token() << " ";
    }

#endif

    // testing a simple sum using LLVM

    return 0;

}
