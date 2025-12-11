#pragma once

typedef struct Expression Expression;

void expression_subscript_free(Expression *expr);
void expression_member_free(Expression *expr);
void expression_ptr_member_free(Expression *expr);
void expression_conditional_free(Expression *expr);
void expression_comma_free(Expression *expr);
void expression_paren_free(Expression *expr);