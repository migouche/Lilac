//
// Created by migouche on 9/10/2023.
//

#include "lexer/token.h"

Token::Token(TokenKind k, std::string s) : kind(k), value(std::move(s)) {}
Token::Token(TokenKind k): kind(k)
{
    value = "";
}

std::string Token::get_value() const {
    return value;
}

TokenKind Token::get_token_kind() const {
    return kind;
}

bool Token::operator==(const Token &other) const {
    return kind == other.kind && value == other.value;
}