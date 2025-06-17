//
// Created by migouche on 9/7/2023.
//

#include "lexer/lexerfunctions.h"

#include "lexer/tokenkind.h"


bool lexer_functions::is_operator_token(const std::string &token)
{
    if(token.length() > 3)
        return false;
    const TokenKind kind = get_token_kind(token);
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
           token == "when" || token == "enum" || token == "impure" || token == "true" || token == "false" || token == "let";
}

bool lexer_functions::is_whitespace(const char c)
{
    return is_newline(c) || c == ' ' || c == '\t';
}

bool lexer_functions::is_newline(const char c)
{
    return c == '\n' || c == '\r';
}

bool lexer_functions::is_numeric(const char c)
{
    return c >= '0' && c <= '9';
}

bool lexer_functions::is_alpha(const char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool lexer_functions::is_alphanumeric(const char c)
{
    return is_alpha(c) || is_numeric(c);
}

bool lexer_functions::is_special_character(const char c)
{
    return c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == ';' || c == ':' ||
    c == '.';
}

bool lexer_functions::is_special_character(const std::string & s)
{
    return s.length() == 1 && is_special_character(s[0]);
}

bool lexer_functions::is_operator_character(const char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '>' || c == '<' || c == '=' || c == '!' ||
    c == '&' || c == '|' || c == '^' || c == '~';
}

bool lexer_functions::is_end_of_operator(const char c) {
    return c == '=';
}
