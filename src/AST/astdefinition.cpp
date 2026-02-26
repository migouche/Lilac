//
// Created by migouche on 6/7/25.
//

#include "../../include/AST/astdefinition.h"

#include <iostream>
#include <utility>
#include <llvm/IR/Module.h>

ASTDefinition::ASTDefinition(std::string  name, std::unique_ptr<ASTValue> value, const bool global) : global(global), name(std::move(name)), value(std::move(value)) {}

void ASTDefinition::print() const
{
    std::cout << "Definition: " << name << " = ";
    value->print();
}

llvm::Value* ASTDefinition::codegen(ParserData& parser_data) const
{
    auto& builder = *parser_data.builder;
    llvm::Module* module = parser_data.module.get();

    // Generate the initializer
    llvm::Value* initVal = value->codegen(parser_data);
    if (!initVal) return nullptr;

    if (global) {
        // Must be a constant for a global
        auto* constInit = llvm::dyn_cast<llvm::Constant>(initVal);
        if (!constInit) {
            llvm::errs() << "Global initializer '" << name << "' is not a constant\n";
            return nullptr;
        }

        // Create or update the global variable
        llvm::GlobalVariable* gv = module->getGlobalVariable(name);
        if (!gv) {
            gv = new llvm::GlobalVariable(
                *module,
                initVal->getType(),
                /*isConstant=*/false,
                llvm::GlobalValue::ExternalLinkage,
                constInit,
                name
            );
        } else {
            gv->setInitializer(constInit);
        }

        parser_data.add_value(name, gv, initVal->getType(), /*global=*/true);
        return gv;
    } else {
        // Local alloca in entry block
        llvm::Function* fn = builder.GetInsertBlock()->getParent();
        llvm::IRBuilder<> tmpB(&fn->getEntryBlock(), fn->getEntryBlock().begin());
        llvm::AllocaInst* alloca = tmpB.CreateAlloca(initVal->getType(), /*arraySize=*/nullptr, name);
        builder.CreateStore(initVal, alloca);
        parser_data.add_value(name, alloca, initVal->getType(), /*global=*/false);
        return alloca;
    }
}
