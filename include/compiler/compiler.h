//
// Created by migouche on 11/14/2023.
//

#ifndef LILAC_COMPILER_H
#define LILAC_COMPILER_H

#include <string>
#include "parser/AST/asttree.h"
#include <vector>

class Compiler
{
private:
    void compile_file(const char*);
    std::vector<ASTTree> trees;

    bool finished;
public:
    template<typename... Args>
    explicit Compiler(Args... files)
    {
        trees = {};
        finished = false;
            for(const auto f: {files...}) {
                if(!finished)
                    compile_file(f);
            }
    }

    ASTTree& get_tree(size_t i = 0);

    std::string status();
};

#endif //LILAC_COMPILER_H
