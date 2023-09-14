#include <iostream>
#include <tuple>
#include "lexer/tokenizer.h"
#include "typedefs.h"
#include "errors/lassert.h"
#include "parser/parser.h"

int main() {
    Parser parser("tests/data/function.llc");

    auto tree = parser.get_tree();

    tree.print();

    return 0;

}
