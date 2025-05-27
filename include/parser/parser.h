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
    [[nodiscard]] std::shared_ptr<FunctionDeclaration> parse_function() const;
    std::shared_ptr<FunctionCall> parse_call;



public:

    explicit Parser(const std::string&);
    [[nodiscard]] ASTTree get_tree() const;
};

#endif //LILAC_PARSER_H
