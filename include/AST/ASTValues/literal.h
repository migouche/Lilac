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
    Token token;
public:
    explicit Literal(Token token);

    void print() const override;
    llvm::Value * codegen(const std::shared_ptr<ParserData>&) override;

};

#endif //LILAC_LITERAL_H
