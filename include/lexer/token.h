//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_TOKEN_H
#define LILAC_TOKEN_H

#include "lexer/tokenkind.h"

class Token
{
private:
    TokenKind kind;
    std::string value;

public:
    Token(TokenKind, std::string, size_t, size_t);
    Token(TokenKind, size_t, size_t);

    const size_t line;
    const size_t pos;

    [[nodiscard]] std::string get_value() const;
    [[nodiscard]] TokenKind get_token_kind() const;
    [[nodiscard]] bool is_primitive_operation() const;

    [[nodiscard]] std::pair<TokenKind, std::string> get_info() const;

    [[nodiscard]] size_t get_line() const;
    [[nodiscard]] size_t get_pos() const;

    bool operator==(const Token& other) const;

    [[nodiscard]] bool is_variable() const;

    std::ostream& print(std::ostream& os) const;
};

std::ostream &operator<<(std::ostream &os, const Token& t);

#endif //LILAC_TOKEN_H
