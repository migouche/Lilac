//
// Created by Lilac
//

#ifndef LILAC_ENUMACCESS_H
#define LILAC_ENUMACCESS_H

#include "AST/astvalue.h"
#include <string>
#include <memory>
#include <vector>

class EnumAccess : public ASTValue {
private:
    std::string variant_name;
    std::vector<std::unique_ptr<ASTValue>> payload;
public:
    EnumAccess(std::string variant_name, std::vector<std::unique_ptr<ASTValue>> payload);
    llvm::Value* codegen(ParserData& data) override;
    std::ostream& print(std::ostream& os) const override;
    const std::vector<std::unique_ptr<ASTValue>>& get_payload() const { return payload; }
    const std::string& get_variant_name() const { return variant_name; }
};

#endif //LILAC_ENUMACCESS_H
