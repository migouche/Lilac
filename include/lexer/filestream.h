//
// Created by migouche on 6/09/23.
//

#ifndef LILAC_FILESTREAM_H
#define LILAC_FILESTREAM_H

#include <fstream>
#include <string>

class FileStream {
private:
    std::ifstream file;
    std::string buffer;

    size_t line = 1, pos = 0;

    std::string swap_buffer(std::string new_buffer);
public:
    explicit FileStream(const std::string& filename);

    std::string peek_token() const;
    std::string get_token(); // NOT CONST

    size_t get_line() const;
    size_t get_pos() const;

    bool is_eof() const;
};


#endif //LILAC_FILESTREAM_H
