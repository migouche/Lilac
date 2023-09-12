//
// Created by migouche on 11/09/23.
//

#ifndef LILAC_LASSERT_H
#define LILAC_LASSERT_H

#include <cassert>

#ifdef NDEBUG

    #define lassert(condition, text) ((void)0)
#else
#  define lassert(condition, text) if (!static_cast<bool>(condition)) {\
    std::string func = __ASSERT_FUNCTION;                              \
    func.append("\n\t");\
    func.append(text);\
    __assert_fail(#condition, __FILE__, __LINE__, func.c_str());}
#endif //NDEBUG


#endif //LILAC_LASSERT_H
