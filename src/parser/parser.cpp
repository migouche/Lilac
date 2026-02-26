#include <iostream>
#include <algorithm>
#include <ranges>
#include "parser/parser.h"

#include "AST/ASTValues/astblock.h"
#include "AST/ASTValues/astif.h"
#include "AST/ASTValues/literal.h"
#include "AST/ASTValues/tuple.h"
#include "AST/ASTValues/variable.h"
#include "AST/ASTValues/functioncall.h"
#include "lexer/tokenkind.h"

// Helper to check scope
bool stack_has(const std::vector<std::vector<std::string>>& stack, const std::string& token) {
    for (const auto& scope : stack | std::views::reverse) {
        if (std::ranges::find(scope, token) != scope.end()) {
            return true;
        }
    }
    return false;
}

Parser::Parser(const std::string &path): tokenizer(std::make_unique<Tokenizer>(path)) {
    token_stack.emplace_back(); // Global scope
}

// Helper macros for expected propagation
#define TRY(x) ({ auto val = (x); if (!val) return std::unexpected(val.error()); std::move(*val); })
#define EXPECT(cond, msg) if (!(cond)) return std::unexpected(std::string(msg));

std::expected<std::unique_ptr<ASTValue>, std::string> Parser::parse_value() {
    auto token = tokenizer->peek_token();

    if (token.get_token_kind() == OPEN_SQUARE_BRACE) {
        auto res = parse_tuple();
        if (res) return std::move(*res);
        return std::unexpected(res.error());
    }
    
    if (token.get_info() == std::make_pair(KEYWORD, std::string("if"))) {
        auto res = parse_if();
        if (res) return std::move(*res);
        return std::unexpected(res.error());
    }

    if (token.get_token_kind() == OPEN_CURLEY_BRACE) {
        auto res = parse_block();
        if (res) return std::move(*res);
        return std::unexpected(res.error());
    }

    // Function Call or Identifier or Literal
    if (token.get_token_kind() == IDENTIFIER || token.is_primitive_operation()) {
        if (tokenizer->peek_token(1).get_token_kind() == OPEN_PARENS) {
            auto res = parse_call();
            if (res) return std::move(*res);
            return std::unexpected(res.error());
        }
        
        // Variable
        if (token.get_token_kind() == IDENTIFIER) {
            tokenizer->get_token(); // consume
            if (!stack_has(token_stack, token.get_value())) {
                return std::unexpected("Identifier '" + token.get_value() + "' not found in scope");
            }
            return std::make_unique<Variable>(token);
        }
    }

    if (token.get_token_kind() == LITERAL || 
        (token.get_token_kind() == KEYWORD && (token.get_value() == "true" || token.get_value() == "false"))) {
        tokenizer->get_token(); // consume
        return std::make_unique<Literal>(token);
    }

    return std::unexpected("Unexpected token in value parsing: " + token.get_value());
}

std::expected<std::unique_ptr<Tuple>, std::string> Parser::parse_tuple() {
    tokenizer->get_token(); // consume '['
    std::vector<std::unique_ptr<ASTValue>> elements;
    
    while (tokenizer->peek_token().get_token_kind() != CLOSE_SQUARE_BRACE) {
        auto val = parse_value();
        if (!val) return std::unexpected(val.error());
        elements.push_back(std::move(*val));

        if (tokenizer->peek_token().get_token_kind() == COMMA) {
            tokenizer->get_token();
        } else if (tokenizer->peek_token().get_token_kind() != CLOSE_SQUARE_BRACE) {
            return std::unexpected("Expected comma or ']' in tuple");
        }
    }
    tokenizer->get_token(); // consume ']'
    return std::make_unique<Tuple>(std::move(elements));
}

std::expected<std::unique_ptr<ASTIf>, std::string> Parser::parse_if() {
    tokenizer->get_token(); // consume 'if'
    EXPECT(tokenizer->get_token().get_token_kind() == OPEN_PARENS, "Expected '(' after if");

    auto condition = parse_value();
    if (!condition) return std::unexpected(condition.error());

    EXPECT(tokenizer->get_token().get_token_kind() == CLOSE_PARENS, "Expected ')' after condition");

    auto then_branch = parse_value();
    if (!then_branch) return std::unexpected(then_branch.error());

    auto else_token = tokenizer->get_token();
    EXPECT(else_token.get_token_kind() == KEYWORD && else_token.get_value() == "else", "Expected 'else'");

    auto else_branch = parse_value();
    if (!else_branch) return std::unexpected(else_branch.error());

    return std::make_unique<ASTIf>(std::move(*condition), std::move(*then_branch), std::move(*else_branch));
}

std::expected<std::unique_ptr<FunctionCall>, std::string> Parser::parse_call() {
    auto name_token = tokenizer->get_token(); // consume name
    tokenizer->get_token(); // consume '('
    
    std::string name = name_token.get_token_kind() == IDENTIFIER ? name_token.get_value() : get_string_from_token(name_token.get_token_kind());
    std::vector<std::unique_ptr<ASTValue>> args;

    while (tokenizer->peek_token().get_token_kind() != CLOSE_PARENS) {
        auto val = parse_value();
        if (!val) return std::unexpected(val.error());
        args.push_back(std::move(*val));

        if (tokenizer->peek_token().get_token_kind() == COMMA) {
            tokenizer->get_token();
        } else if (tokenizer->peek_token().get_token_kind() != CLOSE_PARENS) {
            return std::unexpected("Expected comma or ')' in function call");
        }
    }
    tokenizer->get_token(); // consume ')'
    
    return std::make_unique<FunctionCall>(name, std::move(args));
}

std::expected<std::unique_ptr<ASTDefinition>, std::string> Parser::parse_definition() {
    tokenizer->get_token(); // consume 'let'
    auto name_token = tokenizer->get_token();
    EXPECT(name_token.get_token_kind() == IDENTIFIER, "Expected identifier for definition name");
    
    EXPECT(tokenizer->get_token().get_token_kind() == EQUAL, "Expected '=' in definition");

    auto value = parse_value();
    if (!value) return std::unexpected(value.error());

    EXPECT(tokenizer->get_token().get_token_kind() == SEMICOLON, "Expected ';' after definition");

    token_stack.back().push_back(name_token.get_value());
    bool is_global = token_stack.size() <= 1;

    return std::make_unique<ASTDefinition>(name_token.get_value(), std::move(*value), is_global);
}

std::expected<std::unique_ptr<ASTBlock>, std::string> Parser::parse_block() {
    tokenizer->get_token(); // consume '{'
    token_stack.emplace_back();

    std::vector<std::unique_ptr<ASTDefinition>> definitions;

    // Parse definitions
    while (tokenizer->peek_token().get_info() == std::make_pair(KEYWORD, std::string("let"))) {
        auto def = parse_definition();
        if (!def) return std::unexpected(def.error());
        definitions.push_back(std::move(*def));
    }

    // Parse tail expression
    auto tail = parse_value();
    if (!tail) return std::unexpected(tail.error());

    token_stack.pop_back();
    EXPECT(tokenizer->get_token().get_token_kind() == CLOSE_CURLEY_BRACE, "Expected '}'");

    return std::make_unique<ASTBlock>(std::move(definitions), std::move(*tail));
}

std::expected<std::unique_ptr<FunctionDeclaration>, std::string> Parser::parse_function() {
    bool pure = true;
    auto start = tokenizer->peek_token();
    if (start.get_value() == "impure") {
        pure = false;
        tokenizer->get_token();
    }
    tokenizer->get_token(); // consume 'func'

    auto name_token = tokenizer->get_token();
    EXPECT(name_token.get_token_kind() == IDENTIFIER, "Expected function name");

    // simplified parsing for now, assuming standard structure
    // Domain
    if (tokenizer->peek_token().get_token_kind() == OPEN_PARENS) tokenizer->get_token();
    
    std::vector<Token> domain;
    while (tokenizer->peek_token().get_token_kind() == IDENTIFIER) {
        domain.push_back(tokenizer->get_token());
        if (tokenizer->peek_token().get_token_kind() == COMMA) tokenizer->get_token();
        else break;
    }
    if (tokenizer->peek_token().get_token_kind() == CLOSE_PARENS) tokenizer->get_token();

    EXPECT(tokenizer->get_token().get_token_kind() == get_multi_byte_token_kind("->"), "Expected '->'");

    // Codomain
    if (tokenizer->peek_token().get_token_kind() == OPEN_PARENS) tokenizer->get_token(); // optional parens
    std::vector<Token> codomain;
    while (tokenizer->peek_token().get_token_kind() == IDENTIFIER) {
        codomain.push_back(tokenizer->get_token());
        if (tokenizer->peek_token().get_token_kind() == COMMA) tokenizer->get_token();
        else break;
    }
    if (tokenizer->peek_token().get_token_kind() == CLOSE_PARENS) tokenizer->get_token(); // optional parens

    // Body
    EXPECT(tokenizer->get_token().get_token_kind() == OPEN_CURLEY_BRACE, "Expected '{' for function body");
    
    // Register function name in current scope (global usually)
    token_stack.back().push_back(name_token.get_value());
    token_stack.emplace_back(); // Function scope

    // Cases
    std::vector<FunctionCase> cases;
    while (tokenizer->peek_token().get_token_kind() != CLOSE_CURLEY_BRACE) {
        // Check for explicit case syntax: func_name( ...
        // We only consider it a case definition if the identifier matches the function name.
        bool is_explicit_case = false;
        if (tokenizer->peek_token().get_token_kind() == IDENTIFIER) {
             if (tokenizer->peek_token().get_value() == name_token.get_value()) { // Must match function name
                 if (tokenizer->peek_token(1).get_token_kind() == OPEN_PARENS) {
                     is_explicit_case = true;
                 }
             }
        }

        if (is_explicit_case) {
            auto case_name_token = tokenizer->get_token(); // consume name
            tokenizer->get_token(); // consume '('
            
            std::vector<Token> inputs;
            while (tokenizer->peek_token().get_token_kind() != CLOSE_PARENS) {
                inputs.push_back(tokenizer->get_token());
                if (tokenizer->peek_token().get_token_kind() == COMMA) tokenizer->get_token();
            }
            tokenizer->get_token(); // consume ')'
            
            // Add inputs to scope
            token_stack.emplace_back(); 
            for(const auto& t : inputs) {
                token_stack.back().push_back(t.get_value());
            }

            EXPECT(tokenizer->get_token().get_token_kind() == EQUAL, "Expected '=' in function case");
            
            auto val = parse_value();
            if (!val) return std::unexpected(val.error());

            EXPECT(tokenizer->get_token().get_token_kind() == SEMICOLON, "Expected ';' at end of case");
            
            cases.push_back({inputs, std::move(*val)});
            token_stack.pop_back(); 
        } else {
            // Implicit case (body directly)
            
            // Check if we have definitions (let ...) which implies a block-like structure directly in function body
            // But usually function body is a single expression.
            // If the user writes `func ... { let x = 1; x }`, that's valid if we parse it as a sequence of definitions ending in expression.
            // This mirrors parse_block logic.
            // But parse_block expects '{'.
            // Here we are already inside '{' of the function.
            // So we should reuse block parsing logic but without consuming '{' and '}'?
            // Or maybe treating it as a block AST node?
            
            // Wait, if we allow `let` here, we are essentially parsing a block content.
            // And since we are in `while !CLOSE_BRACE`, we might be parsing multiple statements?
            // But `cases.push_back` expects one expression per case.
            // If the body has multiple statements, it should be wrapped in Block.
            
            // If we see `let`, we can parse definitions until we hit an expression?
            // And then wrap it in a ASTBlock?
            // But ASTBlock usually corresponds to `{ ... }`.
            
            if (tokenizer->peek_token().get_info() == std::make_pair(KEYWORD, std::string("let"))) {
                 // Inline block logic
                 std::vector<std::unique_ptr<ASTDefinition>> definitions;
                 while (tokenizer->peek_token().get_info() == std::make_pair(KEYWORD, std::string("let"))) {
                     auto def = parse_definition();
                     if (!def) return std::unexpected(def.error());
                     definitions.push_back(std::move(*def));
                 }
                 auto tail = parse_value();
                 if (!tail) return std::unexpected(tail.error());
                 
                 // If there was a list of definitions, we should probably expect the function body to end here?
                 // Or rather, this whole sequence IS the case body.
                 
                 // Check for optional semicolon after tail check
                 auto semi = tokenizer->peek_token();
                 if (semi.get_token_kind() == SEMICOLON) {
                      tokenizer->get_token(); 
                 }
                 
                 // Wrap in block
                 auto block = std::make_unique<ASTBlock>(std::move(definitions), std::move(*tail));
                 cases.push_back({{}, std::move(block)});
                 
            } else {
                 // Standard expression
                 auto val = parse_value();
                 if (!val) return std::unexpected(val.error());

                 auto semi = tokenizer->peek_token();
                 if (semi.get_token_kind() == SEMICOLON) {
                      tokenizer->get_token(); 
                 } else {
                      return std::unexpected("Expected ';' after function body expression, got: " + semi.get_value());
                 }
                 cases.push_back({{}, std::move(*val)});
            }
        }
    }
    tokenizer->get_token(); // consume '}'
    token_stack.pop_back();

    return std::make_unique<FunctionDeclaration>(name_token.get_value(), domain, codomain, std::move(cases), pure);
}

ASTTree Parser::get_tree() {
    ASTTree tree;
    while (!tokenizer->end_of_tokens()) {
        auto peek = tokenizer->peek_token();
        if (peek.get_value() == "func" || peek.get_value() == "impure") {
            auto func = parse_function();
            if (func) tree.add_child(std::move(*func));
            else std::cerr << "Error parsing function: " << func.error() << std::endl;
        } else if (peek.get_value() == "let") {
            auto def = parse_definition();
            if (def) tree.add_child(std::move(*def));
            else std::cerr << "Error parsing definition: " << def.error() << std::endl;
        } else {
            // consume unknown to avoid infinite loop
            tokenizer->get_token();
        }
    }
    return tree;
}
