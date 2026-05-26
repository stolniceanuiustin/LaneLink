#include <stdio.h>
#include <stdlib.h>
#include "IntersectionGraph.h"

extern int yyparse();
extern FILE* yyin;


Intersection* main_intr = NULL;

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Error: Please provide an input file.\n");
        printf("Usage: %s <filename.int>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input) {
        perror("Could not open file");
        return 1;
    }

    yyin = input;

    printf("Starting parser...\n");

    if (yyparse() == 0 && main_intr != NULL) {
        printf("\nSuccessfully parsed: %s\n", main_intr->name);
        printf("--------------------------------\n");
        printf("Lanes: %d\n", main_intr->lane_count);
        printf("Connections: %d\n", main_intr->connection_count);
        printf("Phases: %d\n", main_intr->phase_count);
        printf("--------------------------------\n");

        for(int i = 0; i < main_intr->connection_count; i++) {
            Connection* c = &main_intr->connections[i];
            printf("Connection '%s': From Lane %d -> To Lane %d\n", 
                    c->name, c->from->id, c->to->id);
        }
        
        calculate_all_conflicts(main_intr);
        export_intersection_to_json(main_intr, "intersection_data.json");

    } else {
        printf("Parsing failed.\n");
    }

    fclose(input);
    return 0;
}