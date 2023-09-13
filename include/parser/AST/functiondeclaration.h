//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_FUNCTIONDECLARATION_H
#define LILAC_FUNCTIONDECLARATION_H

#include <list>
#include "parser/AST/astnode.h"
#include "lexer/token.h"

struct FunctionCase
{
    std::list<Token> inputs;
    std::list<Token> outputs; // TODO: must be an function call, for once functional programming saves us

    void print() const;
};

class FunctionDeclaration: public ASTNode
{
private:
    std::string name;
    std::list<Token> domain;
    std::list<Token> codomain;
    std::list<FunctionCase> cases;
public:
    FunctionDeclaration(std::string , std::list<Token>, std::list<Token>, std::list<FunctionCase>);
    void print() const;

};


#endif //LILAC_FUNCTIONDECLARATION_H
