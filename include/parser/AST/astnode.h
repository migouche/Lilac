//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_ASTNODE_H
#define LILAC_ASTNODE_H

#include <list>
#include <memory>

class ASTNode
{
protected:
    std::list<std::shared_ptr<ASTNode>> children = {};
public:
    virtual void print() const = 0;
    void add_child(const std::shared_ptr<ASTNode>&);
    virtual ~ASTNode() = default;
 };

#endif //LILAC_ASTNODE_H
