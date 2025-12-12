#pragma once

typedef struct Token Token;
typedef struct Tokenizer Tokenizer;

void skip_comment(Tokenizer *tk);
void skip_space(Tokenizer *tk);
Token *tokenize_keyword(Tokenizer *tk);
Token *tokenize_number(Tokenizer *tk);
Token *tokenize_char(Tokenizer *tk);
Token *tokenize_string(Tokenizer *tk);
Token *tokenize_operator(Tokenizer *tk);
Token *tokenize_preprocessor(Tokenizer *tk);
Token *tokenize_eof(Tokenizer *tk);
Token *tokenize_unknown(Tokenizer *tk);
