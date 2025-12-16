#include "decl_parser_impl/decl_specifier.h"
#include "decl_parser_impl/decl_specifier_impl/decl_specifier_impl.h"
#include "decl_parser_impl/decl_specifier_impl/sue_types.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

DeclSpecifier *make_decl_specifier(
    DeclBuiltinType builtin_type,
    DeclSUE sue_type,
    unsigned storages,
    unsigned func_specs,
    unsigned qualifiers,
    unsigned modifiers)
{
    DeclSpecifier *ds = calloc(1, sizeof(*ds));

    ds->builtin_type = builtin_type;
    ds->sue_type = sue_type;
    ds->storages = storages;
    ds->func_specifiers = func_specs;
    ds->qualifiers = qualifiers;
    ds->modifiers = modifiers;

    return ds;
}

void decl_specifier_free(DeclSpecifier *ds)
{
    if (!ds)
        return;
    switch (ds->sue_type)
    {
    case DSUE_STRUCT:
        decl_struct_type_free(ds->struct_type);
        return;
    case DSUE_UNION:
        decl_union_type_free(ds->union_type);
        return;
    case DSUE_ENUM:
        decl_enum_type_free(ds->enum_type);
        return;
    case DSUE_NONE:
    default:
        free(ds);
        return;
    }
}

const char *builtin_type_name(DeclBuiltinType t)
{
    switch (t)
    {
    case DBT_VOID:
        return "void";
    case DBT_CHAR:
        return "char";
    case DBT_INT:
        return "int";
    case DBT_FLOAT:
        return "float";
    case DBT_DOUBLE:
        return "double";
    default:
        return "<none>";
    }
}

void print_decl_storages(unsigned s)
{
    if (s & DS_EXTERN)
        printf(" extern");
    if (s & DS_STATIC)
        printf(" static");
    if (s & DS_REGISTER)
        printf(" register");
}

void print_decl_func_specifiers(unsigned fs)
{
    if (fs & DFS_INLINE)
        printf(" inline");
    if (fs & DFS_NORETURN)
        printf(" noreturn");
}

void print_decl_qualifiers(unsigned q)
{
    if (q & DTQ_CONST)
        printf(" const");
    if (q & DTQ_VOLATILE)
        printf(" volatile");
    if (q & DTQ_RESTRICT)
        printf(" restrict");
}

void print_decl_modifiers(unsigned mods)
{
    if (mods & DTM_UNSIGNED)
        printf(" unsigned");
    if (mods & DTM_SIGNED)
        printf(" signed");
    if (mods & DTM_SHORT)
        printf(" short");
    if (mods & DTM_LONGLONG)
        printf(" long long");
    else if (mods & DTM_LONG)
        printf(" long");
}

void print_decl_specifier(DeclSpecifier *ds)
{
    print_decl_specifier_impl(ds, 0);
}

void print_decl_specifier_impl(DeclSpecifier *ds, int indent)
{
    if (!ds)
    {
        print_indent(indent);
        printf("<null DeclSpecifier>\n");
        return;
    }

    print_indent(indent);
    printf("DeclSpecifier:\n");

    /* storage */
    if (ds->storages)
    {
        print_indent(indent + 2);
        printf("Storage:");
        print_decl_storages(ds->storages);
        printf("\n");
    }

    /* function specifiers */
    if (ds->func_specifiers)
    {
        print_indent(indent + 2);
        printf("Func-specifiers:");
        print_decl_func_specifiers(ds->func_specifiers);
        printf("\n");
    }

    /* qualifiers */
    if (ds->qualifiers)
    {
        print_indent(indent + 2);
        printf("Qualifiers:");
        print_decl_qualifiers(ds->qualifiers);
        printf("\n");
    }

    /* modifiers */
    if (ds->modifiers)
    {
        print_indent(indent + 2);
        printf("Modifiers:");
        print_decl_modifiers(ds->modifiers);
        printf("\n");
    }

    /* base type */
    print_indent(indent + 2);
    printf("Base type: ");

    if (ds->builtin_type != DBT_NONE)
    {
        printf("%s\n", builtin_type_name(ds->builtin_type));
    }
    else if (ds->sue_type != DSUE_NONE)
    {
        puts("");
        switch (ds->sue_type)
        {
        case DSUE_STRUCT:
            print_decl_struct_impl(ds->struct_type, indent + 2);
            break;
        case DSUE_UNION:
            print_decl_union_impl(ds->union_type, indent + 2);
            break;
        case DSUE_ENUM:
            print_decl_enum_impl(ds->enum_type, indent + 2);
            break;
        default:
            printf("<unknown SUE>\n");
        }
    }
    else
    {
        printf("<unspecified>\n");
    }
}
