#include "parser_impl/c_type_info.h"
#include "parser_impl/c_type_info_impl.h"
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

CTypeInfo *make_unknown()
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    cti->type = CT_UNKNOWN;
    return cti;
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
        printf("%*s<null type>\n", indent, "");
        return;
    }

    const char *name = c_type_name(cti->type);

    printf("%*s%s", indent, "", name);

    switch (cti->type)
    {
    case CT_VOID:
    case CT_CHAR:
    case CT_INT:
    case CT_FLOAT:
    case CT_DOUBLE:
        printf(" (size=%zu, align=%zu)\n", cti->size, cti->align);
        break;
    case CT_POINTER:
        printf(" to\n");
        print_c_type_info_impl(cti->pointer.base, indent + 4);
        break;
    case CT_ARRAY:
        printf(" [%zu]\n", cti->array.length);
        printf("%*sof element:\n", indent + 2, "");
        print_c_type_info_impl(cti->array.base, indent + 4);
        printf("%*s(size=%zu, align=%zu)\n", indent + 2, "",
               cti->size, cti->align);
        break;
    case CT_FUNCTION:
        printf(" (function)\n");
        printf("%*sreturn:\n", indent + 2, "");
        print_c_type_info_impl(cti->func.return_type, indent + 4);
        printf("%*sparams:\n", indent + 2, "");
        if (!cti->func.params || cti->func.params->size == 0)
            printf("%*s<none>\n", indent + 4, "");
        else
        {
            for (size_t i = 0; i < cti->func.params->size; ++i)
            {
                Param *p = *((Param **)vector_get(cti->func.params, i));
                printf("%*s%s\n", indent + 4, "", (p->name ? p->name : "<anonymous>"));
                print_c_type_info_impl(p->type, indent + 6);
            }
        }
        if (cti->func.is_variadic)
            printf("%*s...\n", indent + 4, "");
        break;

    case CT_STRUCT:
        printf(" {\n");
        if (cti->record.is_complete)
        {
            Vector *fields = cti->record.fields;
            for (size_t i = 0; i < fields->size; ++i)
            {
                Field *f = *((Field **)vector_get(fields, i));
                printf("%*s%s @ offset=%zu\n", indent + 2, "", f->name, f->offset);
                print_c_type_info_impl(f->type, indent + 4);
            }
        }
        else
        {
            printf("%*s<incomplete>\n", indent + 2, "");
        }
        printf("%*s} (size=%zu, align=%zu)\n", indent, "",
               cti->size, cti->align);
        break;

    case CT_UNION:
        printf(" {\n");
        if (cti->record.is_complete)
        {
            Vector *fields = cti->record.fields;
            for (size_t i = 0; i < fields->size; ++i)
            {
                Field *f = *((Field **)vector_get(fields, i));
                printf("%*s%s @ offset=0\n", indent + 2, "", f->name);
                print_c_type_info_impl(f->type, indent + 4);
            }
        }
        else
            printf("%*s<incomplete>\n", indent + 2, "");

        printf("%*s} (size=%zu, align=%zu)\n", indent, "",
               cti->size, cti->align);
        break;

    case CT_ENUM:
        printf(" {\n");
        if (cti->enum_type.items)
        {
            Vector *items = cti->enum_type.items;
            for (size_t i = 0; i < items->size; ++i)
            {
                EnumItem *it = *((EnumItem **)vector_get(items, i));
                printf("%*s%s = %lld\n", indent + 2, "", it->name, it->value);
            }
        }
        else
            printf("%*s<incomplete>\n", indent + 2, "");

        printf("%*s} (size=%zu, align=%zu)\n", indent, "",
               cti->size, cti->align);
        break;

    default:
        printf(" <unknown type>\n");
        break;
    }
}
