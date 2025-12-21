//
// Created by Liao
//
#pragma once

#ifndef CCLONEDETECTOR_AST_H
#define CCLONEDETECTOR_AST_H

#include "vector.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct SrcLocation {
    const char *file;
    int32_t line;
    int32_t column;
    int32_t offset;
} SrcLocation;

typedef struct SrcRange {
    SrcLocation start;
    SrcLocation end;
} SrcRange;


typedef struct AstNode              AstNode;
typedef struct AstDecl              AstDecl;
typedef struct AstStmt              AstStmt;
typedef struct AstExpr              AstExpr;
typedef struct AstType              AstType;
typedef struct AstInit              AstInit;
typedef struct AstParam             AstParam;
typedef struct AstEnumItem          AstEnumItem;
typedef struct AstTranslationUnit   AstTranslationUnit;

typedef struct Symbol               Symbol;
typedef struct SymbolTable          SymbolTable;

typedef Vector AstVector;


static inline AstVector *astVector_new_ptr(void) {
    return vector_new(sizeof(void *));
}

static inline int astVector_push_prt(AstVector *vec, void *ptr) {
    return vector_push_back(vec, &ptr);
}

static inline void *astVector_get_ptr(AstVector *vec, size_t idx) {
    void **slot = (void **)vector_get(vec, idx);
    return slot ? *slot : NULL;
}

static inline size_t astVector_size(AstVector *vec) {
    return vec ? vec->size : 0;
}


// ast_top_kinds
typedef enum AstKind {
    AST_DECL = 1,
    AST_STMT,
    AST_EXPR,
    AST_TYPE,
    AST_INIT,
} AstKind;

// commend_kinds
typedef enum TypeQualifier {
    TQ_NONE = 0,
    TQ_CONST = 1u << 0,
    TQ_VOLATILE = 1u << 1,
    TQ_RESTRICT = 1u << 2,
    TQ_ATOMIC = 1u << 3,
} TypeQualifier;

typedef enum StorageClass {
    SC_NONE = 0,
    SC_TYPEDEF,
    SC_EXTERN,
    SC_STATIC,
    SC_AUTO,
    SC_REGISTER,
} StorageClass;

typedef enum BuiltinType {
    BT_NONE = 0,
    BT_SHORT,
    BT_INT,
    BT_LONG,
    BT_LLONG,
    BT_USHORT,
    BT_UINT,
    BT_ULONG,
    BT_ULLONG,
    BT_FLOAT,
    BT_DOUBLE,
    BT_LONGDOUBLE,
    BT_BOOL,
    BT_CHAR,
} BuiltinType;

typedef enum TypeKind {
    TK_BUILTIN = 1,
    TK_POINTER,
    TK_ARRAY,
    TK_RECORD,
    TK_UNION,
    TK_ENUM,
    TK_TYPEDEF,
} TypeKind;

typedef enum RecordKind {
    RT_STRUCT = 1,
    RT_UNION,
} RecordKind;

struct AstType {
    AstKind  _kind;
    TypeKind kind;
    uint32_t qual;
    SrcRange location;

    union {
        struct { BuiltinType bt; } builtin;
        struct { AstType *base; } pointer;

        struct {
            AstType *elem;
            bool has_size;
            uint64_t size;
            bool is_vla;
            AstExpr *vla_expr;
        } array;

        struct {
            AstType *ret;
            AstVector *params;
            bool is_variadic;
        } function;

        struct {
            RecordKind rkind;
            Symbol *tag;
            AstVector *fields;
            bool is_defined;
        } record;

        struct {
            Symbol *tag;
            AstVector *items;
            bool is_defined;
        } enumty;

        struct {
            Symbol *sys;
        } typedef_reference;
    } as;
};

// symbols
typedef enum SymKind {
    SYM_VAR = 1,
    SYM_FUNC,
    SYM_TYPEDEF,
    SYM_TAG_STRUCT,
    SYM_TAG_UNION,
    SYM_TAG_ENUM,
    SYM_ENUM_CONST,
    SYM_LABEL,
} SymKind;

struct Symbol {
    const char *name;
    SymKind kind;
    SrcRange location;

    AstType *type;
    StorageClass sc;

    AstDecl *decl;
    Symbol *next;
};

SymbolTable *symTab_new(void);
void         symTab_free(SymbolTable *st);
void         symTab_enter(SymbolTable *st);
void         symTab_leave(SymbolTable *st);

Symbol      *symTab_define(SymbolTable*st, const char *name, SymKind kind,
                           AstType *type, StorageClass sc, SrcRange location);
Symbol      *symTab_lookup(SymbolTable *st, const char *name);
Symbol      *symTab_lookup_local(SymbolTable *st, const char *name);

static inline bool sym_is_typedef(const Symbol *sym) {
    return sym && sym->kind == SYM_TYPEDEF;
}

#endif //CCLONEDETECTOR_AST_H