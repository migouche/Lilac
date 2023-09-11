//
// Created by migouche on 9/7/2023.
//

#include <tuple>
#include "lexer/tokenizer.h"
#include "lexer/filestream.h"
#include "lexer/lexerfunctions.h"

using namespace lexer_functions;

Tokenizer::Tokenizer(const std::string &path)
{
    FileStream s(path);
    tokens = std::list<Token>();
    while(!s.is_eof()) {
        auto token = s.get_token();
        if (is_operator_token(token) || is_special_character(token))
            tokens.emplace_back(get_token_kind(token), "");
        else if(is_keyword(token))
            tokens.emplace_back(KEYWORD, token);
        else
            tokens.emplace_back(IDENTIFIER, token);
    }
}

Token Tokenizer::get_token() {
    if(end_of_tokens())
        throw std::runtime_error("end of tokens");
    auto t = peek_token();
    index++;
    return t;
}

Token Tokenizer::peek_token() const {
    auto it = tokens.begin();
    std::advance(it, index);
    return *it;
}

bool Tokenizer::end_of_tokens() const {
    return index >= tokens.size();
}
