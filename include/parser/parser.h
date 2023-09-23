//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_PARSER_H
#define LILAC_PARSER_H

#include <memory>
#include "lexer/tokenizer.h"
#include "parser/AST/asttree.h"
#include "parser/AST/astnode.h"
#include "parser/AST/functioncall.h"

class Parser
{
private:
    std::unique_ptr<Tokenizer> tokenizer;
    std::shared_ptr<FunctionDeclaration> parse_function();
    std::shared_ptr<FunctionCall> parse_call;
public:
    explicit Parser(const std::string&);
    ASTTree get_tree();
};

#endif //LILAC_PARSER_H
