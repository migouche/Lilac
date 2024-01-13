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

llvm::Function *FunctionDeclaration::prototype_codegen() const{
    std::vector<llvm::Type*> dom(domain.size(), llvm::Type::getInt32Ty(*parser_data::context));
    llvm::FunctionType* ft =
            llvm::FunctionType::get(llvm::Type::getInt32Ty(
                    *parser_data::context), dom, false);
    llvm::Function* f =
            llvm::Function::Create(ft,
                                   llvm::Function::ExternalLinkage,
                                   name, *parser_data::module);

    unsigned char i = 0;
    for(auto& arg: f->args())
        arg.setName(std::string(1, 'a' + i++)); // dont have names?? for now so im using letters of the alphabet
    return f;
}

llvm::Function *FunctionDeclaration::codegen() const {
    llvm::Function* f = parser_data::module->getFunction(name);
    if(!f)
        f = prototype_codegen();
    // supposedly it can never be null

    llvm::BasicBlock* bb = llvm::BasicBlock::Create(*parser_data::context,
                                                   "entry", f);
    parser_data::builder->SetInsertPoint(bb);


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

bool FunctionCase::input_match(std::vector<Token> tokens) const {
    for (const auto& t: tokens)
    {
        if(t.get_token_kind() != UNDERSCORE || t.get_token_kind() != IDENTIFIER)
            return false;
        if(t.get_token_kind() == UNDERSCORE)
            continue;
        // it is an identifier now

    }

}
