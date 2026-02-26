//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_PARSER_H
#define LILAC_PARSER_H

#include <memory>
#include <expected>
#include <vector>

#include "lexer/tokenizer.h"
#include "AST/asttree.h"
// Forward declarations
class FunctionDeclaration;
class FunctionCall;
class ASTDefinition;
class ASTValue;
class Tuple;
class ASTIf;
class ASTBlock;


class Parser
{
private:
    std::unique_ptr<Tokenizer> tokenizer;
    std::expected<std::unique_ptr<FunctionDeclaration>, std::string> parse_function();
    std::expected<std::unique_ptr<FunctionCall>, std::string> parse_call();
    std::expected<std::unique_ptr<ASTDefinition>, std::string> parse_definition();
    std::expected<std::unique_ptr<ASTValue>, std::string> parse_value();
    std::expected<std::unique_ptr<Tuple>, std::string> parse_tuple();
    std::expected<std::unique_ptr<ASTIf>, std::string> parse_if();
    std::expected<std::unique_ptr<ASTBlock>, std::string> parse_block();

    std::vector<std::vector<std::string>> token_stack;

public:

    explicit Parser(const std::string&);
    ASTTree get_tree() ;
};
#endif //LILAC_PARSER_H
