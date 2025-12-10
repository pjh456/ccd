#include "ir_impl/ast_node.h"

#include <stdlib.h>

ASTNode *ast_node_new(NodeType nt, Token *t)
{
    ASTNode *node = malloc(sizeof(*node));
    node->type = nt;
    node->token = t;
    return node;
}

void ast_node_free(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_BLOCK:
        if (node->block.stmts)
        {
            for (size_t i = 0; i < node->block.stmts->size; ++i)
            {
                ast_node_free((ASTNode *)vector_get(node->block.stmts, i));
            }
            vector_free(node->block.stmts);
        }
        break;
    case AST_EXPRESSION:
    case AST_VAL2PTR:
    case AST_PTR2VAL:
    case AST_NOT:
    case AST_TILDE:
    case AST_INC:
    case AST_DEC:
    case AST_RETURN:
        ast_node_free(node->expr.expr);
        break;
    case AST_AND:
    case AST_OR:
    case AST_XOR:
    case AST_LEFT_SHIFT:
    case AST_RIGHT_SHIFT:

    case AST_PLUS:
    case AST_MINUS:
    case AST_STAR:
    case AST_DIV:
    case AST_MOD:

    case AST_LESS:
    case AST_GREATER:
    case AST_EQUAL:
    case AST_NOT_EQUAL:
    case AST_LESS_EQUAL:
    case AST_GREATER_EQUAL:
    case AST_AND_AND:
    case AST_OR_OR:
        ast_node_free(node->bin.lhs);
        ast_node_free(node->bin.rhs);
        break;
    case AST_AND_ASSIGN:
    case AST_OR_ASSIGN:
    case AST_XOR_ASSIGN:
    case AST_LEFT_SHIFT_ASSIGN:
    case AST_RIGHT_SHIFT_ASSIGN:

    case AST_PLUS_ASSIGN:
    case AST_MINUS_ASSIGN:
    case AST_MUL_ASSIGN:
    case AST_DIV_ASSIGN:
    case AST_MOD_ASSIGN:
        ast_node_free(node->assign.var);
        ast_node_free(node->assign.expr);
        break;

    case AST_IF:
        ast_node_free(node->if_stmt.cond);
        ast_node_free(node->if_stmt.then);
        ast_node_free(node->if_stmt.els);
        break;
    case AST_FOR:
        ast_node_free(node->for_stmt.init);
        ast_node_free(node->for_stmt.cond);
        ast_node_free(node->for_stmt.inc);
        ast_node_free(node->for_stmt.body);
        break;
    case AST_WHILE:
        ast_node_free(node->while_stmt.cond);
        ast_node_free(node->while_stmt.body);
        break;
    case AST_DO_WHILE:
        ast_node_free(node->do_while_stmt.cond);
        ast_node_free(node->do_while_stmt.body);
        break;
    case AST_BREAK:
    case AST_CONTINUE:
        break;
    case AST_IDENTIFIER:
    case AST_NUMBER:
    case AST_CHARACTER:
    case AST_STRING:
        break;
    default:
        break;
    }
    free(node);
}

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