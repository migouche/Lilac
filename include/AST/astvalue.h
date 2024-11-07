//
// Created by migouche on 21/09/23.
//

#ifndef LILAC_ASTVALUE_H
#define LILAC_ASTVALUE_H

#include <llvm/IR/Value.h>
#include "parser/parser_data.h"


class ASTValue
{
public:
    virtual void print() const = 0;
    virtual llvm::Value* codegen(const std::unique_ptr<ParserData>&) = 0;
};

#endif //LILAC_ASTVALUE_H
