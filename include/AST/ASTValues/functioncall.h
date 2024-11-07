//
// Created by migouche on 20/09/23.
//

#ifndef LILAC_FUNCTIONCALL_H
#define LILAC_FUNCTIONCALL_H

#include <list>
#include "AST/astvalue.h"

class FunctionCall: public ASTValue
{
private:
    std::string name;
    std::list<std::shared_ptr<ASTValue>> arguments;
public:
    FunctionCall(std::string name, std::list<std::shared_ptr<ASTValue>> arguments);

    void print() const override;
    llvm::Value * codegen(const std::unique_ptr<ParserData>&) override;
};

#endif //LILAC_FUNCTIONCALL_H
