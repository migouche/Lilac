//
// Created by migouche on 04-Nov-24.
//

#ifndef LILAC_VARIABLE_H
#define LILAC_VARIABLE_H

// will treat arguments as variables

#include "AST/astvalue.h"
#include "lexer/token.h"

class Variable: public ASTValue{
public:
    explicit Variable(const Token&);
    llvm::Value* codegen(const std::unique_ptr<ParserData>&) override;
    void print() const override;
private:
    std::string name;
};

#endif //LILAC_VARIABLE_H
