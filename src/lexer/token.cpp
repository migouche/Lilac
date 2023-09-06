//
// Created by migouche on 9/6/2023.
//

#include "lexer/token.h"


int64_t get_multi_byte_from(const char* token)
{
    return details::i64(token, 0);
}

TokenKind get_token_kind(const char* token)
{
    return TokenKind(get_multi_byte_from(token));
}

TokenKind get_token_kind(unsigned char c)
{
    return TokenKind(c);
}
