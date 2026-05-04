#include "IntersectionGraph.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

/*
Implementation of intersection graph API
*/
Intersection* create_intersection(const char* name) {
    Intersection* intr = (Intersection*)malloc(sizeof(Intersection));
    intr->name = strdup(name);
    intr->lanes = NULL;
    intr->lane_count = 0;
    intr->connections = NULL;
    intr->connection_count = 0;
    intr->phases = NULL;
    intr->phase_count = 0;
    intr->current_phase_index = 0;
    return intr;
}

void add_lane(Intersection* intr, uint32_t id, LaneDirection dir, int bearing) {
    intr->lane_count++;
    intr->lanes = (Lane*)realloc(intr->lanes, sizeof(Lane) * intr->lane_count);
    
    Lane* l = &intr->lanes[intr->lane_count - 1];
    l->id = id;
    l->direction = dir;
    l->bearing = bearing;
    
    // UI logic: Pre-calculate coordinates for SDL based on bearing
    // Center is 0,0 for now; SDL will offset it.
    float rad = bearing * (3.14159 / 180.0);
    l->render_x = cos(rad) * 200.0; 
    l->render_y = sin(rad) * 200.0;
}

Lane* find_lane_by_id(Intersection* intr, uint32_t id) {
    for (int i = 0; i < intr->lane_count; i++) {
        if (intr->lanes[i].id == id) return &intr->lanes[i];
    }
    return NULL;
}

void add_connection(Intersection* intr, const char* name, uint32_t from_id, uint32_t to_id) {
    Lane* src = find_lane_by_id(intr, from_id);
    Lane* tgt = find_lane_by_id(intr, to_id);
    
    if (!src || !tgt) return;

    intr->connection_count++;
    intr->connections = (Connection*)realloc(intr->connections, sizeof(Connection) * intr->connection_count);
    
    Connection* c = &intr->connections[intr->connection_count - 1];
    c->name = strdup(name);
    c->id = intr->connection_count - 1; // Auto-index
    c->from = src;
    c->to = tgt;
    c->conflicts = NULL;
    c->conflict_count = 0;
    c->is_active = false;
}

Connection* find_connection_by_name(Intersection* intr, const char* name) {
    for (int i = 0; i < intr->connection_count; i++) {
        if (strcmp(intr->connections[i].name, name) == 0) {
            return &intr->connections[i];
        }
    }
    return NULL;
}

void add_phase_direct(Intersection* intr, Connection** conns, int count, uint32_t duration) {
    intr->phase_count++;
    intr->phases = realloc(intr->phases, sizeof(Phase) * intr->phase_count);
    
    Phase* p = &intr->phases[intr->phase_count - 1];
    p->active_connections = conns;
    p->connection_count = count;
    p->duration_ms = duration;
}

// Helper to check if two connections physically intersect
bool paths_cross(Connection* a, Connection* b) {
    // We don't conflict if we share a starting lane (traffic splits)
    if (a->from == b->from || a->to == b->to) return false;

    int a1 = a->from->bearing;
    int a2 = a->to->bearing;
    int b1 = b->from->bearing;
    int b2 = b->to->bearing;

    // Standardize angles to 0-359
    // A path crosses another if the start/end points alternate in circular order
    // We check if b1 or b2 lies "between" a1 and a2
    bool b1_between = false;
    if (a1 < a2) b1_between = (b1 > a1 && b1 < a2);
    else b1_between = (b1 > a1 || b1 < a2);

    bool b2_between = false;
    if (a1 < a2) b2_between = (b2 > a1 && b2 < a2);
    else b2_between = (b2 > a1 || b2 < a2);

    // If one is between and the other isn't, they cross!
    return (b1_between != b2_between);
}

void calculate_all_conflicts(Intersection* intr) {
    printf("Computing geometric conflicts\n");
    for (int i = 0; i < intr->connection_count; i++) {
        for (int j = i + 1; j < intr->connection_count; j++) {
            Connection* a = &intr->connections[i];
            Connection* b = &intr->connections[j];

            if (paths_cross(a, b)) {
                // Record conflict in A
                a->conflict_count++;
                a->conflicts = realloc(a->conflicts, sizeof(Connection*) * a->conflict_count);
                a->conflicts[a->conflict_count - 1] = b;

                // Record conflict in B
                b->conflict_count++;
                b->conflicts = realloc(b->conflicts, sizeof(Connection*) * b->conflict_count);
                b->conflicts[b->conflict_count - 1] = a;
                
                printf("  [!] Conflict detected: '%s' <-> '%s'\n", a->name, b->name);
            }
        }
    }
}