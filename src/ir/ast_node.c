// #include "ir_impl/ast_node.h"

// #include <stdlib.h>
// #include <stdio.h>

// ASTNode *ast_node_new(NodeType nt, Token *t)
// {
//     ASTNode *node = calloc(1, sizeof(ASTNode));
//     node->type = nt;
//     node->token = t;
//     return node;
// }

// void free_node(ASTNode *node)
// {
//     if (!node)
//         return;

//     switch (node->type)
//     {
//     case AST_BLOCK:
//         if (node->block.stmts)
//         {
//             for (size_t i = 0; i < node->block.stmts->size; i++)
//             {
//                 free_node(*(ASTNode **)vector_get(node->block.stmts, i));
//             }
//             vector_free(node->block.stmts);
//         }
//         break;
//     case AST_IF:
//         free_node(node->if_stmt.cond);
//         free_node(node->if_stmt.then);
//         free_node(node->if_stmt.els);
//         break;
//     case AST_FOR:
//         free_node(node->for_stmt.init);
//         free_node(node->for_stmt.cond);
//         free_node(node->for_stmt.inc);
//         free_node(node->for_stmt.body);
//         break;
//     case AST_WHILE:
//     case AST_DO_WHILE:
//         free_node(node->loop.cond);
//         free_node(node->loop.body);
//         break;
//     case AST_RETURN:
//         // case AST_E:
//         free_node(node->expr.expr);
//         break;
//     case AST_VAR_DECL:
//         free_node(node->decl.init);
//         break;
//     case AST_NUMBER:
//     case AST_VAR:
//     case AST_BREAK:
//     case AST_CONTINUE:
//         break;
//     default: // 二元运算
//         if (node->bin.lhs)
//             free_node(node->bin.lhs);
//         if (node->bin.rhs)
//             free_node(node->bin.rhs);
//         break;
//     }
//     free(node);
// }

// void print_indent(int indent)
// {
//     for (int i = 0; i < indent; i++)
//         printf("  ");
// }

// void print_ast(ASTNode *node, int indent)
// {
//     if (!node)
//         return;
//     print_indent(indent);

//     switch (node->type)
//     {
//     case AST_BLOCK:
//         printf("BLOCK {\n");
//         for (size_t i = 0; i < node->block.stmts->size; i++)
//             print_ast(*(ASTNode **)vector_get(node->block.stmts, i), indent + 1);
//         print_indent(indent);
//         printf("}\n");
//         break;
//     case AST_RETURN:
//         printf("RETURN\n");
//         print_ast(node->expr.expr, indent + 1);
//         break;
//     case AST_IF:
//         printf("IF\n");
//         print_ast(node->if_stmt.cond, indent + 1);
//         print_indent(indent);
//         printf("THEN\n");
//         print_ast(node->if_stmt.then, indent + 1);
//         if (node->if_stmt.els)
//         {
//             print_indent(indent);
//             printf("ELSE\n");
//             print_ast(node->if_stmt.els, indent + 1);
//         }
//         break;
//     case AST_NUMBER:
//         printf("NUMBER %ld\n", node->val);
//         break;
//     case AST_VAR:
//         printf("VAR %.*s\n", (int)node->var.len, node->var.name);
//         break;
//     case AST_ADD:
//         printf("ADD\n");
//         goto bin_op;
//     case AST_SUB:
//         printf("SUB\n");
//         goto bin_op;
//     case AST_MUL:
//         printf("MUL\n");
//         goto bin_op;
//     case AST_DIV:
//         printf("DIV\n");
//         goto bin_op;
//     case AST_ASSIGN:
//         printf("ASSIGN\n");
//         goto bin_op;
//     case AST_LT:
//         printf("LT\n");
//         goto bin_op;
//     bin_op:
//         print_ast(node->bin.lhs, indent + 1);
//         print_ast(node->bin.rhs, indent + 1);
//         break;
//     default:
//         printf("ASTNode(%d)\n", node->type);
//         break;
//     }
// }