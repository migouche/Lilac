//
// Created by migouche on 6/09/23.
//

#include <utility>
#include "lexer/filestream.h"

// separators: ' ', '\n', '\t', '\r', '\n\r', '\r\n'

bool is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool is_newline(char c)
{
    return c == '\n' || c == '\r';
}

bool is_numeric(char c)
{
    return c >= '0' && c <= '9';
}

bool is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_alphanumeric(char c)
{
    return is_alpha(c) || is_numeric(c);
}

bool is_single_character_token(char c)
{
    return c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == ';' || c == ':' || c == '.';
}

std::string FileStream::get_token() {
    std::string r_token;
    if(!file)
    {
        if(buffer.empty())
            throw std::runtime_error("empty stream");
        else
        {
            auto r = buffer;
            buffer = "";
            return r;
        }
    }


    while(is_whitespace(char(file.peek())))
        file.get();

    while (!file.eof()) {
        char c;
        file.get(c);




        if(c == '/' && char(file.peek()) == '/')
        {
            while(!file.eof() && !is_newline(char(file.peek())))
                file.get(c);
            continue;
        }

        if (is_whitespace(c))
        {
            if(r_token.empty())
                continue;
            break;
        }

        if(is_single_character_token(c)) {
            if(r_token.empty())
                return swap_buffer({c});
            else {
                file.putback(c);
                break;
            }
        }

        if(!file.eof()) // I don't know why but this is needed (can be reworked into do while, but later)
            r_token += c;

    }


    return swap_buffer(r_token);
}

std::string FileStream::peek_token() const {
    return buffer;
}

FileStream::FileStream(const std::string& filename) {
    file.open(filename);

    if (!file) {
        throw std::runtime_error("Could not open file " + filename);
    }

    get_token();
}

bool FileStream::is_eof() const {
    return buffer.empty();
}

std::string FileStream::swap_buffer(std::string new_buffer) {
    std::string r = buffer;
    buffer = std::move(new_buffer);
    return r;
}


