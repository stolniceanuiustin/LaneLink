#!/usr/bin/env python3
import json
import sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np

def visualize(json_path):
    with open(json_path, 'r') as f:
        data = json.load(f)

    fig, ax = plt.subplots(figsize=(10, 10))
    ax.set_aspect('equal')
    
    # Constants
    ROAD_WIDTH = 4.0
    LANE_WIDTH = 1.8
    CENTER_SIZE = 5.0
    ROAD_LENGTH = 15.0
    
    # 1. Draw Center Junction (Gray Square)
    center_rect = patches.Rectangle((-CENTER_SIZE, -CENTER_SIZE), 
                                   CENTER_SIZE*2, CENTER_SIZE*2, 
                                   color='#333333', zorder=1)
    ax.add_patch(center_rect)

    lane_positions = {} # Store (x, y) for connections

    # 2. Draw Road Arms and Lanes
    for lane in data['lanes']:
        lid = lane['id']
        bearing = lane['bearing'] # 0=North, 90=East...
        direction = lane['direction'] # 0=IN, 1=OUT
        
        # Convert bearing to math angle (0=East, CCW)
        math_angle = np.radians(90 - bearing)
        
        # Determine lane offset from road center
        # Simple heuristic: shift based on IN/OUT
        side_offset = -LANE_WIDTH if direction == 0 else LANE_WIDTH
        
        # Road arm vector
        dir_vec = np.array([np.cos(math_angle), np.sin(math_angle)])
        side_vec = np.array([np.cos(math_angle - np.pi/2), np.sin(math_angle - np.pi/2)])
        
        # Lane Start (at the edge of the center box)
        lane_start = dir_vec * CENTER_SIZE + side_vec * side_offset
        lane_end = dir_vec * (CENTER_SIZE + ROAD_LENGTH) + side_vec * side_offset
        
        # Draw Lane Line
        ax.plot([lane_start[0], lane_end[0]], [lane_start[1], lane_end[1]], 
                color='white', lw=2, ls='--', zorder=3)
        
        # Store position for connection arrows
        lane_positions[lid] = lane_start

        # Draw Road Background for this arm
        road_corner = dir_vec * CENTER_SIZE + side_vec * (side_offset * 2)
        # (This is simplified, just drawing a line for the "road" area)
        ax.fill([dir_vec[0]*CENTER_SIZE + side_vec[0]*ROAD_WIDTH, 
                 dir_vec[0]*(CENTER_SIZE+ROAD_LENGTH) + side_vec[0]*ROAD_WIDTH,
                 dir_vec[0]*(CENTER_SIZE+ROAD_LENGTH) - side_vec[0]*ROAD_WIDTH,
                 dir_vec[0]*CENTER_SIZE - side_vec[0]*ROAD_WIDTH],
                [dir_vec[1]*CENTER_SIZE + side_vec[1]*ROAD_WIDTH, 
                 dir_vec[1]*(CENTER_SIZE+ROAD_LENGTH) + side_vec[1]*ROAD_WIDTH,
                 dir_vec[1]*(CENTER_SIZE+ROAD_LENGTH) - side_vec[1]*ROAD_WIDTH,
                 dir_vec[1]*CENTER_SIZE - side_vec[1]*ROAD_WIDTH],
                color='#444444', zorder=2)
        
        # Label Lane
        ax.text(lane_end[0]*1.1, lane_end[1]*1.1, f"Lane {lid}", 
                color='black', fontweight='bold', ha='center')

    # 3. Draw Connections (Arrows)
    for conn in data['connections']:
        start = lane_positions[conn['from']]
        end = lane_positions[conn['to']]
        
        # Draw a curved arrow for the turn
        # Using FancyArrowPatch for better control
        arrow = patches.FancyArrowPatch(start, end, 
                                        connectionstyle="arc3,rad=.3", 
                                        arrowstyle='->', 
                                        mutation_scale=20, 
                                        color='green', 
                                        alpha=0.6,
                                        lw=2,
                                        label=conn['name'],
                                        zorder=4)
        ax.add_patch(arrow)

    plt.title(f"Intersection Layout: {data['name']}", fontsize=15)
    ax.set_xlim(-(CENTER_SIZE + ROAD_LENGTH + 5), (CENTER_SIZE + ROAD_LENGTH + 5))
    ax.set_ylim(-(CENTER_SIZE + ROAD_LENGTH + 5), (CENTER_SIZE + ROAD_LENGTH + 5))
    plt.axis('off')
    
    output_png = "intersection_visualization.png"
    plt.savefig(output_png, bbox_inches='tight', dpi=150)
    print(f"Realistic visualization saved to {output_png}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python visualize_intersection.py <data.json>")
    else:
        visualize(sys.argv[1])


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python visualize_intersection.py <data.json>")
    else:
        visualize(sys.argv[1])
