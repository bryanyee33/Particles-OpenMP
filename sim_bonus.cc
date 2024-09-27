#include <omp.h>

#include <cmath>
#include <fstream>
#include <vector>

#include "collision.h"
#include "io.h"
#include "sim_validator.h"


inline void update_grid(int grid_box_width, int grid_box_row_count,
        std::vector<std::vector<std::vector<int>>> &grid, std::vector<Particle> &particles) {

    //#pragma omp parallel for shared(grid, grid_box_row_count) collapse(2) if(particles.size() >= 100000)
    for (int i = 0; i < grid_box_row_count; ++i) {
        for (int j = 0; j < grid_box_row_count; ++j) {
            grid[i][j].clear();
        }
    }

    // Parallelising is slower since particles.size() is too small,
    // and since updating grid needs critical section
    for (std::vector<Particle>::size_type i = 0; i < particles.size(); ++i) {
        int row = std::min(std::max(0, int(particles[i].loc.y / grid_box_width)), grid_box_row_count - 1);
        int col = std::min(std::max(0, int(particles[i].loc.x / grid_box_width)), grid_box_row_count - 1);

        grid[row][col].emplace_back(i);
    }
}

inline void add_overlaps(std::vector<int> &vec_to_check, std::vector<Particle> &particles, int idx,
        std::vector<std::vector<int>> &overlaps, int radius) {
    for (int i : vec_to_check) {
        Vec2 loc1 = particles[idx].loc;
        Vec2 loc2 = particles[i].loc;
        if (is_particle_overlap(loc2.x - loc1.x, loc2.y - loc1.y, radius)) {
            overlaps[idx].emplace_back(i);
        }
    }
}

inline bool check_and_resolve_particles_store_less_reverse(std::vector<int> &neighbours, std::vector<Particle> &particles,
        int idx, short wall_overlap, std::vector<int> &to_resolve, int square_size, int radius) {
    bool unresolved = false;

    for (auto i = neighbours.rbegin(); i != neighbours.rend(); ++i) {
        if (resolve_particle_collision(particles[idx], particles[*i])) {
            unresolved = true;
            if (*i < idx) {
                to_resolve.emplace_back(*i);
            }
        }
    }
    if (wall_overlap && is_wall_collision(particles[idx], square_size, radius)) {
        resolve_wall_collision(particles[idx], square_size, radius);
        unresolved = true;
    }
    return unresolved;
}

inline bool check_and_resolve_particles_store(std::vector<int> &neighbours, std::vector<Particle> &particles,
        int idx, short wall_overlap, std::vector<int> &to_resolve, int square_size, int radius) {
    bool unresolved = false;

    for (int i : neighbours) {
        if (resolve_particle_collision(particles[idx], particles[i])) {
            unresolved = true;
            to_resolve.emplace_back(i);
        }
    }
    if (wall_overlap && is_wall_collision(particles[idx], square_size, radius)) {
        resolve_wall_collision(particles[idx], square_size, radius);
        unresolved = true;
    }
    return unresolved;
}

inline bool check_and_resolve_particles_reverse(std::vector<int> &neighbours, std::vector<Particle> &particles, int idx,
        short wall_overlap, int square_size, int radius) {
    bool unresolved = false;

    for (auto i = neighbours.rbegin(); i != neighbours.rend(); ++i) {
        if (resolve_particle_collision(particles[idx], particles[*i])) {
            unresolved = true;
        }
    }
    if (wall_overlap && is_wall_collision(particles[idx], square_size, radius)) {
        resolve_wall_collision(particles[idx], square_size, radius);
        unresolved = true;
    }
    return unresolved;
}

int main(int argc, char* argv[]) {
    // Read arguments and input file
    Params params{};
    std::vector<Particle> particles;
    read_args(argc, argv, params, particles);

    int grid_box_width = (params.param_radius << 2) + 1; // Width of each small box in grid -> radius * 4 + 1
    int grid_box_row_count = std::ceil((float)params.square_size / grid_box_width); // No. of small boxes in a row
    int possible_wall_collision_max;
    // If length of last box is smaller than radius, particles in the 2nd last box can still hit the wall.
    if (params.square_size % grid_box_width != 0 && params.square_size % grid_box_width < params.param_radius) {
        possible_wall_collision_max = grid_box_row_count - 2;
    } else {
        possible_wall_collision_max = grid_box_row_count - 1;
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
    int reserve_size = params.param_particles / (grid_box_row_count * grid_box_row_count); // assume roughly equal amount per grid square

    std::vector<std::vector<std::vector<int>>> grid(grid_box_row_count,
            std::vector<std::vector<int>>(grid_box_row_count));
    
    #pragma omp parallel for shared(grid) schedule(static) collapse(2) //if (grid_count >= 37)
    for (int row = 0; row < grid_box_row_count; ++row) {
        for (int col = 0; col < grid_box_row_count; ++col) {
            grid[row][col].reserve(reserve_size);
        }
    }

    std::vector<std::vector<int>> overlaps(params.param_particles);
    std::vector<short> wall_overlaps(params.param_particles, 0); // boolean values: 0/1

    for (int n = 0; n < params.param_steps; ++n) {
        // Position update
        for (Particle &p : particles) {
            p.loc.y += p.vel.y;
            p.loc.x += p.vel.x;
        }
        update_grid(grid_box_width, grid_box_row_count, grid, particles);

        #pragma omp parallel for shared(overlaps) schedule(static) //if (grid_box_row_count >= 37)
        for (int i = 0; i < params.param_particles; ++i) {
            overlaps[i].clear();
            wall_overlaps[i] = 0;
        }


        // Find overlaps
        #pragma omp parallel for shared(grid, overlaps, wall_overlaps, particles) schedule(guided, 4) collapse(2)
        for (int row = 0; row < grid_box_row_count; ++row) {
            for (int col = 0; col < grid_box_row_count; ++col) {
                
                // Same grid box
                int box_len = (int)grid[row][col].size();
                for (int i = 0; i < box_len - 1; ++i) {
                    int p1 = grid[row][col][i];
                    for (int j = i + 1; j < box_len; ++j) {
                        int p2 = grid[row][col][j];
                        if (is_particle_overlap(particles[p2].loc.x - particles[p1].loc.x,
                                particles[p2].loc.y - particles[p1].loc.y, params.param_radius)) {
                            overlaps[p1].emplace_back(p2);
                            overlaps[p2].emplace_back(p1);
                            // free to resolve since both particles are in same box (each grid box 1 thread)
                            resolve_particle_collision(particles[p1], particles[p2]);
                        }
                    }
                }

                // free to resolve since all particles are in same box (each box 1 thread)
                // resolve as much as possible within box in parallel
                // TRACK ONLY CHANGED
                std::vector<int> to_resolve; // tracks what particles needs to be checked for possible collision
                for (int i : grid[row][col]) {
                    // resolve in reverse direction (biggest to smallest idx) to minimise redundant checks.
                    // store only indexes less than current one, since based on this for loop, we will be checking
                    // bigger indexes later already.
                    while (check_and_resolve_particles_store_less_reverse(overlaps[i], particles, i, 0,
                            to_resolve, params.square_size, params.param_radius));
                }
                while (!to_resolve.empty()) {
                    std::vector<int> to_resolve2; // stores the next iteration of particles to resolve
                    for (int i : to_resolve) {
                        while (check_and_resolve_particles_store(overlaps[i], particles, i, 0,
                                to_resolve2, params.square_size, params.param_radius));
                    }
                    to_resolve.swap(to_resolve2);
                }

                // Other grid boxes
                for (int i : grid[row][col]) {
                    /* // CHECK ALL 9 BOXES (FOR MEASUREMENT COMPARISON)
                    for (int x = -1; x < 2; ++x) {
                        for (int y = -1; y < 2; ++y) {
                            if (x == 0 && y == 0) { // already added above
                                continue;
                            }
                            bool y_check_idx_valid = 0 <= row + y && row + y < grid_box_row_count;
                            bool x_check_idx_valid = 0 <= col + x && col + x < grid_box_row_count;
                            if (x_check_idx_valid && y_check_idx_valid) {
                                add_overlaps(grid[row + y][col + x], particles, i, overlaps, params.param_radius);
                            }
                        }
                    }
                    if ((row >= possible_wall_collision_max || col >= possible_wall_collision_max || row == 0 || col == 0) &&
                            is_wall_overlap(particles[i].loc.x, particles[i].loc.y, params.square_size, params.param_radius)) {
                        wall_overlaps[i] = 1;
                    } */

                    // Find which quadrant of grid box the particle is in (resolve only neighbouring grid boxes in those directions),
                    // and check whether the neighbouring grid box found is a valid index

                    // -1 for down, +1 for up, added to row
                    int y_check_idx = row + ((std::fmod(particles[i].loc.y, grid_box_width) >= grid_box_width / 2.0) << 1) - 1;
                    // -1 for left, +1 for right, added to col
                    int x_check_idx = col + ((std::fmod(particles[i].loc.x, grid_box_width) >= grid_box_width / 2.0) << 1) - 1;

                    bool y_check_idx_valid = 0 <= y_check_idx && y_check_idx < grid_box_row_count;
                    bool x_check_idx_valid = 0 <= x_check_idx && x_check_idx < grid_box_row_count;
                    
                    if (x_check_idx_valid) {
                        add_overlaps(grid[row][x_check_idx], particles, i, overlaps, params.param_radius);
                    }
                    if (y_check_idx_valid) {
                        add_overlaps(grid[y_check_idx][col], particles, i, overlaps, params.param_radius);
                    }
                    
                    // Diagonally opposite grid box
                    if (y_check_idx_valid && x_check_idx_valid) {
                        add_overlaps(grid[y_check_idx][x_check_idx], particles, i, overlaps, params.param_radius);
                    }
                    // If last (or 2nd last) row / col; or Out of bounds for at least 1 axis
                    if ((row >= possible_wall_collision_max || col >= possible_wall_collision_max || !y_check_idx_valid || !x_check_idx_valid) &&
                            is_wall_overlap(particles[i].loc.x, particles[i].loc.y, params.square_size, params.param_radius)) {
                        //#pragma omp critical - Not needed as each thread writing to unique wall_overlaps[i]
                        wall_overlaps[i] = 1;
                    }
                }
            }
        }


        // Velocity update

        // PARALLEL ALTERNATE ROW RESOLVE
        bool unresolved = true;
        while (unresolved) {
            unresolved = false;
            // #pragma omp parallel for ordered shared(grid, overlaps, wall_overlaps, particles) schedule(dynamic) collapse(2) reduction(|| : unresolved)
            for (int start = 0; start < 2; ++start) {
                #pragma omp parallel for shared(start, grid, overlaps, wall_overlaps, particles) schedule(dynamic) reduction(|| : unresolved)
                for (int row = start; row < grid_box_row_count; row += 2) {
                    for (int col = 0; col < grid_box_row_count; ++col) {
                        // free to resolve without critical sections, since each particle cannot overlap with
                        // both top & bottom / left & right neighbouring boxes at the same time
                        for (int i : grid[row][col]) {
                            if (check_and_resolve_particles_reverse(overlaps[i], particles, i, wall_overlaps[i], params.square_size, params.param_radius)) {
                                while (check_and_resolve_particles_reverse(overlaps[i], particles, i, wall_overlaps[i], params.square_size, params.param_radius));
                                unresolved = true;
                            }
                        }
                    }
                }
            }
        }

        // CHECKERED PARALLEL RESOLVE
        // bool unresolved = true;
        // while (unresolved) {
        //     unresolved = false;
        //     for (int start = 0; start < 4; ++start) { // 00, 01, 10, 11
        //         #pragma omp parallel for shared(start, grid, overlaps, wall_overlaps, particles) schedule(guided, 5) collapse(2) reduction(|| : unresolved)
        //         for (int row = start >> 1; row < grid_box_row_count; row += 2) { // row_start = 0, 0, 1, 1
        //             for (int col = start & 1; col < grid_box_row_count; col += 2) { // col_start = 0, 1, 0, 1
        //                 // free to resolve without critical sections, since each particle cannot overlap with
        //                 // both top & bottom / left & right neighbouring boxes at the same time
        //                 for (int i : grid[row][col]) {
        //                     if (check_and_resolve_particles_reverse(overlaps[i], particles, i, wall_overlaps[i], params.square_size, params.param_radius)) {
        //                         while (check_and_resolve_particles_reverse(overlaps[i], particles, i, wall_overlaps[i], params.square_size, params.param_radius));
        //                         unresolved = true;
        //                     }
        //                 }
        //             }
        //         }
        //     }
        // }

        // TRACK ONLY CHANGED [Fastest sequential]
        // std::vector<int> to_resolve;
        // for (int i = 0; i < params.param_particles; ++i) {
        //     while (check_and_resolve_particles_store_less_reverse(overlaps[i], particles, i, wall_overlaps[i],
        //             to_resolve, params.square_size, params.param_radius));
        // }
        // while (!to_resolve.empty()) {
        //     std::vector<int> to_resolve2;
        //     for (int i : to_resolve) {
        //         while (check_and_resolve_particles_store(overlaps[i], particles, i, wall_overlaps[i],
        //                 to_resolve2, params.square_size, params.param_radius));
        //     }
        //     to_resolve.swap(to_resolve2);
        // }

        // ARBITRARY ORDER [Faster sequential for Small]
        // bool unresolved = true;
        // while (unresolved) {
        //     unresolved = false;
        //     for (int i = 0; i < params.param_particles; ++i) {
        //         unresolved = check_and_resolve_particles_reverse(overlaps[i], particles, i, wall_overlaps[i], params.square_size, params.param_radius) || unresolved;
        //     }
        // }

        // ARBITRARY REPEAT [Faster sequential for Medium & Large]
        // bool unresolved = true;
        // while (unresolved) {
        //     unresolved = false;
        //     for (int i = 0; i < params.param_particles; ++i) {
        //         if (check_and_resolve_particles_reverse(overlaps[i], particles, i, wall_overlaps[i], params.square_size, params.param_radius)) {
        //             while (check_and_resolve_particles_reverse(overlaps[i], particles, i, wall_overlaps[i], params.square_size, params.param_radius));
        //             unresolved = true;
        //         }
        //     }
        // }
        
        // ARBITRARY REPEAT PARALLEL [WRONG CONCEPT - DOESN'T WORK (resolving particle x & y independently would not result in same values)]
        // bool unresolved = true;
        // while (unresolved) {
        //     unresolved = false;
        //     std::vector<Particle> particles_read = particles;
        //     #pragma omp parallel for shared(overlaps, particles, particles_read) reduction(|| : unresolved)
        //     for (int i = 0; i < params.param_particles; ++i) {
        //         bool wall_overlap = false;
        //         std::vector<Particle> neighbours; // Store neighbouring particles for edit
        //         for (int j : overlaps[i]) {
        //             if (j != -1) {
        //                 neighbours.emplace_back(particles_read[j]); // unedited particles
        //             } else {
        //                 wall_overlap = true;
        //             }
        //         }
        //         if (check_and_resolve_particles(particles[i], neighbours, wall_overlap, params.square_size, params.param_radius)) {
        //             while (check_and_resolve_particles(particles[i], neighbours, wall_overlap, params.square_size, params.param_radius));
        //             unresolved = true;
        //         }
        //     }
        // }

        // RESOLVE BY CLUSTER [Slowest, but took the longest time to make :( - too much overhead]
        // std::unordered_set<int> visited; // contains elements of all resolved clusters
        // std::unordered_set<int> cluster_visited; // contains elements only within current cluster
        // std::stack<int> stack; // DFS stack
        // for (int i = 0; i < params.param_particles; ++i) {
        //     if (visited.find(i) == visited.end()) {
        //         // cluster not yet visited
        //         stack.push(i);
        //         // inside cluster
        //         while (!stack.empty()) {
        //             int curr = stack.top();
        //             stack.pop();
        //             if (cluster_visited.find(curr) == cluster_visited.end()) {
        //                 // cluster element not yet visited
        //                 if (check_and_resolve_particles(overlaps[curr], particles, curr, params.square_size, params.param_radius)) {
        //                     // have unresolved; redo DFS starting from this element
        //                     cluster_visited = std::unordered_set<int>();
        //                     stack = std::stack<int>();      
        //                     // keep resolving until no more
        //                     while (check_and_resolve_particles(overlaps[curr], particles, curr, params.square_size, params.param_radius));
        //                 }
        //                 // DFS
        //                 cluster_visited.insert(curr);
        //                 for (int neighbour : overlaps[curr]) {
        //                     if (neighbour != -1) {
        //                         stack.push(neighbour);
        //                     }
        //                 }
        //             }
        //         }
        //         visited.merge(cluster_visited); // combine with overall visited set
        //         cluster_visited = std::unordered_set<int>();
        //     }
        // }

        // RESOLVE BY CLUSTER PARALLEL [STILL SLOW - too much overhead at small, clusters too huge at large]
        // std::vector<std::vector<int>> clusters;
        // std::unordered_set<int> visited;
        // std::stack<int> stack; // DFS stack
        // // Store clusters
        // for (int i = 0; i < params.param_particles; ++i) {
        //     if (visited.find(i) == visited.end()) {
        //         // cluster not yet visited
        //         stack.emplace(i);
        //         clusters.emplace_back();
        //         // inside cluster
        //         while (!stack.empty()) {
        //             int curr = stack.top();
        //             stack.pop();
        //             if (visited.find(curr) == visited.end()) {
        //                 visited.insert(curr);
        //                 clusters.back().emplace_back(curr);
        //                 for (int neighbour : overlaps[curr]) {
        //                     if (neighbour != -1) {
        //                         stack.emplace(neighbour);
        //                     }
        //                 }
        //             }
        //         }
        //     }
        // }
        // bool unresolved = true;
        // while (unresolved) {
        //     unresolved = false;
        //     #pragma omp parallel for shared(overlaps, particles) schedule(guided) reduction(|| : unresolved)
        //     for (std::vector<int> cluster : clusters) {
        //         for (int i : cluster) {
        //             if (check_and_resolve_particles(overlaps[i], particles, i, params.square_size, params.param_radius)) {
        //                 while (check_and_resolve_particles(overlaps[i], particles, i, params.square_size, params.param_radius));
        //                 unresolved = true;
        //             }
        //         }
        //     }
        // }

        #if CHECK == 1
            // Check final positions
            validator.validate_step(particles);
        #endif
    }
}
