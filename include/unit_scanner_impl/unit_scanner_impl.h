#pragma once

typedef struct StatementUnit StatementUnit;
typedef struct UnitScanner UnitScanner;

StatementUnit *scan_empty(UnitScanner *us);
StatementUnit *scan_compound(UnitScanner *us);

StatementUnit *scan_declaration(UnitScanner *us);
StatementUnit *scan_expression(UnitScanner *us);

StatementUnit *scan_if(UnitScanner *us);
StatementUnit *scan_switch(UnitScanner *us);
StatementUnit *scan_case(UnitScanner *us);
StatementUnit *scan_default(UnitScanner *us);

StatementUnit *scan_while(UnitScanner *us);
StatementUnit *scan_do_while(UnitScanner *us);
StatementUnit *scan_for(UnitScanner *us);

StatementUnit *scan_continue(UnitScanner *us);
StatementUnit *scan_break(UnitScanner *us);
StatementUnit *scan_return(UnitScanner *us);

StatementUnit *scan_label(UnitScanner *us);
StatementUnit *scan_goto(UnitScanner *us);