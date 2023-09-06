//
// Created by migouche on 6/09/23.
//

#include <iostream>
#include "lexer/filestream.h"

// separators: ' ', '\n', '\t', '\r', '\n\r', '\r\n'

bool is_separator(char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
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

    while(r_token.empty()) {

        while (!file.eof()) {
            char c;
            file.get(c);

            if(c == '/' && char(file.peek()) == '/')
            {
                while(!file.eof() && !is_newline(char(file.peek())))
                    file.get(c);
                continue;
            }

            if (is_separator(c))
                break;
            if(!file.eof()) // I don't know why but this is needed
                r_token += c;
        }
    }

    std::string r = buffer;
    buffer = r_token;
    return r;
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


