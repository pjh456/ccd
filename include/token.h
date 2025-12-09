#pragma once

#include <stdint.h>

typedef struct Tokenizer Tokenizer;

typedef enum
{
    T_IDENTIFIER, // x_a
    T_NUMBER,     // 123
    T_CHAR,       // 'a'
    T_STRING,     // "xx"

    T_NOT,         // !
    T_TILDE,       // ~
    T_AND,         // &
    T_OR,          // |
    T_XOR,         // ^
    T_LEFT_SHIFT,  // <<
    T_RIGHT_SHIFT, // >>

    T_PLUS,   // +
    T_MINUS,  // -
    T_STAR,   // *
    T_DIV,    // /
    T_MOD,    // %
    T_ASSIGN, // =

    T_LESS,          // <
    T_GREATER,       // >
    T_EQUAL,         // ==
    T_NOT_EQUAL,     // !=
    T_LESS_EQUAL,    // <=
    T_GREATER_EQUAL, // >=
    T_AND_AND,       // &&
    T_OR_OR,         // ||

    T_AND_ASSIGN,         // &=
    T_OR_ASSIGN,          // |=
    T_XOR_ASSIGN,         // ^=
    T_LEFT_SHIFT_ASSIGN,  // <<=
    T_RIGHT_SHIFT_ASSIGN, // <<=

    T_PLUS_ASSIGN,  // +=
    T_MINUS_ASSIGN, // -=
    T_MUL_ASSIGN,   // *=
    T_DIV_ASSIGN,   // /=
    T_MOD_ASSIGN,   // %=
    T_INC,          // ++
    T_DEC,          // --

    T_PREPROCESS, // #
    T_BACKSLASH,  // 反斜杠

    T_LEFT_PAREN,    // (
    T_RIGHT_PAREN,   // )
    T_LEFT_BRACKET,  // [
    T_RIGHT_BRACKET, // ]
    T_LEFT_BRACE,    // {
    T_RIGHT_BRACE,   // }

    T_COMMA,     // ,
    T_COLON,     // :
    T_SEMICOLON, // ;
    T_DOT,       // .
    T_ARROW,     // ->
    T_QUESTION,  // ?

    T_EOF,    // EOF
    T_UNKNOWN // Unknown
} TokenType;

typedef struct
{
    TokenType type;
    const char *start;
    size_t length;
    int line;
    int col;
} Token;

const char *token_name(TokenType tt);
void print_token(const Token *t);
Token make_token(Tokenizer *tk, TokenType tt, size_t len);