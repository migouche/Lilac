//
// Created by migouche on 11/09/23.
//

#ifndef LILAC_LASSERT_H
#define LILAC_LASSERT_H

#include <iostream>
#include <cstdlib>

#ifdef NDEBUG
    #define lassert(condition, text) static_cast<void>(0)
#else
    #define lassert(condition, text) \
        if (!(condition)) { \
            std::cerr << "Assertion failed: " << #condition << ", file " << __FILE__ \
                      << ", line " << __LINE__ << ", function " << __func__ \
                      << ": " << (text) << std::endl; \
            std::abort(); \
        }
#endif //NDEBUG

#endif //LILAC_LASSERT_H