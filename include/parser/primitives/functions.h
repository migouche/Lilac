//
// Created by migouche on 2/11/24.
//

#ifndef LILAC_FUNCTIONS_H
#define LILAC_FUNCTIONS_H

namespace primitive_functions{
    constexpr auto SUM = R"(
    define i32 @sum(i32 %a, i32 %b) {
        %c = add i32 %a, %b
        ret i32 %c
    }
    )";

    constexpr auto DIV = R"(
    define i32 @div(i32 %a, i32 %b) {
        %c = sdiv i32 %a, %b
        ret i32 %c
    }
    )";
}

#endif //LILAC_FUNCTIONS_H
