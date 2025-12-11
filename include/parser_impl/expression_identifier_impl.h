#pragma once

typedef struct Expression Expression;

void expression_identifier_free(Expression *expr);
void expression_cast_free(Expression *expr);
void expression_assign_free(Expression *expr);