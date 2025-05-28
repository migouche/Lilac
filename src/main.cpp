#include "compiler/compiler.h"
#include "lexer/tokenizer.h"
#include <iostream>

#include "debugmode.h" // remove this line to disable "debug mode"
#include "parser/parser_data.h"

#ifdef DEBUG_MODE
#include <llvm/IR/Module.h>
#endif



int main() {

    //Parser parser("data/function.llc");
    try {
        std::vector<std::string> files = {"data/function.llc"};
        const auto c = std::make_unique<Compiler>(files);

        const auto tree = c->get_tree();

        tree.print();

#ifdef DEBUG_MODE
        c->get_parser_data()->module->print(llvm::errs(), nullptr);
        c->get_parser_data()->module->print(llvm::outs(), nullptr);
#endif
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


    // DEBUG CODE:



    //parser_data::module->print(llvm::errs(), nullptr);
#endif

    // testing a simple sum using LLVM

    return 0;

}
