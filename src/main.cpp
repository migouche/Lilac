#include <iostream>
#include <tuple>
#include "lexer/tokenizer.h"
#include "typedefs.h"
#include "errors/lassert.h"

int main() {
    Tokenizer tokenizer("data/driver.llc");

    while(!tokenizer.end_of_tokens())
    {
        auto t = tokenizer.get_token();
        auto value = t.get_value();
        auto token = t.get_token_kind();
        if(value.empty())
            std::cout << std::hex << u64(token) << " : "<< get_string_from_token(token) <<  std::endl;
        else
            std::cout << std::hex << u64(token) << " : "<< value << std::endl;

    }

    return 0;

}
