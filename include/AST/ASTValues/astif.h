//
// Created by migouche on 31-May-25.
//

#ifndef LILAC_ASTIF_H
#define LILAC_ASTIF_H
#include "AST/astvalue.h"


class ASTIf : public ASTValue{
public:
    ASTIf(std::unique_ptr<ASTValue> condition, std::unique_ptr<ASTValue> then_branch,
          std::unique_ptr<ASTValue> else_branch);

    [[nodiscard]] ASTValue* get_condition() const { return condition.get(); }
    [[nodiscard]] ASTValue* get_then_branch() const { return then_branch.get(); }
    [[nodiscard]] ASTValue* get_else_branch() const { return else_branch.get(); }

    llvm::Value *codegen(ParserData &parser_data) override;
    std::ostream& print(std::ostream& os) const override;

private:
    std::unique_ptr<ASTValue> condition;
    std::unique_ptr<ASTValue> then_branch;
    std::unique_ptr<ASTValue> else_branch;
};
#endif //LILAC_ASTIF_H
