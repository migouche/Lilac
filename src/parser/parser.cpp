//
// Created by migouche on 9/10/2023.
//

#include <iostream>
#include "parser/parser.h"

#include "AST/ASTValues/astblock.h"
#include "AST/ASTValues/astif.h"
#include "AST/ASTValues/literal.h"
#include "AST/ASTValues/tuple.h"
#include "AST/ASTValues/variable.h"

typedef std::vector<std::vector<std::string>> ScopeStack;

void __l_fail(const std::string& message, const std::string& file, const int line)
{
    std::string out = "expect failed at ";
    out.append(file);
    out.append(":");
    out.append(std::to_string(line));
    out.append("\n\t");
    out.append(message);
    throw std::runtime_error(out);
}

#define expect(condition, message) static_cast<bool>(condition) ? void(0) : __l_fail(message,__FILE__, __LINE__)



Parser::Parser(const std::string &path): tokenizer(std::make_unique<Tokenizer>(path)), token_stack(ScopeStack()) {}

struct FunctionHeader
{
    std::string name;
    std::list<Token> domain;
    std::list<Token> codomain;
};

bool stack_has(ScopeStack& stack, const std::string& token)
{
    return std::ranges::any_of(stack, [&](const auto& scope) {
        return std::ranges::find(scope, token) != scope.end();
    });
}


FunctionHeader get_function_header(const std::unique_ptr<Tokenizer>& tokenizer, bool* pure)
{

    expect(tokenizer->peek_token().get_info() == std::make_pair(KEYWORD, std::string ("func")) ||
            tokenizer->peek_token().get_info() == std::make_pair(KEYWORD, std::string("impure")),
           "function parsing must start by 'func' or by 'impure func'"); // cant put template here

    *pure = true;
    if(tokenizer->get_token().get_value() == "impure")
    {
        tokenizer->get_token(); // consume 'func'
        *pure = false;
    }

    bool domain_parens = false;
    bool codomain_parens = false;


    expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER, "function name must be an identifier");
    const std::string name = tokenizer->get_token().get_value();

    if(tokenizer->peek_token().get_token_kind() == CLOSE_PARENS)
    {
        domain_parens = true;
        tokenizer->get_token();
    }

    expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER, "function must have at least one input");

    std::list<Token> domain = {};

    while(tokenizer->peek_token().get_token_kind() == IDENTIFIER)
    {
        domain.push_back(tokenizer->get_token());

        if(tokenizer->peek_token().get_token_kind() == get_multi_byte_token_kind("->"))
            break;
        if(domain_parens && tokenizer->peek_token().get_token_kind() == CLOSE_PARENS)
        {
            tokenizer->get_token();
            break;
        }
        expect(tokenizer->get_token().get_token_kind() == COMMA, "Function domain must be comma-separated");
    }

    if(domain_parens) // NOTE: still needed (ik there is a lot of redundant code, will fix later)
        expect(tokenizer->get_token().get_token_kind() == CLOSE_PARENS, "no matching parenthesis");

    expect(tokenizer->get_token().get_token_kind() == get_multi_byte_token_kind("->"),
           "expected '->' to introduce return type");

    if(tokenizer->peek_token().get_token_kind() == static_cast<TokenKind>('('))
    {
        codomain_parens = true;
        tokenizer->get_token();
    }

    expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER, "function must have at least one output");

    std::list<Token> codomain = {};

    while(tokenizer->peek_token().get_token_kind() == IDENTIFIER)
    {
        codomain.push_back(tokenizer->get_token());
        if(tokenizer->peek_token().get_token_kind() == OPEN_CURLEY_BRACE)
            break;
        if(codomain_parens && tokenizer->peek_token().get_token_kind() == CLOSE_PARENS)
        {
            tokenizer->get_token();
            break;
        }
        expect(tokenizer->get_token().get_token_kind() == COMMA, "Function codomain must be comma-separated");
    }


    if(codomain_parens)
        expect(tokenizer->get_token().get_token_kind() == CLOSE_PARENS, "no matching parenthesis");

    return {name, domain, codomain};
}


struct FunctionBody
{
    std::list<FunctionCase> cases;
};

template<typename T>
using ast_tuple = std::pair<bool, std::shared_ptr<T>>;

std::shared_ptr<ASTValue> parse_value(const std::unique_ptr<Tokenizer>& tokenizer, ScopeStack& token_stack);

ast_tuple<Tuple> parse_tuple(const std::unique_ptr<Tokenizer>& tokenizer, ScopeStack& token_stack)
{
    if(tokenizer->peek_token().get_token_kind() != OPEN_SQUARE_BRACE)
        return {false, nullptr};
    tokenizer->get_token(); // consume the '['
    std::list<std::shared_ptr<ASTValue>> elements;
    while(tokenizer->peek_token().get_token_kind() != CLOSE_SQUARE_BRACE)
    {
        elements.push_back(parse_value(tokenizer, token_stack));
        if(tokenizer->peek_token().get_token_kind() == COMMA)
            tokenizer->get_token(); // consume the comma;
        else
            expect(tokenizer->peek_token().get_token_kind() == CLOSE_SQUARE_BRACE, "no matching ']'");

    }
    expect(tokenizer->get_token().get_token_kind() == CLOSE_SQUARE_BRACE, "expected ']'");
    return {true, std::make_shared<Tuple>(elements)};
}

ast_tuple<ASTIf> parse_if(const std::unique_ptr<Tokenizer>& tokenizer, ScopeStack& token_stack) {
    if(tokenizer->peek_token().get_info() != std::make_pair(KEYWORD, "if"))
        return {false, nullptr};
    tokenizer->get_token(); // consume 'if'
    expect(tokenizer->get_token().get_token_kind() == OPEN_PARENS, "expected '(' after 'if'");
    std::shared_ptr<ASTValue> condition = parse_value(tokenizer, token_stack);
    expect(tokenizer->get_token().get_token_kind() == CLOSE_PARENS, "expected ')' after condition");
    std::shared_ptr<ASTValue> then_branch = parse_value(tokenizer, token_stack);
    const auto else_token = tokenizer->get_token();
    expect(else_token.get_token_kind() == KEYWORD && else_token.get_value() == "else", "expected 'else' after 'then' branch");
    std::shared_ptr<ASTValue> else_branch = parse_value(tokenizer, token_stack);

    return {true, std::make_shared<ASTIf>(condition, then_branch, else_branch)};
}


ast_tuple<FunctionCall> parse_function_call(const std::unique_ptr<Tokenizer>& tokenizer,
ScopeStack& token_stack)
{
    if(!((tokenizer->peek_token().get_token_kind() == IDENTIFIER || tokenizer->peek_token().is_primitive_operation())
    && tokenizer->peek_token(1).get_token_kind() == OPEN_PARENS))
        return {false, nullptr};

    const auto t = tokenizer->get_token();
    expect(t.get_token_kind() == IDENTIFIER || t.is_primitive_operation(), "expected function name as identifier");
    expect(tokenizer->get_token().get_token_kind() == OPEN_PARENS, "expected function parens");
    auto name = t.get_token_kind() == IDENTIFIER ? t.get_value(): get_string_from_token(t.get_token_kind());
    std::list<std::shared_ptr<ASTValue>> arguments = {};
    while(tokenizer->peek_token().get_token_kind() != CLOSE_PARENS)
    {
        arguments.push_back(parse_value(tokenizer, token_stack));
        if (tokenizer->peek_token().get_token_kind() == COMMA)
            tokenizer->get_token();
        else
            expect(tokenizer->peek_token().get_token_kind() == CLOSE_PARENS,
                   "expected comma or ')' to end function call");
    }
    expect(tokenizer->get_token().get_token_kind() == CLOSE_PARENS, "expected ')'");
    return {true, std::make_shared<FunctionCall>(name, arguments)};
}

ast_tuple<ASTValue> parse_expression(const std::unique_ptr<Tokenizer>& tokenizer, ScopeStack& token_stack)
{
    if (const auto peek_t = tokenizer->peek_token(); peek_t.get_token_kind() != IDENTIFIER && peek_t.get_token_kind() !=
        LITERAL && peek_t.get_token_kind() != UNDERSCORE && !(tokenizer->peek_token().get_token_kind() == KEYWORD &&
            (peek_t.get_value() == "true" || peek_t.get_value() == "false")))
        return {false, nullptr};
    auto t = tokenizer->get_token();
    if(t.get_token_kind() == IDENTIFIER) {
        if (!stack_has(token_stack, t.get_value()))
            throw std::runtime_error("Identifier '" + t.get_value() + "' not found in scope");
        return {true, std::make_shared<Variable>(t)};
    }
    if(t.get_token_kind() == LITERAL || t.get_token_kind() == KEYWORD) // no extra checks because they're done before
        return {true, std::make_shared<Literal>(t)};
    if(t.get_token_kind() == UNDERSCORE)
        throw std::runtime_error("underscore not implemented yet");
    throw std::runtime_error("Couldn't parse expression");
}

ast_tuple<ASTDefinition> internal_parse_definition(const std::unique_ptr<Tokenizer>& tokenizer, ScopeStack& scope_stack)
{
    if (tokenizer->peek_token().get_token_kind() != KEYWORD || tokenizer->peek_token().get_value() != "let")
        return {false, nullptr};

    tokenizer->get_token(); // consume 'let'

    const auto name = tokenizer->get_token();
    expect(name.get_token_kind() == IDENTIFIER, "definition name must be an identifier");

    expect(tokenizer->get_token().get_token_kind() == EQUAL, "definition must be assigned to something");

    const auto value = parse_value(tokenizer, scope_stack);
    expect(tokenizer->get_token().get_token_kind() == SEMICOLON, "expected semicolon after definition");

    scope_stack.back().push_back(name.get_value());

    // check if its global (if the stack has size > 1, then it is a local definition)
    auto is_global = scope_stack.size() <= 1;

    return {true, std::make_shared<ASTDefinition>(name.get_value(), value, is_global)};
}

ast_tuple<ASTBlock> parse_block(const std::unique_ptr<Tokenizer>& tokenizer, ScopeStack& scope_stack)
{
    if (tokenizer->peek_token().get_token_kind() != OPEN_CURLEY_BRACE)
        return {false, nullptr};
    tokenizer->get_token(); // consume the '{'
    scope_stack.emplace_back();
    std::vector<std::shared_ptr<ASTDefinition>> definitions;
    while (true)
    {
        if (const auto [is_def, def] = internal_parse_definition(tokenizer, scope_stack); is_def)
            definitions.push_back(def);
        else
            break;
    }

    const auto tail_expression = parse_value(tokenizer, scope_stack);
    scope_stack.pop_back();
    expect(tokenizer->get_token().get_token_kind() == CLOSE_CURLEY_BRACE, "expected '}' to close block (Dont use ; on tail expression!)");
    return {true, std::make_shared<ASTBlock>(definitions, tail_expression)};
}

std::shared_ptr<ASTValue> parse_value(const std::unique_ptr<Tokenizer>& tokenizer,  ScopeStack& token_stack)
{
    if(const auto [is_tuple, tuple] = parse_tuple(tokenizer, token_stack); is_tuple)
        return tuple;
    if(const auto [is_function_call, function_call] = parse_function_call(tokenizer, token_stack); is_function_call)
        return function_call;
    if(const auto [is_expression, expression] = parse_expression(tokenizer, token_stack); is_expression)
        return expression;
    if(const auto [is_if, ast_if] = parse_if(tokenizer, token_stack); is_if)
        return ast_if;
    if (const auto [is_block, block] = parse_block(tokenizer, token_stack); is_block)
        return block;

    const auto fail_token = tokenizer->peek_token();
    std::cerr << "Failed to parse value at " << fail_token.get_line() << ":" << fail_token.get_pos()
              << " with token '" << fail_token.get_value() << "' of kind " << get_string_from_token(fail_token.get_token_kind()) << std::endl;
    throw std::runtime_error("Couldn't parse value");
}


FunctionCase parse_function_case(const std::unique_ptr<Tokenizer>& tokenizer, const FunctionHeader& header, ScopeStack& token_stack)
{
    token_stack.emplace_back();
    //expect((tokenizer->peek_token().get_token_kind() == IDENTIFIER || tokenizer->get_token().get_token_kind() == DOT) &&
    //       tokenizer->get_token().get_token_kind() == OPEN_PARENS,
    //       "function case must start with a function 'definition'");

    const auto peek = tokenizer->peek_token();

    expect(peek.get_token_kind() == IDENTIFIER || peek.get_token_kind() == DOT, "expected function name or dot before '('");
    (void)tokenizer->get_token();
    expect(tokenizer->get_token().get_token_kind() == OPEN_PARENS, "function case must start with '('");

    expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER || // with lazy evaluation, second condition being checked means first is false
              tokenizer->peek_token().get_token_kind() == UNDERSCORE ||
                tokenizer->peek_token().get_token_kind() == LITERAL ||
    (header.domain.size() == 1 && header.domain.front().get_value() == "void" && tokenizer->get_token().get_token_kind() == CLOSE_PARENS),
           "function case must have at least one input or be of type void -> output");

    std::list<Token> inputs = {};

    while(tokenizer->peek_token().get_token_kind() == IDENTIFIER ||
            tokenizer->peek_token().get_token_kind() == UNDERSCORE ||
            tokenizer->peek_token().get_token_kind() == LITERAL)
    {
        expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER ||
               tokenizer->peek_token().get_token_kind() == UNDERSCORE ||
               tokenizer->peek_token().get_token_kind() == LITERAL
               , "argument must be an identifier, underscore or literal");
        inputs.push_back(tokenizer->get_token());
        if(tokenizer->peek_token().get_token_kind() == CLOSE_PARENS)
        {
            tokenizer->get_token();
            break;
        }
        expect(tokenizer->get_token().get_token_kind() == COMMA, "Function domain must be comma-separated");
    }

    expect(tokenizer->get_token().get_token_kind() == EQUAL, "function case must be assigned to something");

    //std::list<Token> operation = {};

    for (const auto& input : inputs)
        token_stack.back().push_back(input.get_value());

    const auto _return = parse_value(tokenizer, token_stack);

    const auto tok = tokenizer->get_token();
    expect(tok.get_token_kind() == SEMICOLON, "expected semicolon, got " + get_string_from_token(tok.get_token_kind()) + tok.get_value());
    token_stack.pop_back();
    return {inputs, _return};
}

FunctionBody parse_function_body(const std::unique_ptr<Tokenizer>& tokenizer, const FunctionHeader& header, ScopeStack& token_stack)
{
    expect(tokenizer->get_token().get_token_kind() == OPEN_CURLEY_BRACE, "function must have a body, got");

    const auto peek = tokenizer->peek_token();
    if (header.domain.size() == 1 && header.domain.front().get_value() == "void" &&
        peek.get_token_kind() != DOT && !(peek.get_token_kind() == IDENTIFIER && peek.get_value() == header.name))
    {
        // just parse the value and construct an imaginary case
        auto value = parse_value(tokenizer, token_stack);
        expect(tokenizer->get_token().get_token_kind() == SEMICOLON,
               "expected semicolon after function body, got " + get_string_from_token(tokenizer->get_token().get_token_kind()) +
               tokenizer->get_token().get_value());
        expect(tokenizer->get_token().get_token_kind() == CLOSE_CURLEY_BRACE,
           "expected '}' to close function body, got " + get_string_from_token(tokenizer->get_token().get_token_kind()) +
           tokenizer->get_token().get_value());

        if (!value)
        {
            std::cerr << "error: function body must have a body, got" << std::endl;
        }

        return FunctionBody{std::list{FunctionCase{{}, value}}};
    }

    std::list<FunctionCase> cases = {};
    while(tokenizer->peek_token().get_token_kind() != CLOSE_CURLEY_BRACE)
    {
        cases.push_back(parse_function_case(tokenizer, header, token_stack));
    }
    expect(tokenizer->get_token().get_token_kind() == CLOSE_CURLEY_BRACE,
           "expected '}', although this should never happen :/");
    return {cases};
}

std::shared_ptr<FunctionDeclaration> get_function_from_parts(const FunctionHeader& h, const FunctionBody& b, bool pure)
{
    return std::make_shared<FunctionDeclaration>(h.name, h.domain, h.codomain, b.cases, pure);
}

std::shared_ptr<FunctionDeclaration> Parser::parse_function() {
    bool pure;
    const auto header = get_function_header(tokenizer, &pure);
    token_stack.back().push_back(header.name);
    const auto body = parse_function_body(tokenizer, header, token_stack);
    return get_function_from_parts(header, body, pure);
}

std::shared_ptr<ASTDefinition> Parser::parse_definition() {
    const auto [is_def, def] = internal_parse_definition(tokenizer, token_stack);
    if (!is_def)
        throw std::runtime_error("Expected definition, got " + get_string_from_token(tokenizer->peek_token().get_token_kind()));
    return def;
}

ASTTree Parser::get_tree()  { // NOTE: must be a copy because compiler lifetime is much greater than parser
    auto tree = ASTTree();
    token_stack.emplace_back();
    while(!tokenizer->end_of_tokens())
    {
        if (const auto peek = tokenizer->peek_token();
            peek.get_info() == std::make_pair(KEYWORD, std::string("func")) ||
            peek.get_info() == std::make_pair(KEYWORD, std::string("impure")))
            tree.add_child(this->parse_function());
        else if (peek.get_info() == std::make_pair(KEYWORD, std::string("let")))
            tree.add_child(this->parse_definition());

    }

    std::cout << "Known funcs: ";
    for (const auto& f: token_stack.back())  // last scope is the global scope, where all functions are defined
    {
        std::cout << f << " ";
    }
    std::cout << std::endl;

    return tree;
}

