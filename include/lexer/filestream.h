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
public:
    explicit FileStream(const std::string& filename);

    std::string peek_token() const;
    std::string get_token();

    bool is_eof() const;
};


#endif //LILAC_FILESTREAM_H
