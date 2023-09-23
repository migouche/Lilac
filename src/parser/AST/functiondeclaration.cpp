//
// Created by migouche on 9/13/2023.
//

#include "parser/AST/functiondeclaration.h"

#include <utility>
#include <iostream>

FunctionDeclaration::FunctionDeclaration(std::string name,
                                         std::list<Token> domain, std::list<Token> codomain,
                                         std::list<FunctionCase> cases):
                                         name(std::move(name)), domain(std::move(domain)),
                                         codomain(std::move(codomain)), cases(std::move(cases)){}

void FunctionDeclaration::print() const {
    std::cout << "name: " << name << std::endl;

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
