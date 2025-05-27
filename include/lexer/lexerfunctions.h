//
// Created by migouche on 9/7/2023.
//

#ifndef LILAC_LEXERFUNCTIONS_H
#define LILAC_LEXERFUNCTIONS_H

#include <string>

namespace lexer_functions
{
    bool is_operator_token(const std::string& token);
    bool is_keyword(const std::string& token);

    bool is_whitespace(char c);
    bool is_newline(char c);
    bool is_numeric(char c);
    bool is_alpha(char c);
    bool is_alphanumeric(char c);
    bool is_special_character(char c);
    bool is_special_character(const std::string&);
    bool is_operator_character(char c);
    bool is_end_of_operator(char c);
}

#endif //LILAC_LEXERFUNCTIONS_H
