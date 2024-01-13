//
// Created by migouche on 6/09/23.
//

#ifndef LILAC_FILESTREAM_H
#define LILAC_FILESTREAM_H

#include <fstream>
#include <string>
#include <list>

class FileStream {
private:
    std::ifstream file;
    std::string buffer;

    size_t line = 1, pos = 0;

    std::string swap_buffer(std::string new_buffer);

    std::istream& get(char& c);
    int get();
    std::istream& putback(char c);
    int peek();
    bool eof();

    std::list<std::pair<size_t, size_t>> pos_buffer;
public:
    explicit FileStream(const std::string& filename);

    std::string peek_token() const;
    std::string get_token(); // NOT CONST  // we add pos and line in here

    std::pair<size_t, size_t> get_pos();

    size_t get_stream_pos();

    bool is_eof() const;

    friend bool is_operator(FileStream& file, std::string& out_string);
};


#endif //LILAC_FILESTREAM_H
