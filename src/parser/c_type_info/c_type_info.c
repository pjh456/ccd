#include "parser_impl/c_type_info.h"
#include "parser_impl/c_type_info_impl/c_type_info_impl.h"
#include "vector.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

CTypeInfo *make_unknown()
{
    CTypeInfo *cti = calloc(1, sizeof(*cti));
    cti->type = CT_UNKNOWN;
    return cti;
}

CTypeInfo *c_type_info_copy(CTypeInfo *cti)
{
    if (!cti)
        return cti;
    CTypeInfo *copied_cti = calloc(1, sizeof(*cti));
    return copied_cti;
}

void c_type_info_free(CTypeInfo *cti)
{
    if (!cti)
        return;

    switch (cti->type)
    {
    case CT_VOID:
        c_void_type_info_free(cti);
        break;
    case CT_CHAR:
        c_char_type_info_free(cti);
        break;
    case CT_INT:
        c_int_type_info_free(cti);
        break;
    case CT_FLOAT:
        c_float_type_info_free(cti);
        break;
    case CT_DOUBLE:
        c_double_type_info_free(cti);
        break;
    case CT_POINTER:
        c_pointer_type_info_free(cti);
        break;
    case CT_ARRAY:
        c_array_type_info_free(cti);
        break;
    case CT_FUNCTION:
        c_function_type_info_free(cti);
        break;
    case CT_STRUCT:
        c_struct_type_info_free(cti);
        break;
    case CT_UNION:
        c_union_type_info_free(cti);
        break;
    case CT_ENUM:
        c_enum_type_info_free(cti);
        break;
    case CT_UNKNOWN:
    default:
        free(cti);
    }
}

void print_c_type_info_storages(unsigned s)
{
    if (s & CTS_EXTERN)
        printf(" extern");
    if (s & CTS_STATIC)
        printf(" static");
    if (s & CTS_REGISTER)
        printf(" register");
}

void print_c_type_info_func_specifiers(unsigned fs)
{
    if (fs & FS_INLINE)
        printf(" inline");
    if (fs & FS_NORETURN)
        printf(" noreturn");
}

void print_c_type_info_qualifiers(unsigned q)
{
    if (q & CTQ_CONST)
        printf(" const");
    if (q & CTQ_VOLATILE)
        printf(" volatile");
    if (q & CTQ_RETRICT)
        printf(" restrict");
}

void print_c_type_info_modifiers(unsigned mods)
{
    if (mods & CTM_UNSIGNED)
        printf(" unsigned");
    if (mods & CTM_SIGNED)
        printf(" signed");
    if (mods & CTM_SHORT)
        printf(" short");
    if (mods & CTM_LONG)
        printf(" long");
    if (mods & CTM_LONGLONG)
        printf(" long long");
}

char *c_type_name(CType ct)
{
    switch (ct)
    {
    case CT_VOID:
        return "VOID";
    case CT_CHAR:
        return "CHAR";
    case CT_INT:
        return "INT";
    case CT_FLOAT:
        return "FLOAT";
    case CT_DOUBLE:
        return "DOUBLE";
    case CT_POINTER:
        return "POINTER";
    case CT_ARRAY:
        return "ARRAY";
    case CT_FUNCTION:
        return "FUNCTION";
    case CT_STRUCT:
        return "STRUCT";
    case CT_UNION:
        return "UNION";
    case CT_ENUM:
        return "ENUM";
    case CT_UNKNOWN:
    default:
        return "UNKNOWN";
    }
}

void print_c_type_info(CTypeInfo *cti)
{
    print_c_type_info_impl(cti, 0);
}

void print_c_type_info_impl(CTypeInfo *cti, int indent)
{
    if (!cti)
    {
        print_indent(indent);
        printf("<null type>\n");
        return;
    }

    print_indent(indent);
    printf("Type: %s\n", c_type_name(cti->type));

    print_indent(indent);
    printf("Size: %zu, Align: %zu\n", cti->size, cti->align);

    if (cti->storages)
    {
        print_indent(indent);
        printf("Storage:");
        print_c_type_info_storages(cti->storages);
        printf("\n");
    }
    if (cti->qualifiers)
    {
        print_indent(indent);
        printf("Qualifiers:");
        print_c_type_info_qualifiers(cti->qualifiers);
        printf("\n");
    }
    if (cti->modifiers)
    {
        print_indent(indent);
        printf("Modifiers:");
        print_c_type_info_modifiers(cti->modifiers);
        printf("\n");
    }
    if (cti->func_specifiers)
    {
        print_indent(indent);
        printf("Func-specifiers:");
        print_c_type_info_func_specifiers(cti->func_specifiers);
        printf("\n");
    }

    switch (cti->type)
    {
    case CT_POINTER:
        print_indent(indent);
        printf("Pointer to:\n");
        print_c_type_info_impl(cti->pointer.base, indent + 4);
        break;

    case CT_ARRAY:
        print_indent(indent);
        printf("Array length: %zu\n", cti->array.length);
        print_indent(indent);
        printf("Element type:\n");
        print_c_type_info_impl(cti->array.base, indent + 4);
        break;

    case CT_FUNCTION:
    {
        print_indent(indent);
        printf("Function:\n");

        print_indent(indent + 4);
        printf("Return type:\n");
        print_c_type_info_impl(cti->func.return_type, indent + 8);

        print_indent(indent + 4);
        printf("Parameters:\n");

        if (!cti->func.params || cti->func.params->size == 0)
        {
            print_indent(indent + 8);
            printf("(none)\n");
        }
        else
        {
            for (size_t i = 0; i < cti->func.params->size; i++)
            {
                Param *p = *((Param **)vector_get(cti->func.params, i));
                print_indent(indent + 8);
                printf("Param %zu: %s\n", i, p->name ? p->name : "<anonymous>");
                print_c_type_info_impl(p->type, indent + 12);
            }
        }

        if (cti->func.is_variadic)
        {
            print_indent(indent + 4);
            printf("Variadic: yes\n");
        }
        break;
    }

    case CT_STRUCT:
    case CT_UNION:
    {
        print_indent(indent);
        printf("Record (%s), complete=%d\n",
               cti->type == CT_STRUCT ? "struct" : "union",
               cti->record.is_complete);

        if (!cti->record.fields)
        {
            print_indent(indent + 4);
            printf("(no fields)\n");
            break;
        }

        for (size_t i = 0; i < cti->record.fields->size; i++)
        {
            Field *f = *((Field **)vector_get(cti->record.fields, i));

            print_indent(indent + 4);
            printf("Field %zu: %s, offset=%zu\n",
                   i, f->name ? f->name : "<anonymous>", f->offset);

            print_c_type_info_impl(f->type, indent + 8);
        }
        break;
    }

    case CT_ENUM:
    {
        print_indent(indent);
        printf("Enum, complete=%d\n", cti->enum_type.is_complete);

        if (!cti->enum_type.items)
        {
            print_indent(indent + 4);
            printf("(no enumerators)\n");
            break;
        }

        for (size_t i = 0; i < cti->enum_type.items->size; i++)
        {
            EnumItem *it = *((EnumItem **)vector_get(cti->enum_type.items, i));
            print_indent(indent + 4);
            printf("%s = %lld\n", it->name, it->value);
        }
        break;
    }

    default:
        break;
    }
}