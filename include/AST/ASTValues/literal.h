//
// Created by migouche on 21/09/23.
//

#ifndef LILAC_LITERAL_H
#define LILAC_LITERAL_H

#include "AST/astvalue.h"
#include "lexer/token.h"

class Literal final : public ASTValue
{
private:
    std::string value;
public:
    explicit Literal(const Token& token);

    std::ostream& print(std::ostream& os) const override;
    llvm::Value * codegen(ParserData&) override;

};

#endif //LILAC_LITERAL_H
