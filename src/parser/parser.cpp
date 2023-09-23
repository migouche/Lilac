//
// Created by migouche on 9/10/2023.
//

#include <iostream>
#include "parser/parser.h"
#include "parser/AST/expression.h"

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

    expect(tokenizer->get_token() == Token(KEYWORD, "func"), "function parsing must start by 'func'");

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

/*
struct FunctionCase
{
    std::list<Token> inputs;
    std::list<Token> outputs; // TODO: must be an function call, for once functional programming saves us
};
*/

struct FunctionBody
{
    std::list<FunctionCase> cases;
};

std::shared_ptr<ASTValue> parse_value(const std::unique_ptr<Tokenizer>& tokenizer)
{
    auto tok = tokenizer->get_token();
    expect(tok.get_token_kind() == IDENTIFIER || tok.is_primitive_operation(),
           "function name / expression must be an identifier");
    std::string name;
    if(tok.get_token_kind() == IDENTIFIER)
        name = tok.get_value();
    else
        name = get_string_from_token(tok.get_token_kind());
    if (tokenizer->peek_token().get_token_kind() == OPEN_PARENS) {
        // function
        tokenizer->get_token();// consume the (
        std::list<std::shared_ptr<ASTNode>> arguments = {};
        while (tokenizer->peek_token().get_token_kind() != CLOSE_PARENS)
        {
            arguments.push_back(parse_value(tokenizer));
            if(tokenizer->peek_token().get_token_kind() == COMMA)
                tokenizer->get_token();
            else
                expect(tokenizer->peek_token().get_token_kind() == CLOSE_PARENS,
                       "expected comma or ')' to end function call");
        }
        tokenizer->get_token(); // consume the )
        return std::make_shared<FunctionCall>(name, arguments);
    } else {
        return std::make_shared<Expression>(tok);
    }
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

    tokenizer->get_token(); // consume ';'
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
    return {cases};
}

std::shared_ptr<FunctionDeclaration> get_function_from_parts(const FunctionHeader& h, const FunctionBody& b)
{
    return std::make_shared<FunctionDeclaration>(h.name, h.domain, h.codomain, b.cases);
}

std::shared_ptr<FunctionDeclaration> Parser::parse_function() {
    auto header = get_function_header(tokenizer);
    auto body = parse_function_body(tokenizer);
    return get_function_from_parts(header, body);
}

ASTTree Parser::get_tree() {
    ASTTree tree = ASTTree();
    while(!tokenizer->end_of_tokens())
    {
        if(tokenizer->peek_token() == Token(KEYWORD, "func"))
            tree.add_child(parse_function());
        else
            tokenizer->get_token();
    }

    return tree;
}
