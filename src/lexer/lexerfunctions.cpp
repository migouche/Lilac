//
// Created by migouche on 9/7/2023.
//

#include "lexer/lexerfunctions.h"


bool lexer_functions::is_operator_token(const std::string &token)
{
    if(token.length() > 3)
        return false;
    TokenKind kind = get_token_kind(token);
    return kind == PLUS || kind == MINUS || kind == STAR || kind == SLASH || kind == PERCENT || kind == GREATER ||
           kind == LESS || kind == EQUAL || kind == EXCLAMATION || kind == AMPERSAND || kind == PIPE || kind == CARET ||
           kind == TILDE || kind == MINUS_GREATER || kind == PLUS_PLUS || kind == MINUS_MINUS || kind == PLUS_EQUAL ||
           kind == MINUS_EQUAL || kind == STAR_EQUAL || kind == SLASH_EQUAL || kind == PERCENT_EQUAL ||
           kind == GREATER_GREATER || kind == LESS_LESS || kind == GREATER_EQUAL || kind == LESS_EQUAL ||
           kind == EQUAL_EQUAL || kind == EXCLAMATION_EQUAL || kind == AMPERSAND_EQUAL || kind == PIPE_EQUAL ||
           kind == CARET_EQUAL || kind == AMPERSAND_AMPERSAND || kind == PIPE_PIPE ||
           kind == AMPERSAND_AMPERSAND_EQUAL || kind == PIPE_PIPE_EQUAL || kind == GREATER_GREATER_EQUAL ||
           kind == LESS_LESS_EQUAL;
}

bool lexer_functions::is_keyword(const std::string &token)
{
    return token == "if" || token == "else" || token == "while" || token == "for" || token == "do" || token == "func" ||
           token == "when" || token == "enum" || token == "impure";
}

bool lexer_functions::is_whitespace(char c, size_t* line)
{
    if(is_newline(c, line))
    {
        (*line)++;
        return true;
    }
    return c == ' ' || c == '\t';
}

bool lexer_functions::is_newline(char c, size_t* line)
{
    auto b =  c == '\n' || c == '\r';
    if(b)
        (*line)++;
    return b;
}

bool lexer_functions::is_numeric(char c)
{
    return c >= '0' && c <= '9';
}

bool lexer_functions::is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool lexer_functions::is_alphanumeric(char c)
{
    return is_alpha(c) || is_numeric(c);
}

bool lexer_functions::is_special_character(char c)
{
    return c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == ';' || c == ':' ||
    c == '.';
}

bool lexer_functions::is_special_character(const std::string & s)
{
    return s.length() == 1 && is_special_character(s[0]);
}

bool lexer_functions::is_operator_character(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '>' || c == '<' || c == '=' || c == '!' ||
    c == '&' || c == '|' || c == '^' || c == '~';
}

bool lexer_functions::is_end_of_operator(char c) {
    return c == '=';
}
