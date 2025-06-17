//
// Created by migouche on 9/13/2023.
//

#include "AST/functiondeclaration.h"
#include "parser/parser_data.h"

#include <utility>
#include <iostream>
#include <llvm/IR/Module.h>


FunctionDeclaration::FunctionDeclaration(std::string name,
                                         std::list<Token> domain, std::list<Token> codomain,
                                         std::list<FunctionCase> cases, bool pure):
                                         name(std::move(name)), domain(std::move(domain)),
                                         codomain(std::move(codomain)), cases(std::move(cases)),
                                         pure(pure){}

void FunctionDeclaration::print() const {
    std::cout << "name: " << name << std::endl;
    std::cout << "purity: " << (pure ? "pure": "impure") << std::endl;

    std::cout << "domain: ";
    for(auto& token: domain) {
        std::cout << token.get_value();
        std::cout << ", ";
    }

    std::cout << "\ncodomain: ";
    for(const auto& token: codomain) {
        std::cout << token.get_value();
        std::cout << ", ";
    }
    std::cout << "\ncases: ";
    for(const auto& f_case: cases)
        f_case.print();
}

llvm::Function *FunctionDeclaration::prototype_codegen(const std::unique_ptr<ParserData>& parser_data) const{
    /*std::vector<llvm::Type*> dom(domain.size(), llvm::Type::getInt32Ty(*parser_data->context));
    llvm::FunctionType* ft =
            llvm::FunctionType::get(llvm::Type::getInt32Ty(
                    *parser_data->context), dom, false);
    llvm::Function* f =
            llvm::Function::Create(ft,
                                   llvm::Function::ExternalLinkage,
                                   name, *parser_data->module);

    unsigned char i = 'a';
    for(auto& arg: f->args())
        arg.setName(std::string(1, i++)); // dont have names?? for now so im using letters of the alphabet
    return f;*/

    std::vector<llvm::Type*> dom;
    for(const auto& token: domain) {
        if(token.get_value() != "void")
            dom.push_back(parser_data->get_primitive(token.get_value()));
    }
    llvm::Type* returnType = parser_data->get_primitive(codomain.front().get_value());

    llvm::FunctionType* ft = llvm::FunctionType::get(returnType, dom, false);
    llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, *parser_data->module);

    char i = 'a';
    for(auto& arg: f->args())
        arg.setName(std::string(1, i++));
    return f;
}

llvm::Function* FunctionDeclaration::codegen(const std::unique_ptr<ParserData>& parser_data) const {
    // get or create function prototype
    llvm::Function* f = parser_data->module->getFunction(name);
    if (!f) f = prototype_codegen(parser_data);
    if (!f || !f->empty()) return nullptr;

    // entry + builder setup
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*parser_data->context, "entry", f);
    parser_data->builder->SetInsertPoint(entry);

    // collect args
    std::vector<llvm::Value*> args;
    for (auto& a : f->args()) args.push_back(&a);

    // prepare blocks: one match block per case + a failure block
    std::vector<llvm::BasicBlock*> matchBlocks;
    for (size_t i = 0; i < cases.size(); ++i)
        matchBlocks.push_back(llvm::BasicBlock::Create(*parser_data->context, "case" + std::to_string(i), f));
    llvm::BasicBlock* failBlock = llvm::BasicBlock::Create(*parser_data->context, "no_match", f);

    // jump from entry to first case
    parser_data->builder->CreateBr(matchBlocks[0]);

    // emit each case
    size_t i = 0;
    // emit each case
    for (const auto& [inputs, output] : cases) {
        // set insertion point to the current match block
        parser_data->builder->SetInsertPoint(matchBlocks[i]);

        // build runtime test (cond) over arguments
        llvm::Value* cond = nullptr;
        auto argIt = args.begin();
        for (const auto& tok : inputs) {
            if (tok.get_token_kind() == UNDERSCORE) {
                // wildcard, skip
            }
            else if (tok.get_token_kind() == LITERAL) {
                const int lit = std::stoi(tok.get_value());
                const auto cmp = parser_data->builder->CreateICmpEQ(
                    *argIt,
                    llvm::ConstantInt::get(parser_data->get_primitive("int"), lit)
                );
                cond = cond ? parser_data->builder->CreateAnd(cond, cmp) : cmp;
            }
            ++argIt;
        }
        if (!cond)
            cond = llvm::ConstantInt::getTrue(*parser_data->context);

        // prepare body and next-case blocks
        llvm::BasicBlock* body = llvm::BasicBlock::Create(*parser_data->context, "body" + std::to_string(i), f);
        llvm::BasicBlock* next = (i + 1 < cases.size()) ? matchBlocks[i + 1] : failBlock;
        parser_data->builder->CreateCondBr(cond, body, next);

        // emit case body
        parser_data->builder->SetInsertPoint(body);
        parser_data->enter_scope();
        argIt = args.begin();
        for (const auto& tok : inputs) {
            if (tok.get_token_kind() == IDENTIFIER) {
                parser_data->add_value(tok.get_value(), *argIt, parser_data->get_primitive(tok.get_value()), false);
            }
            ++argIt;
        }
        if (llvm::Value* retV = output->codegen(parser_data)) {
            parser_data->builder->CreateRet(retV);
        }
        parser_data->exit_scope();

        ++i;
    }
    // no pattern matched: emit unreachable (or errorraise)
    parser_data->builder->SetInsertPoint(failBlock);
    parser_data->builder->CreateUnreachable();

    return f;
}

void FunctionCase::print() const {
    std::cout << "function case: \n\tinputs: \n\t";
    for (const auto& input: inputs)
    {
        input.print();
        std::cout << ", ";
    }

    std::cout << "\n\toutput: ";

    output->print();
    std::cout << std::endl;
}

llvm::Value *FunctionCase::codegen(const std::unique_ptr<ParserData>& parser_data) const {
    return output->codegen(parser_data);
}
