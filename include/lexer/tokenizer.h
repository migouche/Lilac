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
    std::list<Token> tokens;
public:
    explicit Tokenizer(const std::string& path);


    std::_List_iterator<Token> begin();
    std::_List_iterator<Token> end();
};

#endif //LILAC_TOKENIZER_H
