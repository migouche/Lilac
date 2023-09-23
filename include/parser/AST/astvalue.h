//
// Created by migouche on 21/09/23.
//

#ifndef LILAC_ASTVALUE_H
#define LILAC_ASTVALUE_H

#include "parser/AST/astnode.h"

class ASTValue: public ASTNode
{
public:
    void print() const override = 0;
};

#endif //LILAC_ASTVALUE_H
