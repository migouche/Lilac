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
    size_t index = 0;
public:
    explicit Tokenizer(const std::string& path);

    [[nodiscard]] bool end_of_tokens() const;

    Token get_token();
    [[nodiscard]] Token peek_token() const;
};

#endif //LILAC_TOKENIZER_H
