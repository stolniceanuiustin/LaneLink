%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IntersectionGraph.h"

extern Intersection* main_intr;
void yyerror(const char *s);
int yylex();
extern int yylineno;
// Temporary storage for phase names
char* temp_names[32];
int temp_name_count = 0;
%}

%union {
    int ival;
    char* sval;
}

%token <ival> NUMBER DIRECTION
%token <sval> STRING
%token INTERSECTION LANE CONNECT AS PHASE DURATION ARROW

%%

input:
    INTERSECTION STRING '{' { main_intr = create_intersection($2); } elements '}'
    ;

elements:
    element elements | /* empty */ ;

element:
    lane_def | conn_def | phase_def ;

lane_def:
    LANE NUMBER DIRECTION NUMBER ';' {
        add_lane(main_intr, $2, $3, $4);
    };

conn_def:
    CONNECT NUMBER ARROW NUMBER AS STRING ';' {
        add_connection(main_intr, $6, $2, $4);
    };

phase_def:
    PHASE '{' name_list '}' DURATION NUMBER ';' {
        Connection** phase_conns = malloc(sizeof(Connection*) * temp_name_count);
        for(int i = 0; i < temp_name_count; i++) {
            Connection* c = find_connection_by_name(main_intr, temp_names[i]);
            if(c) {
                phase_conns[i] = c;
            } else {
                printf("Error: Connection '%s' not found!\n", temp_names[i]);
            }
            free(temp_names[i]); 
        }
        
        add_phase_direct(main_intr, phase_conns, temp_name_count, $6);
        temp_name_count = 0;
    };

name_list:
    STRING { temp_names[temp_name_count++] = strdup($1); }
    | STRING ',' name_list { temp_names[temp_name_count++] = strdup($1); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Line %d: Parse error near '%s'\n", yylineno, s);
}
