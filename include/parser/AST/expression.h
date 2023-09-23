//
// Created by migouche on 21/09/23.
//

#ifndef LILAC_EXPRESSION_H
#define LILAC_EXPRESSION_H

#include "parser/AST/astvalue.h"
#include "lexer/token.h"

class Expression: public ASTValue
{
private:
    Token token;
public:
    explicit Expression(Token token);

    void print() const override;

};

#endif //LILAC_EXPRESSION_H
