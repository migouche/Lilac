//
// Created by migouche on 6/7/25.
//

#include "../../include/AST/astdefinition.h"

#include <iostream>
#include <utility>
#include <llvm/IR/Module.h>

ASTDefinition::ASTDefinition(std::string  name, const std::shared_ptr<ASTValue>& value, const bool global) : global(global), name(std::move(name)), value(value) {}

void ASTDefinition::print() const
{
    std::cout << "Definition: " << name << " = ";
    value->print();
}

llvm::Value* ASTDefinition::codegen(const std::unique_ptr<ParserData>& parser_data) const
{

    if (global)
    {
        // 1. Generate the initializer value
        llvm::Value* initVal = value->codegen(parser_data);
        if (!initVal) return nullptr;

        // 2. It must be a Constant to use as a global initializer
        auto* constInit = llvm::dyn_cast<llvm::Constant>(initVal);
        if (!constInit) {
            std::cerr << "Global initializer '" << name << "' is not a constant\n";
            return nullptr;
        }

        // 3. Create the GlobalVariable
        llvm::GlobalVariable* gv = new llvm::GlobalVariable(
            *parser_data->module,               // parent module
            initVal->getType(),                 // value type
            /*isConstant=*/false,               // mutable
            llvm::GlobalValue::ExternalLinkage,
            constInit,                          // initializer
            name                                 // global name
        );

        // 4. Register in symbol table and return
        parser_data->add_value(name, gv, initVal->getType(), true);
        return gv;
    }
    else
    {
        // No clue if this works yet
        llvm::IRBuilder<>& builder = *parser_data->builder;

        // 1. Compute the RHS value
        llvm::Value* initVal = value->codegen(parser_data);
        if (!initVal) return nullptr;

        // 2. Create an alloca in the entry block for the variable
        llvm::Function* function = builder.GetInsertBlock()->getParent();
        llvm::IRBuilder<> entryBuilder(&function->getEntryBlock(),
                                       function->getEntryBlock().begin());
        llvm::AllocaInst* alloca = entryBuilder.CreateAlloca(initVal->getType(), nullptr, name);

        // 3. Store the initial value into the alloca
        builder.CreateStore(initVal, alloca);

        // 4. Register the variable in the symbol table
        parser_data->add_value(name, alloca, initVal->getType(), false);

        // 5. Return the alloca (or you could return initVal)
        return alloca;
    }
}
