%{
/* C declarations */
#define YYSTYPE int
%}

/* bison declarations */
%token NUM VAR FUNCTION LOOP
%type exp
%right '='
%left '-' '+'
%left '*' '/'
%left NEG
%%
/* grammar rules */
program:
	| program line
;

line	  '\n'
	| statement '\n'
;

statement:	  FUNCTION exp { 
;

stmtlist:	statement
		| stmtlist statement
;

expr:	  NUM		{ $$ = $1; }
	| VAR
	| exp '+' exp	{ $$ = $1 + $3; }
	| exp '-' exp	{ $$ = $1 - $3; }
	| exp '*' exp	{ $$ = $1 * $3; }
	| exp '/' exp	{ $$ = $1 / $3; }
	| '-' exp %prec NEG	{ $$ = -$2; }
;
%%

/* additional C code */
