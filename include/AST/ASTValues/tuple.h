//
// Created by migouche on 9/24/2023.
//

#ifndef LILAC_TUPLE_H
#define LILAC_TUPLE_H

#include <list>
#include "AST/astvalue.h"

class Tuple final : public ASTValue
{
public:
    explicit Tuple(std::list<std::shared_ptr<ASTValue>> elements); //

    void print() const override;
    llvm::Value * codegen(const std::unique_ptr<ParserData>&) override;
private:
    std::list<std::shared_ptr<ASTValue>> elements;
};

#endif //LILAC_TUPLE_H
