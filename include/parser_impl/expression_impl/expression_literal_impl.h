#pragma once

typedef struct Expression Expression;

void build_signed_char_literal(Expression *expr, signed char value);
void build_unsigned_char_literal(Expression *expr, unsigned char value);

void build_signed_short_literal(Expression *expr, signed short value);
void build_unsigned_short_literal(Expression *expr, unsigned short value);

void build_signed_int_literal(Expression *expr, signed int value);
void build_unsigned_int_literal(Expression *expr, unsigned int value);

void build_signed_long_literal(Expression *expr, signed long value);
void build_unsigned_long_literal(Expression *expr, unsigned long value);

void build_signed_long_long_literal(Expression *expr, signed long long value);
void build_unsigned_long_long_literal(Expression *expr, unsigned long long value);

void build_float_literal(Expression *expr, float value);
void build_double_literal(Expression *expr, double value);
void build_long_double_literal(Expression *expr, long double value);

void build_string_literal(Expression *expr, char *value);

void expression_literal_free(Expression *expr);