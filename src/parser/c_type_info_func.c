#include "parser_impl/c_type_info.h"
#include "parser_impl/c_type_info_impl.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>

CTypeInfo *make_function_type(CTypeInfo *ret, Vector *params, int is_var)
{
    CTypeInfo *cti = malloc(sizeof(*cti));
    memset(cti, 0, sizeof(*cti));

    cti->type = CT_FUNCTION;
    cti->func.return_type = ret;
    cti->func.params = params;
    cti->func.is_variadic = is_var;
    cti->size = cti->align = 0;
    return cti;
}

void c_function_type_info_free(CTypeInfo *cti)
{
    if (!cti || cti->type != CT_FUNCTION)
        return;
    c_type_info_free(cti->func.return_type);
    if (cti->func.params)
    {
        for (size_t idx = 0; idx < cti->func.params->size; ++idx)
            c_param_type_info_free(*((Param **)vector_get(cti->func.params, idx)));
        vector_free(cti->func.params);
    }
    free(cti);
}