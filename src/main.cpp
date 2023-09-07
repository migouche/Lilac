#include <iostream>
#include "lexer/filestream.h"
#include "lexer/token.h"

int main() {
    auto file = FileStream("data/driver.llc");

    std::cout << "tokens: \n\n";
    while(!file.is_eof())
        std::cout << file.get_token() << std::endl;


    return 0;

}
