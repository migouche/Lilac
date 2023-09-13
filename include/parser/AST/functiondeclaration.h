//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_FUNCTIONDECLARATION_H
#define LILAC_FUNCTIONDECLARATION_H

#include "parser/AST/astnode.h"

class FunctionDeclaration: public ASTNode
{
private:
    std::string name;
    std::list<Token> domain;
    std::list<Token> codomain;
    std::list

};

#endif //LILAC_FUNCTIONDECLARATION_H
