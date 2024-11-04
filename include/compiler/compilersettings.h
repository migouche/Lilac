//
// Created by migouche on 4/11/24.
//

#ifndef LILAC_COMPILERSETTINGS_H
#define LILAC_COMPILERSETTINGS_H

#include <llvm/IR/CallingConv.h>

namespace CompilerSettings {
    const constexpr llvm::CallingConv::ID CALLING_CONVENTION = llvm::CallingConv::C;
}

#endif //LILAC_COMPILERSETTINGS_H
