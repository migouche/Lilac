//
// Created by migouche on 6/09/23.
//

#include <utility>
#include "lexer/filestream.h"
#include "lexer/lexerfunctions.h"
#include "errors/lassert.h"

using namespace lexer_functions;

bool is_operator(std::ifstream& file, std::string& out_string)
{
    if(! is_operator_character(char(file.peek())))
        return false;
    std::string r;

    while(!file.eof() && is_operator_character(char(file.peek())))
    {
        char c;
        file.get(c);
        if(is_end_of_operator(c))
        {
            r += c;
            out_string = r;
            return true;
        }

        if(c == '/' && file.peek() == '/')
        {
            file.putback('/');
            if(out_string.empty())
                return false;
            out_string = r;
            return true;
        }

        r += c;
    }
    out_string = r;
    return true;
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

    if(is_operator(file, r_token))
        return swap_buffer(r_token);

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

        if(is_special_character(c)) {
            if(r_token.empty())
                return swap_buffer({c});
            else {
                file.putback(c);
                break;
            }
        }

        if(is_operator_character(c))
        {
            lassert(!r_token.empty(), "must have read something else already");
            file.putback(c);
            break;
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


