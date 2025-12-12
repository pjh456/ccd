#pragma once

typedef struct Expression Expression;

void expression_call_free(Expression *expr);
void expression_sizeof_expr_free(Expression *expr);
void expression_sizeof_type_free(Expression *expr);