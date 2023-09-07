#include <iostream>
#include <tuple>
#include "lexer/tokenizer.h"
#include "typedefs.h"

int main() {
    Tokenizer tokenizer("data/driver.llc");

    for (const auto& [token, value]: tokenizer)
    {
        if(value.empty())
            std::cout << std::hex << ull(token) << " : "<< get_string_from_token(token) <<  std::endl;
        else
            std::cout << std::hex << ull(token) << " : "<< value << std::endl;

    }


    return 0;

}
