//
// Created by migouche on 20/09/23.
//

#ifndef LILAC_FUNCTIONCALL_H
#define LILAC_FUNCTIONCALL_H

#include "parser/AST/astvalue.h"

class FunctionCall: public ASTValue
{
private:
    std::string name;
public:
    FunctionCall(std::string name, std::list<std::shared_ptr<ASTNode>> arguments);

    void print() const override;
};

#endif //LILAC_FUNCTIONCALL_H
