//
// Created by migouche on 6/09/23.
//

#include <utility>
#include "lexer/filestream.h"
#include "lexer/lexerfunctions.h"
#include "errors/lassert.h"

using namespace lexer_functions;

bool is_operator(FileStream& file, std::string& out_string)
{
    if(! is_operator_character(static_cast<char>(file.peek())))
        return false;
    std::string r;

    while(!file.eof() && is_operator_character(static_cast<char>(file.peek())))
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


    while(is_whitespace(static_cast<char>(peek())))
        get();

    //pos_buffer.emplace_front(pos, line);

    if(is_operator(*this, r_token)) {
        pos_buffer.emplace_back(pos, line);
        return swap_buffer(r_token);
    }

    while (!eof()) {
        char c;
        get(c);




        if(c == '/' && static_cast<char>(peek()) == '/')
        {
            while(!eof() && !is_newline(static_cast<char>(peek())))
                get(c);
            continue;
        }

        if (is_whitespace(c))
        {
            if(r_token.empty())
                continue;
            break;
        }
        //pos_buffer.emplace_back(pos, line);

        if(is_special_character(c)) {
            if(r_token.empty()) {
                pos_buffer.emplace_back(pos, line);
                return swap_buffer({c});
            }
            else {
                putback(c);
                break;
            }
        }

        if(is_operator_character(c))
        {
            lassert(!r_token.empty(), "must have read something else already");
            putback(c);
            break;
        }

        if(!file.eof()) // I don't know why but this is needed (can be reworked into do while, but later)
            r_token += c;

    }

    pos_buffer.emplace_back(pos, line);

    return swap_buffer(r_token);
}

std::string FileStream::peek_token() const {
    return buffer;
}

FileStream::FileStream(const std::string& filename) {
    file.open(filename);
    pos_buffer = {};
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


std::pair<size_t, size_t> FileStream::get_pos()
{
    auto p = pos_buffer.front();
    pos_buffer.pop_front();
    return p;
}

std::istream& FileStream::get(char &c) {
    pos++;
    if(is_newline(static_cast<char>(peek()))) {
        pos = 0;
        line++;
    }
    return file.get(c);
}

int FileStream::peek() {
    return file.peek();
}

std::istream &FileStream::putback(const char c) {
    pos--;
    return file.putback(c);
}

int FileStream::get() {
    return file.get();
}

bool FileStream::eof() const {
    return file.eof();
}

size_t FileStream::get_stream_pos() {
    return file.tellg();
}


