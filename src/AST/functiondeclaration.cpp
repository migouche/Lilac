//
// Created by migouche on 9/13/2023.
//

#include "AST/functiondeclaration.h"
#include "parser/parser_data.h"

#include <utility>
#include <iostream>


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

llvm::Function *FunctionDeclaration::prototype_codegen(const std::shared_ptr<ParserData>& parser_data) const{
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

    unsigned char i = 'a';
    for(auto& arg: f->args())
        arg.setName(std::string(1, i++));
    return f;
}

llvm::Function *FunctionDeclaration::codegen(const std::shared_ptr<ParserData>& parser_data) const {
    llvm::Function* f = parser_data->module->getFunction(name);
    if(!f)
        f = prototype_codegen(parser_data);
    // supposedly it can never be null

    if(!f->empty()) {
        std::cerr << "Error: Function redefinition!\n";
        return nullptr;
    }


    llvm::BasicBlock* bb = llvm::BasicBlock::Create(*parser_data->context,
                                                    "entry", f);
    parser_data->builder->SetInsertPoint(bb);

    parser_data->enter_scope();


    for(auto& arg: f->args())
        parser_data->add_value(std::string(arg.getName()), &arg);

    if(llvm::Value* ret = cases.front().codegen(parser_data))
    {
        parser_data->builder->CreateRet(ret);

        //TODO: check for type consistency
        return f;
    }
    f->eraseFromParent();
    std::cerr << "couldn't compile function :V\n";
    return nullptr;
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

bool FunctionCase::input_match(const std::vector<Token>& tokens) const {

    // tokens is the function call
    // this->inputs is the one that can have variables and '0'

    if(inputs.size() != tokens.size())
    {
        std::cerr << "DIFFERENT SIZES IN TOKENS FROM DECLARATION AND TOKENS FROM CALL\n"
        << "CODE SHOULD NOT HAVE REACHED THIS\n";
        return false;
    }

    for (const auto& [my_tok, call_tok]: llvm::zip(this->inputs, tokens))
    {
        if(my_tok.get_token_kind() != UNDERSCORE || my_tok.get_token_kind() != IDENTIFIER)
            return false;
        if(my_tok.get_token_kind() == UNDERSCORE)
            continue;
        // it is an identifier now


    }

    return false; // TODO: FINISH THIS (for now we just compiling first case)

}

llvm::Value *FunctionCase::codegen(const std::shared_ptr<ParserData>& parser_data) const {
    return output->codegen(parser_data);
}
