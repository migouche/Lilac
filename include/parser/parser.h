//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_PARSER_H
#define LILAC_PARSER_H

#include <memory>

#include "lexer/tokenizer.h"
#include "AST/asttree.h"
#include "AST/ASTValues/functioncall.h"


class Parser
{
private:
    std::unique_ptr<Tokenizer> tokenizer;
    [[nodiscard]] std::shared_ptr<FunctionDeclaration> parse_function() ;
    std::shared_ptr<FunctionCall> parse_call;
    std::shared_ptr<ASTDefinition> parse_definition() ;
    std::vector<std::vector<std::string>> token_stack;

public:

    explicit Parser(const std::string&);
    [[nodiscard]] ASTTree get_tree() ;
};

#endif //LILAC_PARSER_H
