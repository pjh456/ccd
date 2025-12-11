#pragma once

typedef struct Expression Expression;

void expression_unary_free(Expression *expr);
void expression_binary_free(Expression *expr);