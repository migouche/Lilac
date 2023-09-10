//
// Created by migouche on 9/10/2023.
//

#include "parser/parser.h"

Parser::Parser(const std::string &path) {
    tokenizer = std::make_unique<Tokenizer>(path);
}