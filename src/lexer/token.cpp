//
// Created by migouche on 9/6/2023.
//

#include <functional>
#include "lexer/token.h"
#include "typedefs.h"


int64_t get_multi_byte_from(const std::string& token)
{
    return details::i64(token.c_str(), 0);
}

TokenKind get_token_kind(const std::string& token)
{
    return TokenKind(get_multi_byte_from(token));
}

TokenKind get_token_kind(unsigned char c)
{
    return TokenKind(c);
}

std::string get_string_from_token(TokenKind token)
{
    std::string out;
    ull n = token;
    while (n > 0)
    {
        out += char(n % 0x100);
        n >>= 8;
    }
    std::reverse(out.begin(), out.end());
    return out;
}
