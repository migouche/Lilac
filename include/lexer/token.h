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
    Token(TokenKind, std::string);
    explicit Token(TokenKind);

    [[nodiscard]] std::string get_value() const;
    [[nodiscard]] TokenKind get_token_kind() const;

    bool operator==(const Token& other) const;
};

#endif //LILAC_TOKEN_H
