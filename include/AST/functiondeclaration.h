//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_FUNCTIONDECLARATION_H
#define LILAC_FUNCTIONDECLARATION_H

#include <list>
#include "lexer/token.h"
#include "astvalue.h"

struct FunctionCase
{
    std::list<Token> inputs;
    //&std::list<Token> outputs; // TODO: must be an function call, for once functional programming saves us
    std::shared_ptr<ASTValue> output;

    void print() const;
};

class FunctionDeclaration
{
private:
    std::string name;
    std::list<Token> domain;
    std::list<Token> codomain;
    std::list<FunctionCase> cases;
    bool pure;
public:
    FunctionDeclaration(std::string , std::list<Token>, std::list<Token>, std::list<FunctionCase>, bool);
    void print() const;
    llvm::Function* codegen();

};


#endif //LILAC_FUNCTIONDECLARATION_H
