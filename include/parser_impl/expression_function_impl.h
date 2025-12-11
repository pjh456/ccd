#pragma once

typedef struct Expression Expression;

void expression_call_free(Expression *expr);
void expression_sizeof_free(Expression *expr);