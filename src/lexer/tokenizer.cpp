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
    while(!s.is_eof()) { // TODO: can def do this on the go but eh, it works
        std::string token = s.get_token();
        if (is_operator_token(token) || is_special_character(token))
            tokens.emplace_back(get_token_kind(token), s.get_line(), s.get_pos());
        else if(is_keyword(token))
            tokens.emplace_back(KEYWORD, token, s.get_line(), s.get_pos());
        else
            tokens.emplace_back(IDENTIFIER, token, s.get_line(), s.get_pos());
    }
}

Token Tokenizer::get_token() {
    if(end_of_tokens())
        throw std::runtime_error("unexpected EOF");
    auto t = peek_token();
    index++;
    return t;
}

Token Tokenizer::peek_token() const {
    return peek_token(0);
}

bool Tokenizer::end_of_tokens() const {
    return index >= tokens.size();
}

Token Tokenizer::peek_token(size_t i) const {
    if(index + i >= tokens.size())
        throw std::runtime_error("Unexpected EOF");

    auto it = tokens.begin();
    std::advance(it, index + i);
    return *it;
}
