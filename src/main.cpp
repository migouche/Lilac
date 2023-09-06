#include <iostream>
#include "lexer/filestream.h"
#include "lexer/token.h"

int main() {
    auto file = FileStream("data/driver.llc");

    while(!file.is_eof())
        std::cout << file.get_token() << std::endl;


    return 0;

}
