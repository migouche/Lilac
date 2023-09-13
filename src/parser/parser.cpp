//
// Created by migouche on 9/10/2023.
//

#include "parser/parser.h"

void __l_fail(const std::string& message)
{
    std::string out = "expect failed: ";
    out.append(message);
    throw std::runtime_error(out);
}

#define expect(condition, message) static_cast<bool>(condition) ? void(0) : __l_fail(message)


/* copy just in case
#define expect(condition, message) if (!static_cast<bool>(condition)) { \
    std::string out = "expect failed: ";                                        \
    out.append(message);                                                         \
    throw std::runtime_error(out);} else {(void)0;}\
    */
Parser::Parser(const std::string &path) {
    tokenizer = std::make_unique<Tokenizer>(path);
}

struct FunctionHeader
{
    std::string name;
    std::list<Token> domain;
    std::list<Token> codomain;
};

FunctionHeader get_function_header(const std::unique_ptr<Tokenizer>& tokenizer)
{
    expect(tokenizer->get_token() == Token(TokenKind(1), "func"), "function parsing must start by 'func'");

    bool domain_parens = false;
    bool codomain_parens = false;


    expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER, "function name must be an identifier");
    std::string name = tokenizer->get_token().get_value();

    if(tokenizer->peek_token() == Token(TokenKind('(')))
    {
        domain_parens = true;
        tokenizer->get_token();
    }

    expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER, "function must have at least one input");

    std::list<Token> domain = {};

    while(tokenizer->peek_token().get_token_kind() == IDENTIFIER)
    {
        domain.push_back(tokenizer->get_token());
        if(domain_parens && tokenizer->peek_token() == Token(TokenKind(')')))
        {
            tokenizer->get_token();
            break;
        }
        expect(tokenizer->get_token() == Token(TokenKind(',')), "Function domain must be comma-separated");
    }

    if(domain_parens) // NOTE: still needed (ik there is a lot of redundant code, will fix later)
        expect(tokenizer->get_token() == Token(TokenKind(')')), "no matching parenthesis");

    expect(tokenizer->get_token() == Token(get_multi_byte_token_kind("->")), "expected '->' to introduce return type");

    if(tokenizer->peek_token() == Token(TokenKind('(')))
    {
        codomain_parens = true;
        tokenizer->get_token();
    }

    expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER, "function must have at least one output");

    std::list<Token> codomain = {};

    while(tokenizer->peek_token().get_token_kind() == IDENTIFIER)
    {
        codomain.push_back(tokenizer->get_token());
        if(codomain_parens && tokenizer->peek_token() == Token(TokenKind(')')))
        {
            tokenizer->get_token();
            break;
        }
        expect(tokenizer->get_token() == Token(TokenKind(',')), "Function domain must be comma-separated");
    }


    if(codomain_parens)
        expect(tokenizer->get_token() == Token(TokenKind(')')), "no matching parenthesis");

    return {name, domain, codomain};
}

struct FunctionCase
{
    std::list<Token> inputs;
    std::list<Token> outputs; // TODO: must be an function call, for once functional programming saves us
};

struct FunctionBody
{
    std::list<FunctionCase> cases;
};

FunctionCase parse_function_case(const std::unique_ptr<Tokenizer>& tokenizer)
{
    expect(tokenizer->get_token().get_token_kind() == IDENTIFIER &&
           tokenizer->get_token() == Token(TokenKind('(')), // no beef inferring for now :(
           "function case must start with a function 'definition'");
    expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER, "function case must have at least one input");

    std::list<Token> inputs = {};

    while(tokenizer->peek_token().get_token_kind() == IDENTIFIER)
    {
        expect(tokenizer->peek_token().get_token_kind() == IDENTIFIER, "argument must be an identifier");
        inputs.push_back(tokenizer->get_token());
        if(tokenizer->peek_token() == Token(TokenKind(')')))
        {
            tokenizer->get_token();
            break;
        }
        expect(tokenizer->get_token() == Token(TokenKind(',')), "Function domain must be comma-separated");
    }

    expect(tokenizer->get_token() == Token(TokenKind('=')), "function case must be assigned to something");

    std::list<Token> operation = {};

    while(tokenizer->peek_token() != Token(TokenKind(';')))
    {
        operation.push_back(tokenizer->get_token());
    }
    tokenizer->get_token(); // consume ';'
    return {inputs, operation};
}

FunctionBody parse_function_body(const std::unique_ptr<Tokenizer>& tokenizer)
{
    expect(tokenizer->get_token() == Token(TokenKind('{')), "function must have a body");
    std::list<FunctionCase> cases = {};
    while(tokenizer->peek_token() != Token(TokenKind('}')))
    {
        cases.push_back(parse_function_case(tokenizer));
    }
}

FunctionDeclaration Parser::parse_function() {
    auto header = get_function_header(tokenizer);
    expect(tokenizer->get_token() == Token(TokenKind('{')), "function must have a body");
    auto body = parse_function_body(tokenizer);
    return {header, body};
}

ASTTree Parser::get_tree() {
    while(!tokenizer->end_of_tokens())
    {
        if(tokenizer->peek_token() == Token(TokenKind(1), "func"))
            parse_function();
    }
}