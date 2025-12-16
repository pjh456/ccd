#pragma once

typedef struct Vector Vector;
typedef struct Token Token;
typedef struct DeclParam DeclParam;
typedef struct DeclSpecifier DeclSpecifier;
typedef struct Declarator Declarator;
typedef struct DeclInitializer DeclInitializer;
typedef struct StatementUnit StatementUnit;
typedef struct DeclUnit DeclUnit;
typedef struct DeclParser DeclParser;

int is_in_typedef_table(char *name);
int is_in_sue_table(char *name);

int is_declaration_statement(StatementUnit *su);

Token *peek_token_in_stmt(StatementUnit *stmt, size_t pos);

DeclUnit *parse_decl_statement(DeclParser *dp);
DeclSpecifier *parse_decl_specifier(DeclParser *dp);

Declarator *parse_declarator(DeclParser *dp);
Declarator *parse_pointer(DeclParser *dp);
Declarator *parse_direct_declarator(DeclParser *dp);

unsigned parse_pointer_qualifiers(DeclParser *dp);
Declarator *parse_array_suffix(DeclParser *dp, Declarator *decl);
Declarator *parse_function_suffix(DeclParser *dp, Declarator *decl);

Vector *parse_decl_initializer_list(DeclParser *dp);
DeclInitializer *parse_decl_initializer(DeclParser *dp);

Vector *parse_decl_param_list(DeclParser *dp);
DeclParam *parse_decl_param(DeclParser *dp);