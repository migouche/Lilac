//
// Created by migouche on 9/6/2023.
//

#ifndef LILAC_TOKENKIND_H
#define LILAC_TOKENKIND_H

#include "typedefs.h"

#define get_multi_byte_token_kind(token) TokenKind( token##_u64 )


// Got from https://stackoverflow.com/a/39144576/13916511
//implementation of user-defined literal _u64
#include <string>


namespace details
{
    constexpr u64 string_to_u64(char const *s, const u64 v)
    {
        //in C++11, constexpr function body has to be one-line
        //though C++14 has relaxed this rule.
        return *s ? string_to_u64(s + 1, v * 256 + *s) : v;
    }
}

constexpr u64 operator ""_u64(char const *s, unsigned long)
{
    return details::string_to_u64(s, 0);
}

enum TokenKind
{
    ERROR = 0,
    IDENTIFIER = 1,
    KEYWORD = 2,
    LITERAL = 3,
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

    UNDERSCORE = '_',

    MULTIBYTE_START = 255,

    MINUS_GREATER = "->"_u64,
    PLUS_PLUS = "++"_u64,
    MINUS_MINUS = "--"_u64,
    PLUS_EQUAL = "+="_u64,
    MINUS_EQUAL = "-="_u64,
    STAR_EQUAL = "*="_u64,
    SLASH_EQUAL = "/="_u64,
    PERCENT_EQUAL = "%="_u64,
    GREATER_GREATER = ">>"_u64,
    LESS_LESS = "<<"_u64,
    GREATER_EQUAL = ">="_u64,
    LESS_EQUAL = "<="_u64,
    EQUAL_EQUAL = "=="_u64,
    EXCLAMATION_EQUAL = "!="_u64,
    AMPERSAND_EQUAL = "&="_u64,
    PIPE_EQUAL = "|="_u64,
    CARET_EQUAL = "^="_u64,
    AMPERSAND_AMPERSAND = "&&"_u64,
    PIPE_PIPE = "||"_u64,
    AMPERSAND_AMPERSAND_EQUAL = "&&="_u64,
    PIPE_PIPE_EQUAL = "||="_u64,
    GREATER_GREATER_EQUAL = ">>="_u64,
    LESS_LESS_EQUAL = "<<="_u64,
};

TokenKind get_token_kind(const std::string& token);
TokenKind get_token_kind(unsigned char);
std::string get_string_from_token(TokenKind);


#endif //LILAC_TOKENKIND_H

