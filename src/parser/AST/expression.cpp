//
// Created by migouche on 22/09/23.
//

#include "parser/AST/expression.h"

#include <utility>
#include <iostream>

Expression::Expression(Token token): token(std::move(token)) {}

void Expression::print() const {std::cout << token.get_value(); }