//
// Created by migouche on 9/10/2023.
//

#include <iostream>
#include "lexer/token.h"

Token::Token(TokenKind k, std::string s, size_t line, size_t pos): kind(k), value(std::move(s)) , line(line), pos(pos){}

Token::Token(TokenKind k, size_t line, size_t pos): Token(k, "", line, pos) {}

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


bool Token::is_primitive_operation() const {
    if(!value.empty())
        return false;
    return kind == PLUS || kind == MINUS || kind == STAR || kind == SLASH || kind == EQUAL_EQUAL || kind == LESS ||
    kind == GREATER || kind == LESS_EQUAL || kind == GREATER_EQUAL;
}

std::pair<TokenKind, std::string> Token::get_info() const {
    return {kind, value};
}

size_t Token::get_line() const {
    return line;
}

size_t Token::get_pos() const {
    return pos;
}

bool Token::is_variable() const {
    return get_token_kind() ==  IDENTIFIER && isalpha(get_value().at(0)); // will have to rework
}


std::ostream &operator<<(std::ostream &os, const Token& t) {
    os << t.get_line() << ":" << t.get_pos() << " ";
    if(t.get_token_kind() == IDENTIFIER)
        return os << "IDENTIFIER: " << t.get_value();
    if(t.get_token_kind() == KEYWORD)
        return os << "KEYWORD: " << t.get_value();
    if(t.get_token_kind() == LITERAL)
        return os << "LITERAL: " << t.get_value();
    return os << "'" << get_string_from_token(t.get_token_kind()) << "'";
}


