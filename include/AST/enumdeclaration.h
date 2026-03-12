#ifndef LILAC_ENUMDECLARATION_H
#define LILAC_ENUMDECLARATION_H

#include <string>
#include <vector>
#include <optional>
#include <ostream>

struct EnumVariant {
    std::string name;
    std::optional<std::string> payload_type;
};

class ParserData;

class EnumDeclaration {
public:
    EnumDeclaration(std::string name, std::vector<EnumVariant> variants);
    std::ostream& print(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const EnumDeclaration& decl) {
        return decl.print(os);
    }
    void codegen(ParserData& data) const;
private:
    std::string name;
    std::vector<EnumVariant> variants;
};

#endif //LILAC_ENUMDECLARATION_H