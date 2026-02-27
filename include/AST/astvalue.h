//
// Created by migouche on 21/09/23.
//

#ifndef LILAC_ASTVALUE_H
#define LILAC_ASTVALUE_H

#include <llvm/IR/Value.h>
#include <iostream>
#include "parser/parser_data.h"


class ASTValue
{
public:
    virtual ~ASTValue() = default;

    virtual std::ostream& print(std::ostream& os) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const ASTValue& node) {
        return node.print(os);
    }

    virtual llvm::Value* codegen(ParserData&) = 0;
};

#endif //LILAC_ASTVALUE_H
