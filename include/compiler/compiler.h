//
// Created by migouche on 11/14/2023.
//

#ifndef LILAC_COMPILER_H
#define LILAC_COMPILER_H

#include <string>
#include "AST/asttree.h"
#include "parser/parser_data.h"
#include <vector>

class Compiler
{
private:
    void compile_file(const std::string& file,  const std::unique_ptr<ParserData>&);
    std::vector<ASTTree> trees;
    std::unique_ptr<ParserData> data;

public:
    explicit Compiler(const std::vector<std::string>&);

    ASTTree& get_tree(size_t i = 0);

    static std::string status();
    [[nodiscard]] const std::unique_ptr<ParserData> & get_parser_data() const;

    static llvm::Value* log_error_v(std::string);

};

#endif //LILAC_COMPILER_H
