//
// Created by migouche on 11/14/2023.
//
#include "compiler/compiler.h"
#include "parser/parser.h"

void Compiler::compile_file(const char* file) {
    Parser parser(file);
    trees.push_back(parser.get_tree());
}

ASTTree &Compiler::get_tree(size_t i) {
    return trees.at(i);
}

std::string Compiler::status() {
    return "hey ho";
}
