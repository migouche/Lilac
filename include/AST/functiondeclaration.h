//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_FUNCTIONDECLARATION_H
#define LILAC_FUNCTIONDECLARATION_H

#include <vector>
#include <memory>

#include "astdefinition.h"
#include "lexer/token.h"
#include "astvalue.h"
#include "parser/parser_data.h"

struct FunctionCase
{
    std::vector<Token> inputs;
    //&std::vector<Token> outputs; // TODO: must be an function call, for once functional programming saves us
    std::unique_ptr<ASTValue> output;

    [[nodiscard]] llvm::Value* codegen(ParserData&) const;

    void print() const;
};

class FunctionDeclaration
{
private:
    std::string name;
    std::vector<Token> domain;
    std::vector<Token> codomain;
    std::vector<FunctionCase> cases;
    bool pure;
public:
    FunctionDeclaration(std::string , std::vector<Token>, std::vector<Token>, std::vector<FunctionCase>, bool);
    void print() const;
    [[nodiscard]] llvm::Function* codegen(ParserData&) const;
    [[nodiscard]] llvm::Function* prototype_codegen(ParserData&) const;

};


#endif //LILAC_FUNCTIONDECLARATION_H
