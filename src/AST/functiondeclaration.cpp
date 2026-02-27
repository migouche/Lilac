//
// Created by migouche on 9/13/2023.
//

#include "AST/functiondeclaration.h"
#include "parser/parser_data.h"

#include <utility>
#include <iostream>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>


FunctionDeclaration::FunctionDeclaration(std::string name,
                                         std::vector<Token> domain, std::vector<Token> codomain,
                                         std::vector<FunctionCase> cases, bool pure):
                                         name(std::move(name)), domain(std::move(domain)),
                                         codomain(std::move(codomain)), cases(std::move(cases)),
                                         pure(pure){}

std::ostream& FunctionDeclaration::print(std::ostream& os) const {
    os << "name: " << name << std::endl;
    os << "purity: " << (pure ? "pure": "impure") << std::endl;

    os << "domain: ";
    for(auto& token: domain) {
        os << token.get_value();
        os << ", ";
    }

    os << "\ncodomain: ";
    for(const auto& token: codomain) {
        os << token.get_value();
        os << ", ";
    }
    os << "\ncases: ";
    for(const auto& f_case: cases)
        f_case.print(os);
    return os;
}

llvm::Function *FunctionDeclaration::prototype_codegen(ParserData& parser_data) const{
    /*std::vector<llvm::Type*> dom(domain.size(), llvm::Type::getInt32Ty(*parser_data.context));
    llvm::FunctionType* ft =
            llvm::FunctionType::get(llvm::Type::getInt32Ty(
                    *parser_data.context), dom, false);
    llvm::Function* f =
            llvm::Function::Create(ft,
                                   llvm::Function::ExternalLinkage,
                                   name, *parser_data.module);

    unsigned char i = 'a';
    for(auto& arg: f->args())
        arg.setName(std::string(1, i++)); // dont have names?? for now so im using letters of the alphabet
    return f;*/

    std::vector<llvm::Type*> dom;
    for(const auto& token: domain) {
        if(token.get_value() != "void")
            dom.push_back(parser_data.get_primitive(token.get_value()));
    }
    llvm::Type* returnType = parser_data.get_primitive(codomain.front().get_value());

    llvm::FunctionType* ft = llvm::FunctionType::get(returnType, dom, false);
    llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, *parser_data.module);
    if (cases.size() <= 1) // use those names
    {
        int i = 0;
        char n = 'a';
        for (auto& token: cases.front().inputs)
        {
            if (token.get_value() != "void") {
                if (token.get_token_kind() == IDENTIFIER) {
                    f->getArg(i++)->setName(token.get_value());
                } else {
                    f->getArg(i++)->setName(std::string(1, n++));
                }
            }
        }
    } else
    {
        char i = 'a';
        for(auto& arg: f->args())
            arg.setName(std::string(1, i++));
    }
    return f;
}

llvm::Function* FunctionDeclaration::codegen(ParserData& parser_data) const {
    // get or create function prototype
    llvm::Function* f = parser_data.module->getFunction(name);
    if (!f) f = prototype_codegen(parser_data);
    if (!f || !f->empty()) return nullptr;

    // entry + builder setup
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*parser_data.context, "entry", f);
    parser_data.builder->SetInsertPoint(entry);

    // collect args
    std::vector<llvm::Value*> args;
    for (auto& a : f->args()) args.push_back(&a);

    // only one case, we can optimize some stuff:
    if (cases.size() == 1) {
        parser_data.enter_scope();
        auto arg_it = f->arg_begin();
        for (const auto& input_token : cases.front().inputs) {
             if (input_token.get_token_kind() == IDENTIFIER) {
                 // It's a variable argument
                 llvm::Value* arg_val = &*arg_it;
                 std::string arg_name = input_token.get_value();
                 parser_data.add_value(arg_name, arg_val, arg_val->getType(), false);
             }
             if (arg_it != f->arg_end()) ++arg_it;
        }

        //codegen the body & return
        llvm::Value* retV = cases.front().codegen(parser_data);
        if (retV) {
            // Check if block already has terminator (e.g. from nested control flow - unlikely for single expr but possible if expression involves control flow)
            if (!parser_data.builder->GetInsertBlock()->getTerminator()) {
                 parser_data.builder->CreateRet(retV);
            }
        } else {
            // Error in body codegen
            std::cerr << "Codegen failed for function body: " << name << std::endl;
            f->eraseFromParent(); // Remove the broken function from module
            parser_data.exit_scope();
            return nullptr;
        }
        
        parser_data.exit_scope();
        return f;
    }

    // prepare blocks: one match block per case + a failure block
    std::vector<llvm::BasicBlock*> matchBlocks;
    for (size_t i = 0; i < cases.size(); ++i) {
        matchBlocks.push_back(llvm::BasicBlock::Create(*parser_data.context, "case" + std::to_string(i), f));
    }
    llvm::BasicBlock* failBlock = llvm::BasicBlock::Create(*parser_data.context, "no_match", f);

    // jump from entry to first case
    parser_data.builder->CreateBr(matchBlocks[0]);

    size_t i = 0;
    // emit each case
    for (const auto& [inputs, output] : cases) {
        // set insertion point to the current match block
        parser_data.builder->SetInsertPoint(matchBlocks[i]);

        // build runtime test (cond) over arguments
        llvm::Value* cond = nullptr;
        // Use f->args() if 'args' vector is not available or reliable
        auto argIt = args.begin();
        for (const auto& tok : inputs) {
            if (argIt == args.end()) break;

            if (tok.get_token_kind() == LITERAL) {
                const int lit = std::stoi(tok.get_value());
                const auto cmp = parser_data.builder->CreateICmpEQ(
                    *argIt,
                    llvm::ConstantInt::get(parser_data.get_primitive("int"), lit)
                );
                // Wait, logic 'cond = cond ? AND : cmp' implies if checking multiple args, ALL must match (AND).
                // Yes, pattern matching (a, b) = ... means a AND b match.
                // But previous code logic was:
                /* cond = cond ? parser_data.builder->CreateAnd(cond, cmp) : cmp; */
                // However, CreateAnd needs correct block? Builder is set to matchBlocks[i]. Ok.
                if (cond) {
                     cond = parser_data.builder->CreateAnd(cond, cmp);
                } else {
                     cond = cmp;
                }
            }
            ++argIt;
        }
        
        if (!cond)
            cond = llvm::ConstantInt::getTrue(*parser_data.context);

        // prepare body blocks
        llvm::BasicBlock* body = llvm::BasicBlock::Create(*parser_data.context, "body" + std::to_string(i), f);
        llvm::BasicBlock* next = (i + 1 < cases.size()) ? matchBlocks[i + 1] : failBlock;
        
        parser_data.builder->CreateCondBr(cond, body, next);

        // emit case body
        parser_data.builder->SetInsertPoint(body);
        parser_data.enter_scope();
        argIt = args.begin();
        for (const auto& tok : inputs) {
            if (argIt == args.end()) break;
            
            if (tok.get_token_kind() == IDENTIFIER) {
                parser_data.add_value(tok.get_value(), *argIt, parser_data.get_primitive(tok.get_value()), false);
            }
            ++argIt;
        }
        
        if (llvm::Value* retV = output->codegen(parser_data)) {
            parser_data.builder->CreateRet(retV);
        } else {
             // Error in case body
             std::cerr << "Codegen failed for case " << i << " in " << name << std::endl;
             // Should we erase function effectively?
        }
        parser_data.exit_scope();

        ++i;
    }
    // no pattern matched: emit unreachable (or errorraise)
    parser_data.builder->SetInsertPoint(failBlock);
    parser_data.builder->CreateUnreachable();

    return f;
}

std::ostream& FunctionCase::print(std::ostream& os) const {
    os << "function case: \n\tinputs: \n\t";
    for (const auto& input: inputs)
    {
        input.print(os);
        os << ", ";
    }

    os << "\n\toutput: ";

    output->print(os);
    os << std::endl;
    return os;
}

llvm::Value *FunctionCase::codegen(ParserData& parser_data) const {
    return output->codegen(parser_data);
}
