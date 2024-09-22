#!/bin/bash

# Vary no. of Particles
# ./gen_testcase.py 10000 10000 15 100 2 5 > tests/generated/num_particles/10k.in
# ./gen_testcase.py 20000 10000 15 100 2 5 > tests/generated/num_particles/20k.in
# ./gen_testcase.py 30000 10000 15 100 2 5 > tests/generated/num_particles/30k.in
# ./gen_testcase.py 40000 10000 15 100 2 5 > tests/generated/num_particles/40k.in
# ./gen_testcase.py 50000 10000 15 100 2 5 > tests/generated/num_particles/50k.in
# ./gen_testcase.py 60000 10000 15 100 2 5 > tests/generated/num_particles/60k.in
# ./gen_testcase.py 70000 10000 15 100 2 5 > tests/generated/num_particles/70k.in
# ./gen_testcase.py 80000 10000 15 100 2 5 > tests/generated/num_particles/80k.in
# ./gen_testcase.py 90000 10000 15 100 2 5 > tests/generated/num_particles/90k.in
# ./gen_testcase.py 100000 10000 15 100 2 5 > tests/generated/num_particles/100k.in

# Vary grid length
# ./gen_testcase.py 100000 10000 15 100 2 5 > tests/generated/grid_length/10k.in
# ./gen_testcase.py 100000 20000 15 100 2 5 > tests/generated/grid_length/20k.in
# ./gen_testcase.py 100000 30000 15 100 2 5 > tests/generated/grid_length/30k.in
# ./gen_testcase.py 100000 40000 15 100 2 5 > tests/generated/grid_length/40k.in
# ./gen_testcase.py 100000 50000 15 100 2 5 > tests/generated/grid_length/50k.in
# ./gen_testcase.py 100000 60000 15 100 2 5 > tests/generated/grid_length/60k.in
# ./gen_testcase.py 100000 70000 15 100 2 5 > tests/generated/grid_length/70k.in
# ./gen_testcase.py 100000 80000 15 100 2 5 > tests/generated/grid_length/80k.in
# ./gen_testcase.py 100000 90000 15 100 2 5 > tests/generated/grid_length/90k.in
# ./gen_testcase.py 100000 100000 15 100 2 5 > tests/generated/grid_length/100k.in

# Vary radius
# ./gen_testcase.py 10000 10000 5 100 2 5 > tests/generated/radius/5.in
# ./gen_testcase.py 10000 10000 10 100 2 5 > tests/generated/radius/10.in
# ./gen_testcase.py 10000 10000 15 100 2 5 > tests/generated/radius/15.in
# ./gen_testcase.py 10000 10000 20 100 2 5 > tests/generated/radius/20.in
# ./gen_testcase.py 10000 10000 25 100 2 5 > tests/generated/radius/25.in
# ./gen_testcase.py 10000 10000 30 100 2 5 > tests/generated/radius/30.in
# ./gen_testcase.py 10000 10000 35 100 2 5 > tests/generated/radius/35.in
# ./gen_testcase.py 10000 10000 40 100 2 5 > tests/generated/radius/40.in
# ./gen_testcase.py 10000 10000 45 100 2 5 > tests/generated/radius/45.in
# ./gen_testcase.py 10000 10000 50 100 2 5 > tests/generated/radius/50.in

# Vary steps
# ./gen_testcase.py 100000 10000 15 20 2 5 > tests/generated/steps/20.in
# ./gen_testcase.py 100000 10000 15 40 2 5 > tests/generated/steps/40.in
# ./gen_testcase.py 100000 10000 15 60 2 5 > tests/generated/steps/60.in
# ./gen_testcase.py 100000 10000 15 80 2 5 > tests/generated/steps/80.in
# ./gen_testcase.py 100000 10000 15 100 2 5 > tests/generated/steps/100.in
# ./gen_testcase.py 100000 10000 15 120 2 5 > tests/generated/steps/120.in
# ./gen_testcase.py 100000 10000 15 140 2 5 > tests/generated/steps/140.in
# ./gen_testcase.py 100000 10000 15 160 2 5 > tests/generated/steps/160.in
# ./gen_testcase.py 100000 10000 15 180 2 5 > tests/generated/steps/180.in
# ./gen_testcase.py 100000 10000 15 200 2 5 > tests/generated/steps/200.in

# Vary Min. Velocity
# ./gen_testcase.py 100000 10000 15 100 1 50 > tests/generated/min_vel/1.in
# ./gen_testcase.py 100000 10000 15 100 5 50 > tests/generated/min_vel/5.in
# ./gen_testcase.py 100000 10000 15 100 10 50 > tests/generated/min_vel/10.in
# ./gen_testcase.py 100000 10000 15 100 15 50 > tests/generated/min_vel/15.in
# ./gen_testcase.py 100000 10000 15 100 20 50 > tests/generated/min_vel/20.in
# ./gen_testcase.py 100000 10000 15 100 25 50 > tests/generated/min_vel/25.in
# ./gen_testcase.py 100000 10000 15 100 30 50 > tests/generated/min_vel/30.in
# ./gen_testcase.py 100000 10000 15 100 35 50 > tests/generated/min_vel/35.in
# ./gen_testcase.py 100000 10000 15 100 40 50 > tests/generated/min_vel/40.in
# ./gen_testcase.py 100000 10000 15 100 45 50 > tests/generated/min_vel/45.in
# ./gen_testcase.py 100000 10000 15 100 50 50 > tests/generated/min_vel/50.in

# Vary Max. Velocity
# ./gen_testcase.py 100000 10000 15 100 1 1 > tests/generated/max_vel/1.in
# ./gen_testcase.py 100000 10000 15 100 1 5 > tests/generated/max_vel/5.in
# ./gen_testcase.py 100000 10000 15 100 1 10 > tests/generated/max_vel/10.in
# ./gen_testcase.py 100000 10000 15 100 1 15 > tests/generated/max_vel/15.in
# ./gen_testcase.py 100000 10000 15 100 1 20 > tests/generated/max_vel/20.in
# ./gen_testcase.py 100000 10000 15 100 1 25 > tests/generated/max_vel/25.in
# ./gen_testcase.py 100000 10000 15 100 1 30 > tests/generated/max_vel/30.in
# ./gen_testcase.py 100000 10000 15 100 1 35 > tests/generated/max_vel/35.in
# ./gen_testcase.py 100000 10000 15 100 1 40 > tests/generated/max_vel/40.in
# ./gen_testcase.py 100000 10000 15 100 1 45 > tests/generated/max_vel/45.in
# ./gen_testcase.py 100000 10000 15 100 1 50 > tests/generated/max_vel/50.in


