// #include "ir_impl/ir_impl.h"
// #include <stdlib.h>
// #include <string.h>
// #include <stdio.h>

// // --- Parser Context ---
// typedef struct
// {
//     Vector *tokens;
//     size_t pos;
// } Parser;

// // --- Helper Functions ---

// static Token *curr(Parser *p)
// {
//     if (p->pos >= p->tokens->size)
//         return NULL;
//     return (Token *)vector_get(p->tokens, p->pos);
// }

// // 检查当前 token 类型
// static int check(Parser *p, TokenType type)
// {
//     Token *t = curr(p);
//     return t && t->type == type;
// }

// // 消耗指定类型的 token，如果匹配失败则报错
// static Token *consume(Parser *p, TokenType type)
// {
//     Token *t = curr(p);
//     if (!t || t->type != type)
//     {
//         fprintf(stderr, "Parse Error: expected token type %s but got %d (line %d)\n",
//                 type, token_name(t ? t->type : T_UNKNOWN), t ? t->line : -1);
//         exit(1);
//     }
//     p->pos++;
//     return t;
// }

// // 如果匹配则消耗并返回真
// static int match(Parser *p, TokenType type)
// {
//     if (check(p, type))
//     {
//         p->pos++;
//         return 1;
//     }
//     return 0;
// }

// // --- Forward Declarations ---
// static ASTNode *expr(Parser *p);
// static ASTNode *stmt(Parser *p);
// static ASTNode *assign(Parser *p);

// // --- Expression Parser (优先级从低到高) ---

// // primary = "(" expr ")" | NUM | IDENTIFIER
// static ASTNode *primary(Parser *p)
// {
//     if (match(p, T_LEFT_PAREN))
//     {
//         ASTNode *node = expr(p);
//         consume(p, T_RIGHT_PAREN);
//         return node;
//     }

//     Token *tok = curr(p);
//     if (match(p, T_NUMBER))
//     {
//         // 简单处理：将字符串转为 long (实际上应该根据 Token 内容解析)
//         // 这里假设 token 原文里是合法的数字字符串
//         char buf[64];
//         int len = tok->length < 63 ? tok->length : 63;
//         memcpy(buf, tok->start, len);
//         buf[len] = '\0';
//         return new_number_nodeber_node(strtol(buf, NULL, 10), tok);
//     }

//     if (match(p, T_IDENTIFIER))
//     {
//         ASTNode *node = ast_node_new(AST_VAR, tok);
//         node->var.name = (char *)tok->start; // 注意：这里没做深拷贝，依赖 source 存在
//         node->var.len = tok->length;
//         return node;
//     }

//     fprintf(stderr, "Parse Error: expected expression at %d:%d\n", tok->line, tok->col);
//     exit(1);
// }

// // unary = ("+" | "-") unary | primary
// static ASTNode *unary(Parser *p)
// {
//     if (match(p, T_PLUS))
//         return unary(p); // +x -> x
//     if (match(p, T_MINUS))
//     {
//         Token *op = curr(p); // 获取刚才被 match 消耗的前一个 token 的位置其实需要技巧，这里简化
//         // 实际上 match 已经移走了 pos。为了简单，我们用 0 - x 表示 -x
//         return new_binary_node(AST_SUB, new_number_node(0, NULL), unary(p), NULL);
//     }
//     return primary(p);
// }

// // mul = unary ("*" unary | "/" unary | "%" unary)*
// static ASTNode *mul(Parser *p)
// {
//     ASTNode *node = unary(p);
//     for (;;)
//     {
//         Token *tok = curr(p);
//         if (match(p, T_STAR))
//             node = new_binary_node(AST_MUL, node, unary(p), tok);
//         else if (match(p, T_DIV))
//             node = new_binary_node(AST_DIV, node, unary(p), tok);
//         else if (match(p, T_MOD))
//             node = new_binary_node(AST_MOD, node, unary(p), tok);
//         else
//             return node;
//     }
// }

// // add = mul ("+" mul | "-" mul)*
// static ASTNode *add(Parser *p)
// {
//     ASTNode *node = mul(p);
//     for (;;)
//     {
//         Token *tok = curr(p);
//         if (match(p, T_PLUS))
//             node = new_binary_node(AST_ADD, node, mul(p), tok);
//         else if (match(p, T_MINUS))
//             node = new_binary_node(AST_SUB, node, mul(p), tok);
//         else
//             return node;
//     }
// }

// // relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// static ASTNode *relational(Parser *p)
// {
//     ASTNode *node = add(p);
//     for (;;)
//     {
//         Token *tok = curr(p);
//         if (match(p, T_LESS))
//             node = new_binary_node(AST_LT, node, add(p), tok);
//         else if (match(p, T_LESS_EQUAL))
//             node = new_binary_node(AST_LE, node, add(p), tok);
//         else if (match(p, T_GREATER))
//             node = new_binary_node(AST_GT, node, add(p), tok);
//         else if (match(p, T_GREATER_EQUAL))
//             node = new_binary_node(AST_GE, node, add(p), tok);
//         else
//             return node;
//     }
// }

// // equality = relational ("==" relational | "!=" relational)*
// static ASTNode *equality(Parser *p)
// {
//     ASTNode *node = relational(p);
//     for (;;)
//     {
//         Token *tok = curr(p);
//         if (match(p, T_EQUAL))
//             node = new_binary_node(AST_EQ, node, relational(p), tok);
//         else if (match(p, T_NOT_EQUAL))
//             node = new_binary_node(AST_NE, node, relational(p), tok);
//         else
//             return node;
//     }
// }

// // assign = equality ("=" assign)?
// static ASTNode *assign(Parser *p)
// {
//     ASTNode *node = equality(p);
//     if (match(p, T_ASSIGN))
//     {
//         Token *tok = curr(p);                                     // 这里的 tok 定位可能不准，简化处理
//         node = new_binary_node(AST_ASSIGN, node, assign(p), tok); // 右结合
//     }
//     return node;
// }

// static ASTNode *expr(Parser *p)
// {
//     return assign(p);
// }

// // --- Statement Parser ---

// // decl = type ident ("=" expr)? ";"
// // 简化：这里只处理 int，且假设只有 int 类型
// static ASTNode *declaration(Parser *p)
// {
//     Token *type_tok = consume(p, T_INT); // 暂时只支持 int
//     Token *ident = consume(p, T_IDENTIFIER);

//     ASTNode *node = ast_node_new(AST_VAR_DECL, ident);
//     node->decl.name = (char *)ident->start;
//     node->decl.len = ident->length;

//     if (match(p, T_ASSIGN))
//     {
//         node->decl.init = expr(p);
//     }
//     else
//     {
//         node->decl.init = NULL;
//     }
//     consume(p, T_SEMICOLON);
//     return node;
// }

// static ASTNode *stmt(Parser *p)
// {
//     Token *tok = curr(p);

//     // Block: { ... }
//     if (match(p, T_LEFT_BRACE))
//     {
//         ASTNode *node = ast_node_new(AST_BLOCK, tok);
//         node->block.stmts = vector_new(sizeof(ASTNode *));
//         while (!check(p, T_RIGHT_BRACE) && !check(p, T_EOF))
//         {
//             ASTNode *s = stmt(p);
//             vector_push_back(node->block.stmts, &s);
//         }
//         consume(p, T_RIGHT_BRACE);
//         return node;
//     }

//     // Return: return expr;
//     if (match(p, T_RETURN))
//     {
//         ASTNode *node = ast_node_new(AST_RETURN, tok);
//         node->expr.expr = expr(p);
//         consume(p, T_SEMICOLON);
//         return node;
//     }

//     // If: if (expr) stmt (else stmt)?
//     if (match(p, T_IF))
//     {
//         ASTNode *node = ast_node_new(AST_IF, tok);
//         consume(p, T_LEFT_PAREN);
//         node->if_stmt.cond = expr(p);
//         consume(p, T_RIGHT_PAREN);
//         node->if_stmt.then = stmt(p);
//         if (match(p, T_ELSE))
//         {
//             node->if_stmt.els = stmt(p);
//         }
//         else
//         {
//             node->if_stmt.els = NULL;
//         }
//         return node;
//     }

//     // While: while (expr) stmt
//     if (match(p, T_WHILE))
//     {
//         ASTNode *node = ast_node_new(AST_WHILE, tok);
//         consume(p, T_LEFT_PAREN);
//         node->loop.cond = expr(p);
//         consume(p, T_RIGHT_PAREN);
//         node->loop.body = stmt(p);
//         return node;
//     }

//     // For: for (init; cond; inc) stmt
//     if (match(p, T_FOR))
//     {
//         ASTNode *node = ast_node_new(AST_FOR, tok);
//         consume(p, T_LEFT_PAREN);

//         // Init
//         if (!match(p, T_SEMICOLON))
//         {
//             // 简单起见，这里假设 init 是表达式语句
//             // 如果要支持 for(int i=0;;) 需要这里调用 declaration()
//             if (check(p, T_INT))
//             {
//                 // 这是一个 Hack，为了演示把 decl 转为 init
//                 // 实际上 decl 返回的是 AST_VAR_DECL，需要存入 node->for_stmt.init
//                 node->for_stmt.init = declaration(p);
//                 // declaration 内部吃掉了分号，这里不用再吃了
//                 // 但逻辑有点乱，为了简单，For Init 这里暂时只解析 expr;
//             }
//             else
//             {
//                 node->for_stmt.init = ast_node_new(AST_EXPR_STMT, curr(p));
//                 node->for_stmt.init->expr.expr = expr(p);
//                 consume(p, T_SEMICOLON);
//             }
//         }
//         else
//         {
//             node->for_stmt.init = NULL;
//         }

//         // Cond
//         if (!match(p, T_SEMICOLON))
//         {
//             node->for_stmt.cond = expr(p);
//             consume(p, T_SEMICOLON);
//         }
//         else
//         {
//             node->for_stmt.cond = NULL;
//         }

//         // Inc
//         if (!match(p, T_RIGHT_PAREN))
//         {
//             node->for_stmt.inc = expr(p); // 这里应该是表达式，不带分号
//             consume(p, T_RIGHT_PAREN);
//         }
//         else
//         {
//             node->for_stmt.inc = NULL;
//         }

//         node->for_stmt.body = stmt(p);
//         return node;
//     }

//     // Declaration: int a = 1;
//     if (check(p, T_INT))
//     {
//         return declaration(p);
//     }

//     // ExprStmt: expr;
//     ASTNode *node = ast_node_new(AST_EXPR_STMT, tok);
//     node->expr.expr = expr(p);
//     consume(p, T_SEMICOLON);
//     return node;
// }

// // --- Entry Point ---

// Vector *parse(Vector *tokens)
// {
//     Parser p = {tokens, 0};
//     Vector *stmts = vector_new(sizeof(ASTNode *)); // 存储 ASTNode 指针

//     while (p.pos < tokens->size && !check(&p, T_EOF))
//     {
//         ASTNode *s = stmt(&p);
//         vector_push_back(stmts, &s);
//     }

//     return stmts;
// }