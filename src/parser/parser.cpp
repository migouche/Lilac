//
// Created by migouche on 9/10/2023.
//

#include <iostream>
#include "parser/parser.h"
#include "AST/ASTValues/literal.h"
#include "AST/ASTValues/tuple.h"
#include "AST/ASTValues/variable.h"
#include <llvm/Support/ErrorHandling.h>

void __l_fail(const std::string& message, const std::string& file, int line)
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


Parser::Parser(const std::string &path): tokenizer(std::make_unique<Tokenizer>(path)) {}

struct FunctionHeader
{
    std::string name;
    std::list<Token> domain;
    std::list<Token> codomain;
};

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
    std::string name = tokenizer->get_token().get_value();

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

    if(tokenizer->peek_token().get_token_kind() == TokenKind('('))
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
using ast_tuple = std::tuple<bool, std::shared_ptr<T>>;

std::shared_ptr<ASTValue> parse_value(const std::unique_ptr<Tokenizer>& tokenizer);

ast_tuple<Tuple> parse_tuple(const std::unique_ptr<Tokenizer>& tokenizer)
{
    if(tokenizer->peek_token().get_token_kind() != OPEN_SQUARE_BRACE)
        return {false, nullptr};
    tokenizer->get_token(); // consume the '['
    std::list<std::shared_ptr<ASTValue>> elements;
    while(tokenizer->peek_token().get_token_kind() != CLOSE_SQUARE_BRACE)
    {
        elements.push_back(parse_value(tokenizer));
        if(tokenizer->peek_token().get_token_kind() == COMMA)
            tokenizer->get_token(); // consume the comma;
        else
            expect(tokenizer->peek_token().get_token_kind() == CLOSE_SQUARE_BRACE, "no matching ']'");

    }
    expect(tokenizer->get_token().get_token_kind() == CLOSE_SQUARE_BRACE, "expected ']'");
    return {true, std::make_shared<Tuple>(elements)};
}

ast_tuple<FunctionCall> parse_function_call(const std::unique_ptr<Tokenizer>& tokenizer)
{
    if(!((tokenizer->peek_token().get_token_kind() == IDENTIFIER || tokenizer->peek_token().is_primitive_operation())
    && tokenizer->peek_token(1).get_token_kind() == OPEN_PARENS))
        return {false, nullptr};

    auto t = tokenizer->get_token();
    expect(t.get_token_kind() == IDENTIFIER || t.is_primitive_operation(), "expected function name as identifier");
    expect(tokenizer->get_token().get_token_kind() == OPEN_PARENS, "expected function parens");
    auto name = t.get_token_kind() == IDENTIFIER ? t.get_value(): get_string_from_token(t.get_token_kind());
    std::list<std::shared_ptr<ASTValue>> arguments = {};
    while(tokenizer->peek_token().get_token_kind() != CLOSE_PARENS)
    {
        arguments.push_back(parse_value(tokenizer));
        if (tokenizer->peek_token().get_token_kind() == COMMA)
            tokenizer->get_token();
        else
            expect(tokenizer->peek_token().get_token_kind() == CLOSE_PARENS,
                   "expected comma or ')' to end function call");
    }
    expect(tokenizer->get_token().get_token_kind() == CLOSE_PARENS, "expected ')'");
    return {true, std::make_shared<FunctionCall>(name, arguments)};
}

ast_tuple<ASTValue> parse_expression(const std::unique_ptr<Tokenizer>& tokenizer)
{
    if(tokenizer->peek_token().get_token_kind() != IDENTIFIER)
        return {false, nullptr};
    auto t = tokenizer->get_token();
    if(isdigit(t.get_value().at(0)))
        return {true, std::make_shared<Literal>(t)};
    else
        return {true, std::make_shared<Variable>(t)};
}

std::shared_ptr<ASTValue> parse_value(const std::unique_ptr<Tokenizer>& tokenizer)
{
    auto [is_tuple, tuple] = parse_tuple(tokenizer);
    if(is_tuple)
        return tuple;
    auto [is_function_call, function_call] = parse_function_call(tokenizer);
    if(is_function_call)
        return function_call;
    auto [is_expression, expression] = parse_expression(tokenizer);
    if(is_expression)
        return expression;
    throw std::runtime_error("Couldn't parse value");
}


FunctionCase parse_function_case(const std::unique_ptr<Tokenizer>& tokenizer)
{
        expect(tokenizer->get_token().get_token_kind() == IDENTIFIER &&
           tokenizer->get_token().get_token_kind() == OPEN_PARENS, // no beef inferring for now :(
           "function case must start with a function 'definition'");
    expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER, "function case must have at least one input");

    std::list<Token> inputs = {};

    while(tokenizer->peek_token().get_token_kind() == IDENTIFIER)
    {
        expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER, "argument must be an identifier");
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

    auto _return = parse_value(tokenizer);

    auto tok = tokenizer->get_token();
    expect(tok.get_token_kind() == SEMICOLON, "expected semicolon, got " + get_string_from_token(tok.get_token_kind()) + tok.get_value());
    return {inputs, _return};
}

FunctionBody parse_function_body(const std::unique_ptr<Tokenizer>& tokenizer)
{
    expect(tokenizer->get_token().get_token_kind() == OPEN_CURLEY_BRACE, "function must have a body, got");
    std::list<FunctionCase> cases = {};
    while(tokenizer->peek_token().get_token_kind() != CLOSE_CURLEY_BRACE)
    {
        cases.push_back(parse_function_case(tokenizer));
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
    auto header = get_function_header(tokenizer, &pure);
    auto body = parse_function_body(tokenizer);
    return get_function_from_parts(header, body, pure);
}

ASTTree Parser::get_tree() { // NOTE: must be a copy because compiler lifetime is much greater than parser
    ASTTree tree = ASTTree();
    while(!tokenizer->end_of_tokens())
    {
        expect(tokenizer->peek_token().get_info() == std::make_pair(KEYWORD, std::string("func"))||
           tokenizer->peek_token().get_info() == std::make_pair(KEYWORD, std::string("impure")),
                   "expected function declaration, got " + get_string_from_token(tokenizer->peek_token().get_token_kind()));

        tree.add_child(parse_function());
    }

    return tree;
}

