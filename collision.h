#pragma once

/**
 * This file contains default collision detection and resolution logic.
 */

#include <cassert>
#include <cmath>
#include <cstdlib>

#include "io.h"

// Is particle overlapping with a wall
inline bool is_wall_overlap(Vec2 loc, int square_size, int radius);
// Is particle colliding with a wall
inline bool is_wall_collision(std::vector<Particle> &particles, int idx, int square_size, int radius);
// Make particle not collide with wall
// PRECONDITION: Can be called with no preconditions
inline void resolve_wall_collision(std::vector<Particle> &particles, int idx, int square_size, int radius);
// Are the particles overlapping
inline bool is_particle_overlap(Vec2 loc1, Vec2 loc2, int radius);
// Are the particles moving closer together
inline bool is_particle_moving_closer(std::vector<Particle> &particles, int idx1, int idx2);
// Are the particles colliding
inline bool is_particle_collision(std::vector<Particle> &particles, int idx1, int idx2);
// Make particles not collide with each other
// PRECONDITION: Must only be called if particles are overlapping
inline void resolve_particle_collision(std::vector<Particle> &particles, int idx1, int idx2);
// Get the total energy of a group of particles
inline double getEnergy(std::vector<Particle>& particles);
// Get the total momentum of a group of particles
inline Vec2 getMomentum(std::vector<Particle>& particles);

/*******************
 * IMPLEMENTATIONS *
 *******************/

/**
 * Whether a particle is overlapping with a wall
 * - loc: The location of the particle
 * - square_size: The length of the simulation area
 * - radius: The radius of the particle
 */
inline bool is_wall_overlap(Vec2 loc, int square_size, int radius) {
    return loc.x - radius <= 0 || loc.x + radius >= square_size ||
            loc.y - radius <= 0 || loc.y + radius >= square_size;
}

/**
 * Whether a particle is colliding with a wall
 * - loc: The location of the particle
 * - square_size: The length of the simulation area
 * - radius: The radius of the particle
 */
inline bool is_wall_collision(std::vector<Particle> &particles, int idx, int square_size, int radius) {
    Particle &p = particles[idx];
    return (p.loc.x - radius <= 0 && p.vel.x < 0) || (p.loc.x + radius >= square_size && p.vel.x > 0) ||
            (p.loc.y - radius <= 0 && p.vel.y < 0) || (p.loc.y + radius >= square_size && p.vel.y > 0);
}

/**
 * Implements the rules to change the velocity of a particle that collides with a wall
 * - loc: The location of the particle
 * - vel: The velocity of the particle
 * - square_size: The length of the simulation area
 * - radius: The radius of the particle
 */
inline void resolve_wall_collision(std::vector<Particle> &particles, int idx, int square_size, int radius) {
    Particle &p = particles[idx];
    if (p.loc.x - radius <= 0) {
        p.vel.x = std::abs(p.vel.x);
    } else if (p.loc.x + radius >= square_size) {
        p.vel.x = -std::abs(p.vel.x);
    }
    if (p.loc.y - radius <= 0) {
        p.vel.y = std::abs(p.vel.y);
    } else if (p.loc.y + radius >= square_size) {
        p.vel.y = -std::abs(p.vel.y);
    }
}

/**
 * Whether two particles are overlapping
 * - loc1: The location of the first particle
 * - loc2: The location of the second particle
 * - radius: The radius of the particles
 */
inline bool is_particle_overlap(Vec2 loc1, Vec2 loc2, int radius) {
    double dx = loc2.x - loc1.x;
    double dy = loc2.y - loc1.y;
    return (dx * dx + dy * dy) <= (radius << 1) * (radius << 1);
}

/**
 * Whether two particles are moving closer together, with some tolerance
 * - loc1: The location of the first particle
 * - vel1: The velocity of the first particle
 * - loc2: The location of the second particle
 * - vel2: The velocity of the second particle
 */
inline bool is_particle_moving_closer(std::vector<Particle> &particles, int idx1, int idx2) {
    Particle &p1 = particles[idx1];
    Particle &p2 = particles[idx2];
    return (p2.vel.x - p1.vel.x) * (p2.loc.x - p1.loc.x) +
            (p2.vel.y - p1.vel.y) * (p2.loc.y - p1.loc.y) < -0.0000001;
}

/**
 * Whether two particles are colliding
 * - loc1: The location of the first particle
 * - vel1: The velocity of the first particle
 * - loc2: The location of the second particle
 * - vel2: The velocity of the second particle
 * - radius: The radius of the particles
 */
inline bool is_particle_collision(std::vector<Particle> &particles, int idx1, int idx2, int radius) {
    return is_particle_overlap(particles[idx1].loc, particles[idx2].loc, radius) && is_particle_moving_closer(particles, idx1, idx2);
}

/**
 * Implements the rules to change the velocity of two particles that collide
 * - loc1: The location of the first particle
 * - vel1: The velocity of the first particle
 * - loc2: The location of the second particle
 * - vel2: The velocity of the second particle
 */
inline void resolve_particle_collision(std::vector<Particle> &particles, int idx1, int idx2) {
    Particle &p1 = particles[idx1];
    Particle &p2 = particles[idx2];
    double dx = p2.loc.x - p1.loc.x;
    double dy = p2.loc.y - p1.loc.y;

    // Calculate the new velocities of the particles after the collision
    double collision_scale = std::min(0.0, (p2.vel.x - p1.vel.x) * dx + (p2.vel.y - p1.vel.y) * dy) /
            (dx * dx + dy * dy);

    p1.vel.x += collision_scale * dx;
    p1.vel.y += collision_scale * dy;
    p2.vel.x -= collision_scale * dx;
    p2.vel.y -= collision_scale * dy;
}

/**
 * Get the total energy of a group of particles
 * - particles: The particles to calculate the energy of
 */
inline double getEnergy(std::vector<Particle>& particles) {
    double energy = 0;
    for (Particle& p : particles) {
        energy += p.vel.x * p.vel.x + p.vel.y * p.vel.y;
    }
    return energy;
}

/**
 * Get the total momentum of a group of particles
 * - particles: The particles to calculate the momentum of
 */
inline Vec2 getMomentum(std::vector<Particle>& particles) {
    Vec2 momentum = {0.0, 0.0};
    for (Particle& p : particles) {
        momentum.x += p.vel.x;
        momentum.y += p.vel.y;
    }
    return momentum;
}
