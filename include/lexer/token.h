//
// Created by migouche on 9/6/2023.
//

#ifndef LILAC_TOKEN_H
#define LILAC_TOKEN_H


// Got from https://stackoverflow.com/a/39144576/13916511
//implementation of user-defined literal _i64
#include <cstdint>
#include <string>

#define get_multi_byte_token_kind(token) TokenKind( token##_i64 )

namespace details
{
    constexpr int64_t i64(char const *s, int64_t v)
    {
        //in C++11, constexpr function body has to be one-line
        //though C++14 has relaxed this rule.
        return *s ? i64(s+1, v * 256 + *s) : v;
    }
}

constexpr int64_t operator "" _i64(char const *s, unsigned long)
{
    return details::i64(s, 0);
}

enum TokenKind
{
    ERROR = 0,
    IDENTIFIER = 1,
    OPEN_PARENS = '(',
    CLOSE_PARENS = ')',
    OPEN_CURLEY_BRACE = '{',
    CLOSE_CURLEY_BRACE = '}',
    OPEN_SQUARE_BRACE = '[',
    CLOSE_SQUARE_BRACE = ']',
    COMMA = ',',
    SEMICOLON = ';',
    COLON = ':',
    DOT = '.',
    PLUS = '+',
    MINUS = '-',
    STAR = '*',
    SLASH = '/',
    PERCENT = '%',

    MULTIBYTE_START = 255,

    MINUS_GREATER = "->"_i64,
    PLUS_PLUS = "++"_i64,
    MINUS_MINUS = "--"_i64,
    PLUS_EQUAL = "+="_i64,
    MINUS_EQUAL = "-="_i64,
};

TokenKind get_token_kind(const char*);
TokenKind get_token_kind(unsigned char);

#endif //LILAC_TOKEN_H

