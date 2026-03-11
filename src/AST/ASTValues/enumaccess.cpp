//
// Created by Lilac
//

#include "AST/ASTValues/enumaccess.h"
#include "parser/parser_data.h"
#include <iostream>

EnumAccess::EnumAccess(std::string variant_name, std::vector<std::unique_ptr<ASTValue>> payload)
    : variant_name(std::move(variant_name)), payload(std::move(payload)) {}

llvm::Value* EnumAccess::codegen(ParserData& data) {
    // For now we just return null or integer tag
    // Since real payload parsing for enums requires more sophisticated LLVM type generation,
    // let's do a basic stub that looks up the enum variant in parser data and returns an integer.
    
    std::string enum_name = data.get_enum_for_variant(variant_name);
    if (enum_name.empty()) {
        std::cerr << "Unknown enum variant: " << variant_name << std::endl;
        return nullptr;
    }

    auto variants = data.get_enum(enum_name);
    int tag_index = -1;
    for (size_t i = 0; i < variants->size(); ++i) {
        if ((*variants)[i].name == variant_name) {
            tag_index = i;
            break;
        }
    }

    if (tag_index == -1) {
        std::cerr << "Variant " << variant_name << " not found in enum " << enum_name << std::endl;
        return nullptr;
    }

    llvm::Type* enum_ty = data.get_primitive(enum_name);
    llvm::Value* res = llvm::UndefValue::get(enum_ty);
    res = data.builder->CreateInsertValue(res, llvm::ConstantInt::get(data.get_primitive("int"), tag_index), 0);

    if (!payload.empty()) {
        llvm::Value* payload_val = payload[0]->codegen(data);
        if (!payload_val) {
            std::cerr << "Enum payload codegen failed" << std::endl;
            return nullptr;
        }
        res = data.builder->CreateInsertValue(res, payload_val, 1);
    } else {
        res = data.builder->CreateInsertValue(res, llvm::ConstantInt::get(data.get_primitive("int"), 0), 1);
    }

    return res;
}

std::ostream& EnumAccess::print(std::ostream& os) const {
    os << "." << variant_name;
    if (!payload.empty()) {
        os << "(";
        for (size_t i = 0; i < payload.size(); ++i) {
            payload[i]->print(os);
            if (i < payload.size() - 1) os << ", ";
        }
        os << ")";
    }
    return os;
}