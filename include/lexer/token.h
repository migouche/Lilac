//
// Created by migouche on 9/6/2023.
//

#ifndef LILAC_TOKEN_H
#define LILAC_TOKEN_H

#define get_multi_byte_token_kind(token) TokenKind( token##_i64 )


// Got from https://stackoverflow.com/a/39144576/13916511
//implementation of user-defined literal _i64
#include <cstdint>
#include <string>


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
    KEYWORD = 2,
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
    GREATER = '>',
    LESS = '<',
    EQUAL = '=',
    EXCLAMATION = '!',
    AMPERSAND = '&',
    PIPE = '|',
    CARET = '^',
    TILDE = '~',

    MULTIBYTE_START = 255,

    MINUS_GREATER = "->"_i64,
    PLUS_PLUS = "++"_i64,
    MINUS_MINUS = "--"_i64,
    PLUS_EQUAL = "+="_i64,
    MINUS_EQUAL = "-="_i64,
    STAR_EQUAL = "*="_i64,
    SLASH_EQUAL = "/="_i64,
    PERCENT_EQUAL = "%="_i64,
    GREATER_GREATER = ">>"_i64,
    LESS_LESS = "<<"_i64,
    GREATER_EQUAL = ">="_i64,
    LESS_EQUAL = "<="_i64,
    EQUAL_EQUAL = "=="_i64,
    EXCLAMATION_EQUAL = "!="_i64,
    AMPERSAND_EQUAL = "&="_i64,
    PIPE_EQUAL = "|="_i64,
    CARET_EQUAL = "^="_i64,
    AMPERSAND_AMPERSAND = "&&"_i64,
    PIPE_PIPE = "||"_i64,
    AMPERSAND_AMPERSAND_EQUAL = "&&="_i64,
    PIPE_PIPE_EQUAL = "||="_i64,
    GREATER_GREATER_EQUAL = ">>="_i64,
    LESS_LESS_EQUAL = "<<="_i64,
};

TokenKind get_token_kind(const std::string& token);
TokenKind get_token_kind(unsigned char);
std::string get_string_from_token(TokenKind);


#endif //LILAC_TOKEN_H

