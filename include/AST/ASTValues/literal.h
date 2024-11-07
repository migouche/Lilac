//
// Created by migouche on 21/09/23.
//

#ifndef LILAC_LITERAL_H
#define LILAC_LITERAL_H

#include "AST/astvalue.h"
#include "lexer/token.h"

class Literal: public ASTValue
{
private:
    std::string value;
public:
    explicit Literal(const Token& token);

    void print() const override;
    llvm::Value * codegen(const std::unique_ptr<ParserData>&) override;

};

#endif //LILAC_LITERAL_H
