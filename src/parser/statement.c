// #include "parser_impl/statement.h"
// #include "parser_impl/declaration.h"
// #include "parser_impl/expression.h"

// Statement *statement_new(StatementType st)
// {

// }

// void statement_free(Statement *stmt)
// {
//     if (!stmt)
//         return;

//     switch (stmt->type)
//     {
//     case STMT_COMPOUND:
//         if (stmt->compound.stmts)
//         {
//             for (size_t idx = 0; idx < stmt->compound.stmts->size; ++idx)
//             {
//                 statement_free(
//                     (Statement *)vector_get(stmt->compound.stmts, idx));
//             }
//         }
//         vector_free(stmt->compound.stmts);
//         break;
//     case STMT_EXPR:

//         break;
//     case STMT_DECL:
//         if (stmt->decl.decls)
//         {
//             for (size_t idx = 0; idx < stmt->decl.decls->size; ++idx)
//             {
//             }
//         }
//         vector_free(stmt->decl.decls);
//         break;
//     case STMT_IF:
//         // expression
//         statement_free(stmt->if_stmt.then_branch);
//         statement_free(stmt->if_stmt.else_branch);
//         break;
//     }
// }