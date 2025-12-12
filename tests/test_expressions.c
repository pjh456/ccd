#include <stdio.h>
#include <stdlib.h>
#include "parser_impl/expression.h"
#include "parser_impl/c_type_info.h"
#include "vector.h"

#define TEST(expr)                             \
    do                                         \
    {                                          \
        printf("\n--- Test: %s ---\n", #expr); \
        print_expression(expr);                \
        printf("\n");                          \
    } while (0)

// 为方便构造 vector
static Vector *make_args(Expression *a, Expression *b, Expression *c)
{
    Vector *v = vector_new(sizeof(Expression *));
    if (a)
        vector_push_back(v, &a);
    if (b)
        vector_push_back(v, &b);
    if (c)
        vector_push_back(v, &c);
    return v;
}

int main()
{
    printf("===== Expression Test Begin =====\n\n");

    // function: int func(int, double)
    Vector *params = vector_new(sizeof(Param *));
    vector_push_back(params, &(Param *){make_param_type("a", make_int_type(CTS_NONE, CTQ_NONE, CTM_SIGNED))});
    vector_push_back(params, &(Param *){make_param_type("b", make_double_type(CTS_NONE, CTQ_NONE, CTM_SIGNED))});
    CTypeInfo *func_t = make_function_type(make_int_type(CTS_NONE, CTQ_NONE, CTM_SIGNED), params, 0);
    c_type_info_free(func_t);

    // === 构造各种表达式 ===

    // 1. 字面量
    Expression *lit_int = make_expression_literal(make_int_type(CTS_NONE, CTQ_NONE, CTM_SIGNED), &(int){42});
    Expression *lit_char = make_expression_literal(make_char_type(CTS_NONE, CTQ_NONE, CTM_SIGNED), &(char){'x'});
    Expression *lit_float = make_expression_literal(make_float_type(CTS_NONE, CTQ_NONE), &(float){3.14f});
    Expression *lit_string = make_expression_literal(make_unknown(), "hello");

    // 2. 标识符
    Expression *id_x = make_expression_identifier("x");
    Expression *id_p = make_expression_identifier("ptr");
    Expression *id_f = make_expression_identifier("func");

    // 3. 括号
    Expression *paren = make_expression_paren(expression_copy(id_x));

    // 4. 类型转换
    Expression *cast_expr = make_expression_cast(make_double_type(CTS_NONE, CTQ_NONE, CTM_SIGNED), expression_copy(id_x));

    // 5. 一元表达式
    Expression *neg = make_expression_unary(OP_NEG, expression_copy(id_x), 1);      // -x
    Expression *bitnot = make_expression_unary(OP_TILDE, expression_copy(id_x), 1); // ~x
    Expression *pre_inc = make_expression_unary(OP_INC, expression_copy(id_x), 1);  // ++x
    Expression *post_dec = make_expression_unary(OP_DEC, expression_copy(id_x), 0); // x--

    // 6. 二元表达式
    Expression *add = make_expression_binary(OP_PLUS, expression_copy(lit_int), expression_copy(id_x));
    Expression *mul = make_expression_binary(OP_STAR, expression_copy(id_x), expression_copy(lit_int));
    Expression *lt = make_expression_binary(OP_LESS, expression_copy(id_x), expression_copy(lit_int));
    Expression *land = make_expression_binary(OP_AND_AND, expression_copy(id_x), expression_copy(lit_int));
    Expression *bitxor = make_expression_binary(OP_XOR, expression_copy(id_x), expression_copy(lit_int));

    // 7. 赋值表达式
    Expression *assign = make_expression_assign(OP_ASSIGN, expression_copy(id_x), expression_copy(lit_int));
    Expression *plus_eq = make_expression_assign(OP_PLUS_ASSIGN, expression_copy(id_x), expression_copy(lit_int));
    Expression *and_assign = make_expression_assign(OP_AND_ASSIGN, expression_copy(id_x), expression_copy(lit_int));

    // 8. 下标访问
    Expression *subscript = make_expression_subscript(expression_copy(id_p), expression_copy(lit_int));

    // 9. 成员访问
    Expression *member = make_expression_member(expression_copy(id_x), make_expression_identifier("field"));
    Expression *ptr_member = make_expression_ptr_member(expression_copy(id_p), make_expression_identifier("next"));

    // 10. sizeof
    Expression *sizeof_expr = make_expression_sizeof(expression_copy(id_x));

    // 11. 函数调用
    Vector *call_args = make_args(expression_copy(id_x), expression_copy(lit_int), expression_copy(lit_float));
    Expression *call_expr = make_expression_call(id_f, call_args);

    // 12. 条件运算符
    Expression *cond_expr = make_expression_conditional(expression_copy(id_x), expression_copy(lit_int), expression_copy(lit_float));

    // 13. 逗号表达式
    Vector *comma_list = make_args(expression_copy(id_x), expression_copy(lit_int), expression_copy(lit_float));
    Expression *comma_expr = make_expression_comma(comma_list);

    // === 全部打印测试 ===

    TEST(lit_int);
    TEST(lit_char);
    TEST(lit_float);
    TEST(lit_string);

    TEST(id_x);
    TEST(paren);
    TEST(cast_expr);

    TEST(neg);
    TEST(bitnot);
    TEST(pre_inc);
    TEST(post_dec);

    TEST(add);
    TEST(mul);
    TEST(lt);
    TEST(land);
    TEST(bitxor);

    TEST(assign);
    TEST(plus_eq);
    TEST(and_assign);

    TEST(subscript);
    TEST(member);
    TEST(ptr_member);

    TEST(sizeof_expr);
    TEST(call_expr);
    TEST(cond_expr);
    TEST(comma_expr);

    printf("\n===== Expression Test End =====\n");
    return 0;
}
