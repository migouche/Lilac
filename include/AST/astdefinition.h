//
// Created by migouche on 6/7/25.
//

#ifndef ASTDEFINITION_H
#define ASTDEFINITION_H
#include "ASTValues/variable.h"

class ASTDefinition
{
public:
    ASTDefinition(std::string , std::unique_ptr<ASTValue>, bool global);
    std::ostream& print(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const ASTDefinition& def) {
        return def.print(os);
    }
    [[nodiscard]] llvm::Value* codegen(ParserData&) const;
private:
    bool global;
    std::string name;
    std::unique_ptr<ASTValue> value;
};


#endif //ASTDEFINITION_H
