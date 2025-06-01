#include "compiler/compiler.h"
#include "lexer/tokenizer.h"
#include <iostream>

#include "debugmode.h" // remove this line to disable "debug mode"
#include "parser/parser_data.h"

#include <CLI/CLI11.hpp>

#ifdef DEBUG_MODE
#include <llvm/IR/Module.h>
#endif



int main(const int argc, char** argv) {

    CLI::App app{"Lilac Compiler"};

    std::vector<std::string> input_files;
    std::string output_file = "a.out";
    int O = 0;
    bool debug = false;

    app.add_option("input", input_files, "Input files")->required()->expected(1, -1);
    app.add_option("-o,--output", output_file, "Output executable");
    app.add_option("-O", O, "Optimization level")->check(CLI::Range(0, 3));
    app.add_flag("-g,--debug", debug, "Debug mode");

    CLI11_PARSE(app, argc, argv);
#ifdef DEBUG_MODE

    std::cout << "Lilac Compiler\n"
              << "Input files: ";

    for (const auto& file : input_files) {
        std::cout << file << " ";
    }
    std::cout << "\nOutput file: " << output_file
              << "\nOptimization level: O" << O
              << "\nDebug mode: " << (debug ? "enabled" : "disabled")
              << std::endl;

#endif

    //Parser parser("data/function.llc");
    try {
        const auto c = std::make_unique<Compiler>(input_files, output_file, O, debug);

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
