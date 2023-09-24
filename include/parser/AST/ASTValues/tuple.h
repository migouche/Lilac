//
// Created by migouche on 9/24/2023.
//

#ifndef LILAC_TUPLE_H
#define LILAC_TUPLE_H

#include "parser/AST/astvalue.h"

class Tuple: public ASTValue
{
public:
    Tuple(std::list<std::shared_ptr<ASTNode>> elements);

    void print() const override;
};

#endif //LILAC_TUPLE_H
