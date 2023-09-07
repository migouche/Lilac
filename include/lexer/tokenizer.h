//
// Created by migouche on 9/7/2023.
//

#ifndef LILAC_TOKENIZER_H
#define LILAC_TOKENIZER_H

#include <list>
#include "token.h"

class Tokenizer
{
private:
    std::list<std::tuple<TokenKind, std::string>> tokens;
public:
    explicit Tokenizer(const std::string& path);

    std::_List_iterator<std::tuple<TokenKind, std::string>> begin();
    std::_List_iterator<std::tuple<TokenKind, std::string>> end();
};

#endif //LILAC_TOKENIZER_H
