#include <SDL2/SDL.h>
#include "IntersectionGraph.h"
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
void draw_intersection(SDL_Renderer* renderer, Intersection* intr) {
    int cx = SCREEN_WIDTH / 2;
    int cy = SCREEN_HEIGHT / 2;
    int box_size = 50;  // Size of the central square
    int road_len = 150; // Length of the roads
    int lane_off = 15;  // Shift lanes from the center of the road

    // 1. Draw central junction box
    SDL_Rect hub = {cx - box_size, cy - box_size, box_size * 2, box_size * 2};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &hub);

    // 2. Draw Road Arms (Only where lanes exist)
    SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
    for (int i = 0; i < intr->lane_count; i++) {
        float rad = (intr->lanes[i].bearing - 90) * (M_PI / 180.0); // -90 to make 0 North
        int rx = cx + cos(rad) * (box_size + road_len/2);
        int ry = cy + sin(rad) * (box_size + road_len/2);
        
        // Draw road arm as a thick line/rect
        SDL_Rect arm;
        if (abs((int)intr->lanes[i].bearing % 180) == 0) { // North/South
            arm = (SDL_Rect){cx - box_size, (intr->lanes[i].bearing == 0) ? cy - box_size - road_len : cy + box_size, box_size * 2, road_len};
        } else { // East/West
            arm = (SDL_Rect){(intr->lanes[i].bearing == 90) ? cx + box_size : cx - box_size - road_len, cy - box_size, road_len, box_size * 2};
        }
        SDL_RenderFillRect(renderer, &arm);
    }

    // 3. Draw Connections as "Flows"
    for (int i = 0; i < intr->connection_count; i++) {
        Connection* c = &intr->connections[i];

        // Adjust bearings so 0 is Up, 90 is Right, etc.
        float r1 = (c->from->bearing - 90) * (M_PI / 180.0);
        float r2 = (c->to->bearing - 90) * (M_PI / 180.0);

        // Start point (Edge of box, shifted for INCOMING)
        int x1 = cx + cos(r1) * box_size + cos(r1 + M_PI/2) * lane_off;
        int y1 = cy + sin(r1) * box_size + sin(r1 + M_PI/2) * lane_off;

        // End point (Edge of box, shifted for OUTGOING)
        int x2 = cx + cos(r2) * box_size + cos(r2 - M_PI/2) * lane_off;
        int y2 = cy + sin(r2) * box_size + sin(r2 - M_PI/2) * lane_off;

        // Set Color
        if (c->is_active) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        else SDL_SetRenderDrawColor(renderer, 180, 0, 0, 255);

        // Draw path with a "Kink" to make it look like a turn
        // Instead of going to (cx, cy), we go to a point slightly inside
        int mx = cx + cos(r1) * (box_size/2) + cos(r1 + M_PI/2) * lane_off;
        int my = cy + sin(r1) * (box_size/2) + sin(r1 + M_PI/2) * lane_off;
        
        int mx2 = cx + cos(r2) * (box_size/2) + cos(r2 - M_PI/2) * lane_off;
        int my2 = cy + sin(r2) * (box_size/2) + sin(r2 - M_PI/2) * lane_off;

        SDL_RenderDrawLine(renderer, x1, y1, mx, my);
        SDL_RenderDrawLine(renderer, mx, my, mx2, my2);
        SDL_RenderDrawLine(renderer, mx2, my2, x2, y2);
        
        // Draw "Car" at the start
        SDL_Rect car = {x1 - 4, y1 - 4, 8, 8};
        SDL_RenderFillRect(renderer, &car);
    }
}
void start_visualizer(Intersection* intr) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return;

    SDL_Window* window = SDL_CreateWindow("LaneLink Visualizer", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event e;
    uint32_t last_tick = SDL_GetTicks();

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }

        // --- PHASE LOGIC ---
        uint32_t current_tick = SDL_GetTicks();
        Phase* p = &intr->phases[intr->current_phase_index];

        // Reset all connections to inactive
        for(int i=0; i<intr->connection_count; i++) intr->connections[i].is_active = false;
        // Set active ones for this phase
        for(int i=0; i<p->connection_count; i++) p->active_connections[i]->is_active = true;

        if (current_tick - last_tick > p->duration_ms) {
            intr->current_phase_index = (intr->current_phase_index + 1) % intr->phase_count;
            last_tick = current_tick;
            printf("Switching to phase %d\n", intr->current_phase_index);
        }

        // --- RENDERING ---
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); // Dark background
        SDL_RenderClear(renderer);
        
        draw_intersection(renderer, intr);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}