//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_PARSER_H
#define LILAC_PARSER_H

#include <memory>
#include "lexer/tokenizer.h"
#include "parser/AST/asttree.h"

class Parser
{
private:
    std::unique_ptr<Tokenizer> tokenizer;
public:
    explicit Parser(const std::string&);
    ASTTree parse();
};

#endif //LILAC_PARSER_H
