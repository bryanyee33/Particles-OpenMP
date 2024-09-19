#include <omp.h>

#include <cmath>
#include <fstream>
#include <vector>
// #include <unordered_set>
// #include <stack>

#include "collision.h"
#include "io.h"
#include "sim_validator.h"


inline void update_grid(int grid_box_width, int grid_box_row_count, int grid_box_count,
        std::vector<std::vector<int>> &grid, std::vector<Particle> &particles) {

    //#pragma omp parallel for shared(grid, grid_box_row_count) if(particles.size() >= 100000)
    for (int i = 0; i < grid_box_count; ++i) {
        grid[i].clear();
    }

    // Parallelising is slower since particles.size() is too small,
    // and since updating grid needs critical section
    for (std::vector<Particle>::size_type i = 0; i < particles.size(); ++i) {
        int row = std::min(std::max(0, int(particles[i].loc.y / grid_box_width)), grid_box_row_count - 1);
        int col = std::min(std::max(0, int(particles[i].loc.x / grid_box_width)), grid_box_row_count - 1);
    
        grid[row * grid_box_row_count + col].emplace_back(i);
    }
}

inline void add_overlaps(std::vector<int> &vec_to_check, std::vector<Particle> &particles, int idx, std::vector<std::vector<int>> &overlaps, int radius) {
    for (int i : vec_to_check) {
        Vec2 loc1 = particles[idx].loc;
        Vec2 loc2 = particles[i].loc;
        if (idx != i && is_particle_overlap(loc2.x - loc1.x, loc2.y - loc1.y, radius)) {
            overlaps[idx].emplace_back(i);
        }
    }
}

inline bool check_and_resolve_particles_store_less_reverse(std::vector<int> &neighbours, std::vector<Particle> &particles, int idx, int wall_overlap,
        std::vector<int> &to_resolve, int square_size, int radius) {
    bool unresolved = false;

    for (auto i = neighbours.rbegin(); i != neighbours.rend(); ++i) {
        if (is_particle_moving_closer(particles[idx], particles[*i])) {
            resolve_particle_collision(particles[idx], particles[*i]);
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

inline bool check_and_resolve_particles_store(std::vector<int> &neighbours, std::vector<Particle> &particles, int idx, int wall_overlap,
        std::vector<int> &to_resolve, int square_size, int radius) {
    bool unresolved = false;

    for (int i : neighbours) {
        if (is_particle_moving_closer(particles[idx], particles[i])) {
            resolve_particle_collision(particles[idx], particles[i]);
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

// inline bool check_and_resolve_particles(std::vector<int> &neighbours, std::vector<Particle> &particles, int idx, int square_size, int radius) {
//     bool unresolved = false;

//     for (int i : neighbours) {
//         if (i == -1) {
//             if (is_wall_collision(particles[idx], square_size, radius)) {
//                 resolve_wall_collision(particles[idx], square_size, radius);
//                 return true; // last element in neighbours
//             }
//         } else if (is_particle_moving_closer(particles[idx], particles[i])) {
//             resolve_particle_collision(particles[idx], particles[i]);
//             unresolved = true;
//         }
//     }
//     return unresolved;
// }

int main(int argc, char* argv[]) {
    // Read arguments and input file
    Params params{};
    std::vector<Particle> particles;
    read_args(argc, argv, params, particles);

    int grid_box_width = (params.param_radius << 2) + 1; // Width of each small box in grid
    int grid_box_row_count = std::ceil(params.square_size / grid_box_width); // No. of small boxes in a row
    int grid_box_count = grid_box_row_count * grid_box_row_count; // No. of small boxes in grid
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
    int reserve_size = params.param_particles / grid_box_count; // assume roughly equal amount per grid square

    // 2D Array stored as 1D: Stored row by row, from bottom left to top right
    std::vector<std::vector<int>> grid(grid_box_count);
    
    #pragma omp parallel for shared(grid) schedule(static) //if (grid_box_row_count >= 37)
    for (int i = 0; i < grid_box_count; ++i) {
        grid[i].reserve(reserve_size);
    }

    // 3D Array stored as 1D: for each small box in grid,
    // store bottom left, bottom right, top left, top right of this box (in this order)
    std::vector<std::vector<int>> grid_quadrants(grid_box_count << 2);

    #pragma omp parallel for shared(grid_quadrants) schedule(static)
    for (int i = 0; i < grid_box_count << 2; ++i) {
        grid_quadrants[i].reserve(reserve_size >> 2);
    }

    std::vector<std::vector<int>> overlaps(params.param_particles);

    #pragma omp parallel for shared(overlaps) schedule(static) //if (params.param_particles >= 10000)
    for (int i = 0; i < params.param_particles; ++i) {
        overlaps[i].reserve(reserve_size);
    }

    std::vector<int> wall_overlaps(params.param_particles, 0);


    for (int n = 0; n < params.param_steps; ++n) {
        // Position update
        for (Particle &p : particles) {
            p.loc.y += p.vel.y;
            p.loc.x += p.vel.x;
        }
        update_grid(grid_box_width, grid_box_row_count, grid_box_count, grid, particles);

        #pragma omp parallel for shared(grid, grid_box_row_count)
        for (int i = 0; i < grid_box_count << 2; ++i) {
            grid_quadrants[i].clear();
        }

        // Find overlaps
        #pragma omp parallel for shared(overlaps) schedule(static) //if (grid_box_row_count >= 37)
        for (int i = 0; i < params.param_particles; ++i) {
            overlaps[i].clear();
        }

        double half_box_size = grid_box_width / 2.0;

        #pragma omp parallel for shared(grid, grid_quadrants, particles) schedule(static)
        for (int box = 0; box < grid_box_count; ++box) {
            // Find which quadrant of grid box each particle is in            
            for (int i : grid[box]) {
                // 0 for left, 1 for right
                int x_dir = std::fmod(particles[i].loc.x, grid_box_width) >= half_box_size;
                // 0 for bottom, 2 for top
                int y_dir = (std::fmod(particles[i].loc.y, grid_box_width) >= half_box_size) << 1;
                // x_dir + y_dir = 0 bottom left, 1 bottom right, 2 top left, 3 top right
                grid_quadrants[(box << 2) + x_dir + y_dir].emplace_back(i);
            }
        }

        #pragma omp parallel for shared(grid, grid_quadrants, overlaps, wall_overlaps, particles) schedule(static)
        for (int j = 0; j < grid_box_count << 2; ++j) {
            int box = j >> 2;
            int quad = j - (box << 2);
            int row = box / grid_box_row_count;
            int col = box - row * grid_box_row_count;
            int row_check_idx = row + ((quad > 1) << 1) - 1; // row added with -1 for down, +1 for up
            int col_check_idx = col + ((quad & 1) << 1) - 1; // col added with -1 for left, +1 for right

            bool row_check_idx_valid = 0 <= row_check_idx && row_check_idx < grid_box_row_count;
            bool col_check_idx_valid = 0 <= col_check_idx && col_check_idx < grid_box_row_count;

            // Find overlaps
            for (int i : grid_quadrants[j]) {
                add_overlaps(grid[box], particles, i, overlaps, params.param_radius); // Same grid box
                wall_overlaps[i] = 0;
            }
            if (col_check_idx_valid) {
                for (int i : grid_quadrants[j]) {
                    add_overlaps(grid[row * grid_box_row_count + col_check_idx], particles, i, overlaps, params.param_radius);
                }
            }
            if (row_check_idx_valid) {
                for (int i : grid_quadrants[j]) {
                    add_overlaps(grid[row_check_idx * grid_box_row_count + col], particles, i, overlaps, params.param_radius);
                }
            }
                
            // Diagonally opposite grid box
            if (row_check_idx_valid && col_check_idx_valid) {
                for (int i : grid_quadrants[j]) {
                    add_overlaps(grid[row_check_idx * grid_box_row_count + col_check_idx], particles, i, overlaps, params.param_radius);
                }
            }
            // If last (or 2nd last) row / col; or Out of bounds for at least 1 axis
            if (row >= possible_wall_collision_max || col >= possible_wall_collision_max ||
                   !row_check_idx_valid || !col_check_idx_valid) {
                for (int i : grid_quadrants[j]) {
                    if (is_wall_overlap(particles[i].loc.x, particles[i].loc.y, params.square_size, params.param_radius)) {
                        //#pragma omp critical - Not needed as each thread writing to unique wall_overlaps[i]
                        wall_overlaps[i] = 1;
                    }
                }
            }
        }

        // Velocity update

        // ARBITRARY ORDER [Fastest for Small]
        // bool unresolved = true;
        // while (unresolved) {
        //     unresolved = false;
        //     for (int i = 0; i < params.param_particles; ++i) {
        //         unresolved = check_and_resolve_particles(overlaps[i], particles, i, params.square_size, params.param_radius) || unresolved;
        //     }
        // }

        // ARBITRARY REPEAT [Fastest for Medium & Large]
        // bool unresolved = true;
        // while (unresolved) {
        //     unresolved = false;
        //     for (int i = 0; i < params.param_particles; ++i) {
        //         if (check_and_resolve_particles(overlaps[i], particles, i, params.square_size, params.param_radius)) {
        //             while (check_and_resolve_particles(overlaps[i], particles, i, params.square_size, params.param_radius));
        //             unresolved = true;
        //         }
        //     }
        // }

        // TRACK ONLY CHANGED
        std::vector<int> to_resolve;
        for (int i = 0; i < params.param_particles; ++i) {
            while (check_and_resolve_particles_store_less_reverse(overlaps[i], particles, i, wall_overlaps[i], to_resolve,
                    params.square_size, params.param_radius));
        }
        while (!to_resolve.empty()) {
            std::vector<int> to_resolve2;
            for (int i : to_resolve) {
                while (check_and_resolve_particles_store(overlaps[i], particles, i, wall_overlaps[i], to_resolve2,
                        params.square_size, params.param_radius));
            }
            to_resolve.swap(to_resolve2);
        }
        

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
        // // Resolve clusters in parallel
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
