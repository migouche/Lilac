#include "compiler/compiler.h"
#include "lexer/tokenizer.h"
#include <iostream>

#include "debugmode.h" // remove this line to disable "debug mode"
#include "parser/parser_data.h"

#include <CLI/CLI11.hpp>

#include "parser/parser.h"

#ifdef DEBUG_MODE
#include <llvm/IR/Module.h>
#endif



int main(const int argc, char** argv) {

    CLI::App app{"Lilac Compiler"};

    std::vector<std::string> input_files;
    std::string output_file = "a.out";
    int O = 0;
    bool debug = false;
    bool parse = false;

    app.add_option("input", input_files, "Input files")->required()->expected(1, -1);
    app.add_option("-o,--output", output_file, "Output executable");
    app.add_option("-O", O, "Optimization level")->check(CLI::Range(0, 3));
    app.add_flag("-g,--debug", debug, "Debug mode");
    app.add_flag("-p,--parse", parse, "Parse only, do not compile");

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
                << "\nParse only: " << (parse ? "enabled" : "disabled")
                << std::endl;

#endif

    //Parser parser("data/function.llc");
    try
    {
        if (parse) {
            for (const auto& file : input_files) {
                Parser parser(file);
                auto tree = parser.get_tree();
                tree.print(std::cout);
            }
        } else {
            Compiler compiler(input_files, output_file, O, debug);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;

}
