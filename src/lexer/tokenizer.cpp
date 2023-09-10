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

std::_List_iterator<Token> Tokenizer::begin() {
    return tokens.begin();
}

std::_List_iterator<Token> Tokenizer::end() {
    return tokens.end();
}