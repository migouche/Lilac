//
// Created by migouche on 14/09/23.
//

#include "parser/AST/astnode.h"

void ASTNode::add_child(const std::shared_ptr<ASTNode>& node) {
    children.push_back(node);
}