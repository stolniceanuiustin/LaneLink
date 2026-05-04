%option yylineno
%{
#include "y.tab.h"
#include "IntersectionGraph.h"
#include <string.h>
%}

%option noyywrap

%%
"intersection"  { return INTERSECTION; }
"lane"          { return LANE; }
"INCOMING"      { yylval.ival = LANE_INCOMING; return DIRECTION; }
"OUTGOING"      { yylval.ival = LANE_OUTGOING; return DIRECTION; }
"connect"       { return CONNECT; }
"as"            { return AS; }
"phase"         { return PHASE; }
"duration"      { return DURATION; }
"->"            { return ARROW; }

[0-9]+          { yylval.ival = atoi(yytext); return NUMBER; }

\"[^\"]*\"      { 
                  yylval.sval = strdup(yytext + 1);
                  yylval.sval[strlen(yylval.sval) - 1] = '\0';
                  return STRING; 
                }
"//".* ; /* Ignore single-line comments */
[ \t\n\r]       ; 
";"     { return ';'; }
"{"     { return '{'; }
"}"     { return '}'; }
","     { return ','; }
.               { printf("Unknown character: %s\n", yytext); }
%%