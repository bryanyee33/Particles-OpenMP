#include <omp.h>

#include <cmath>
#include <fstream>
#include <vector>
#include <unordered_set>

#include "collision.h"
#include "io.h"
#include "sim_validator.h"

void print_particle(Particle &p) {
    printf("Particle %d: Loc: %f,%f, Vel: %f,%f\n", p.i, p.loc.x, p.loc.y, p.vel.x, p.vel.y);
}

inline void update_grid(int grid_width, int grid_count,
        std::vector<std::vector<std::unordered_set<int>>> &grid, std::vector<Particle> &particles) {

    for (auto vec : grid) {
        for (auto set : vec) {
            set.clear();
        }
    }

    int row, col;
    for (std::vector<Particle>::size_type i = 0; i < particles.size(); i++) {
        Particle p = particles[i];
        row = std::max(0, int(p.loc.y / grid_width));
        row = std::min(row, grid_count - 1);
        col = std::max(0, int(p.loc.x / grid_width));
        col = std::min(col, grid_count - 1);
        grid[row][col].insert(i);
    }
}

inline void check_and_resolve_particles(std::unordered_set<int> &set_to_check, std::vector<Particle> &particles, int particle_idx, int radius) {
    for (int i : set_to_check) {
        Particle &p = particles[particle_idx];
        Particle &o = particles[i];
        if (i != particle_idx && is_particle_collision(p.loc, p.vel, o.loc, o.vel, radius)) {
            resolve_particle_collision(p.loc, p.vel, o.loc, o.vel);
        }
    }
}

int main(int argc, char* argv[]) {
    // Read arguments and input file
    Params params{};
    std::vector<Particle> particles;
    read_args(argc, argv, params, particles);

    int grid_width = (params.param_radius << 2) + 1;
    int grid_count = std::ceil(params.square_size / grid_width);
    int possible_wall_collision_max;
    // If length of last box is smaller than radius, particles in the 2nd last box can still hit the wall.
    if (params.square_size % grid_width != 0 && params.square_size % grid_width < params.param_radius) {
        possible_wall_collision_max = grid_count - 2;
    } else {
        possible_wall_collision_max = grid_count - 1;
    }

    // Set number of threads
    omp_set_num_threads(params.param_threads);

#if CHECK == 1
    // Initialize collision checker
    SimulationValidator validator(params.param_particles, params.square_size, params.param_radius);
    // Initialize with starting positions
    validator.initialize(particles);
    // Uncomment the line below to enable visualization (makes program much slower)
    // validator.enable_viz_output("test.out");
#endif

    // TODO: this is the part where you simulate particle behavior.
    std::vector<std::vector<std::unordered_set<int>>> grid(grid_count,
            std::vector<std::unordered_set<int>>(grid_count, std::unordered_set<int>()));

    for (int i = 0; i < params.param_steps; i++) {
        // Position update
        for (Particle &p : particles) {
            p.loc.y += p.vel.y;
            p.loc.x += p.vel.x;
        }
        update_grid(grid_width, grid_count, grid, particles);
        //printf("\n\nNEXT ITERATION\n");
        // Velocity update
        for (int row = 0; row < grid_count; row++) {
            for (int col = 0; col < grid_count; col++) {
                for (int i : grid[row][col]) {
                    //printf("IN LOOP UPDATE\n");
                    //printf("Row: %d, Col: %d\n", row, col);
                    //print_particle(particles[i]);

                    // Find which quadrant of grid box the particle is in, and store the resulting indexes to check
                    int y_check_idx = row + ((std::fmod(particles[i].loc.y, grid_width) >= grid_width / 2.0) << 1) - 1; // -1 for down, +1 for up, added to row
                    int x_check_idx = col + ((std::fmod(particles[i].loc.x, grid_width) >= grid_width / 2.0) << 1) - 1; // -1 for left, +1 for right, added to col
                    bool y_check_idx_valid = 0 <= y_check_idx && y_check_idx < grid_count;
                    bool x_check_idx_valid = 0 <= x_check_idx && x_check_idx < grid_count;

                    // Check for collisions
                    // Same grid box
                    check_and_resolve_particles(grid[row][col], particles, i, params.param_radius);
                    
                    if (y_check_idx_valid) {
                        check_and_resolve_particles(grid[y_check_idx][col], particles, i, params.param_radius);
                    }
                    if (x_check_idx_valid) {
                        check_and_resolve_particles(grid[row][x_check_idx], particles, i, params.param_radius);
                    }
                    // Diagonally opposite grid box
                    if (y_check_idx_valid && x_check_idx_valid) {
                        check_and_resolve_particles(grid[y_check_idx][x_check_idx], particles, i, params.param_radius);
                    }
                    // If last (or 2nd last) row / col; or Out of bounds for at least 1 axis
                    if (row >= possible_wall_collision_max || col >= possible_wall_collision_max || !y_check_idx_valid || !x_check_idx_valid) {
                        // No point checking if collides, just resolve
                        resolve_wall_collision(particles[i].loc, particles[i].vel, params.square_size, params.param_radius);
                    }
                    
                    //printf("OUT LOOP UPDATE\n");
                    //print_particle(particles[i]);
                }
            }
        }

        #if CHECK == 1
            // Check final positions
            validator.validate_step(particles);
        #endif
    }
}
