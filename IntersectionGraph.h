#ifndef INTGRAPH_H
#define INTGRAPH_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    LANE_INCOMING,
    LANE_OUTGOING
} LaneDirection;

typedef struct {
    uint32_t id;
    LaneDirection direction;
    int bearing; // 0-359 - the angle at which the lane enters the intersection
    float render_x; 
    float render_y;
} Lane;

typedef struct Connection {
    char* name;
    uint32_t id;
    Lane* from;
    Lane* to;
    
    struct Connection** conflicts;
    int conflict_count;
    
    bool is_active; 
} Connection;

typedef struct {
    Connection** active_connections;
    int connection_count;
    uint32_t duration_ms;
} Phase;

typedef struct {
    char* name;

    Lane* lanes;
    int lane_count;
    
    Connection* connections;
    int connection_count;
    
    Phase* phases;
    int phase_count;
    
    int current_phase_index;
} Intersection;



Intersection* create_intersection(const char* name);
void add_lane(Intersection* intr, uint32_t id, LaneDirection dir, int bearing);

void add_connection(Intersection* intr, const char* name, uint32_t from_id, uint32_t to_id);
Connection* find_connection_by_name(Intersection* intr, const char* name);
// A phase is just a collection of connections that are green
void add_phase(Intersection* intr, uint32_t* connection_ids, int count, uint32_t duration);
void add_phase_direct(Intersection* intr, Connection** conns, int count, uint32_t duration);

Lane* find_lane_by_id(Intersection* intr, uint32_t id);
void calculate_all_conflicts(Intersection* intr);
bool paths_cross(Connection* a, Connection* b);
void start_visualizer(Intersection* intr);
#endif