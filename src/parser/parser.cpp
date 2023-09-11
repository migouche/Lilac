//
// Created by migouche on 9/10/2023.
//

#include <cassert>
#include "parser/parser.h"
#include "errors/lassert.h"

#define passert(condition, message) if (!static_cast<bool>(condition)) {\
    throw std::runtime_error("passert failed: ##condition || " + message);}\

Parser::Parser(const std::string &path) {
    tokenizer = std::make_unique<Tokenizer>(path);
}

FunctionDeclaration Parser::parse_function() {
    passert(tokenizer->get_token() == Token(TokenKind(1), "func"), "function parsing must start by 'func'");
    bool domain_parens = false;
    bool codomain_parens = false;
    lassert(tokenizer->peek_token() == Token(TokenKind(2), ""), "function name must be an identifier");
    std::string name = tokenizer->get_token().get_value();
    if(tokenizer->peek_token() == Token(TokenKind('('), ""))
    {
        domain_parens = true;
        tokenizer->get_token();
    }
}

ASTTree Parser::get_tree() {
 return {};
}