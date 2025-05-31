//
// Created by migouche on 31-May-25.
//

#ifndef LILAC_ASTIF_H
#define LILAC_ASTIF_H
#include "AST/astvalue.h"


class ASTIf : public ASTValue{
public:
    ASTIf(std::shared_ptr<ASTValue> condition, std::shared_ptr<ASTValue> then_branch,
          std::shared_ptr<ASTValue> else_branch);

    [[nodiscard]] std::shared_ptr<ASTValue> get_condition() const { return condition; }
    [[nodiscard]] std::shared_ptr<ASTValue> get_then_branch() const { return then_branch; }
    [[nodiscard]] std::shared_ptr<ASTValue> get_else_branch() const { return else_branch; }

    llvm::Value *codegen(const std::unique_ptr<ParserData> &parser_data) override;
    void print() const override;

private:
    std::shared_ptr<ASTValue> condition;
    std::shared_ptr<ASTValue> then_branch;
    std::shared_ptr<ASTValue> else_branch;
};



#endif //LILAC_ASTIF_H
