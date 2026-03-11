#include "AST/enumdeclaration.h"
#include "parser/parser_data.h"

EnumDeclaration::EnumDeclaration(std::string name, std::vector<EnumVariant> variants)
    : name(std::move(name)), variants(std::move(variants)) {}

std::ostream& EnumDeclaration::print(std::ostream& os) const {
    os << "Enum " << name << " {";
    for (size_t i = 0; i < variants.size(); ++i) {
        os << variants[i].name;
        if (variants[i].payload_type) {
            os << "(" << *variants[i].payload_type << ")";
        }
        if (i < variants.size() - 1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}

void EnumDeclaration::codegen(ParserData& data) const {
    // Phase 1: register the enum in ParserData for semantic analysis
    data.register_enum(name, variants);
}