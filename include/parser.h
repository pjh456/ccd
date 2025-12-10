#pragma once

#include "parser_impl/c_type_info.h"
#include "parser_impl/literal_value.h"
#include "parser_impl/expression.h"
#include "parser_impl/statement.h"

typedef struct Parser Parser;

Parser *parser_new(Token *token_stream);
Declaration **parse_translation_unit(Parser *p);