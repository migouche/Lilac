//
// Created by migouche on 04-Nov-24.
//

#ifndef LILAC_VARIABLE_H
#define LILAC_VARIABLE_H

// will treat arguments as variables

#include "AST/astvalue.h"
#include "lexer/token.h"

class Variable final : public ASTValue{
public:
    explicit Variable(const Token&);
    llvm::Value* codegen(ParserData&) override;
    void print() const override;
private:
    std::string name;
};

#endif //LILAC_VARIABLE_H
