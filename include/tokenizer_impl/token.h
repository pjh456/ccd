#pragma once

#include <stdint.h>

typedef struct Tokenizer Tokenizer;

typedef enum
{
    T_IDENTIFIER, // x_a
    T_NUMBER,     // 123
    T_CHARACTER,  // 'a'
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

    T_EXTERN,   // extern
    T_STATIC,   // static
    T_INLINE,   // inline
    T_REGISTER, // register
    T_RESTRICT, // restrict
    T_VOLATILE, // volalite
    T_CONST,    // const

    T_AUTO,     // auto
    T_VOID,     // void
    T_SIGNED,   // signed
    T_UNSIGNED, // unsigned
    T_CHAR,     // char
    T_SHORT,    // short
    T_INT,      // int
    T_LONG,     // long
    T_FLOAT,    // float
    T_DOUBLE,   // double

    T_DO,       // do
    T_WHILE,    // while
    T_FOR,      // for
    T_CONTINUE, // continue
    T_BREAK,    // break
    T_IF,       // if
    T_ELSE,     // else
    T_SWITCH,   // switch
    T_CASE,     // case
    T_DEFAULT,  // default
    T_RETURN,   // return
    T_GOTO,     // goto

    T_ENUM,    // enum
    T_SIZEOF,  // sizeof
    T_STRUCT,  // struct
    T_TYPEDEF, // typedef
    T_UNION,   // union

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