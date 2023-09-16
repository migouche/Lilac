//
// Created by migouche on 9/10/2023.
//

#include <iostream>
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

void Token::print() const {
    if(kind == IDENTIFIER || kind == KEYWORD)
        std::cout << value;
    else
        std::cout << get_string_from_token(kind);
}

std::ostream &operator<<(std::ostream &os, const Token& t) {
    if(t.get_token_kind() == IDENTIFIER)
        return os << "IDENTIFIER: " << t.get_value();
    if(t.get_token_kind() == KEYWORD)
        return os << "KEYWORD: " << t.get_value();
    return os << "'" <<get_string_from_token(t.get_token_kind()) << "'";
}


