//
// Created by migouche on 11/14/2023.
//
#include <iostream>
#include "compiler/compiler.h"
#include "parser/parser.h"
#include "parser/parser_data.h"

void Compiler::compile_file(const std::string& file,  const std::shared_ptr<ParserData>& parser_data) {
    Parser parser(file);
    trees.push_back(parser.get_tree());

}

ASTTree &Compiler::get_tree(size_t i) {
    return trees.at(i);
}

std::string Compiler::status() {
    return "hey ho";
}

Compiler::Compiler(const std::vector <std::string>& files) {
    //std::shared_ptr parser_data = std::make_shared<parser_data::ParserData>();
    data = std::make_shared<ParserData>();

    for(const auto& f: files)
        compile_file(f, data);

    for(const auto& t: trees)
    {
        t.codegen(data);
    }
}

std::shared_ptr<ParserData> Compiler::get_parser_data() {
    return data;
}


