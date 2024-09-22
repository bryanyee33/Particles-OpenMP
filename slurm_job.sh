#!/bin/bash

## This is an example Slurm template job script for A1 that just runs the script and arguments you pass in via `srun`.

#SBATCH --job-name=test
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --mem=4gb
#SBATCH --partition=xs-4114
#SBATCH --time=00:30:00
#SBATCH --output=%x_%j.slurmlog
#SBATCH --error=%x_%j.slurmlog

echo "We are running on $(hostname)"
echo "Job started at $(date)"
#echo "Arguments to srun: $@"

# Runs your script with the arguments you passed in

# Own Testing
srun --partition=i7-13700 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 16
srun --partition=i7-13700 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/large/100k_density_0.9.in 16
# srun --partition=i7-13700 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/standard/10k_density_0.7.in 8
# srun --partition=i7-13700 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/standard/10k_density_0.7.in 8
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/large/100k_density_0.9.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/standard/10k_density_0.7.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/standard/10k_density_0.7.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/small/random10.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/small/random10.in 20

# Vary no. of threads
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 30
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 25
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 21
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 15
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 10
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 9
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 8
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 7
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 6
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 5
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 4
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 3
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 2
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 1

# Vary no. of Particles
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/num_particles/10k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/num_particles/20k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/num_particles/30k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/num_particles/40k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/num_particles/50k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/num_particles/60k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/num_particles/70k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/num_particles/80k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/num_particles/90k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/num_particles/100k.in 20

# Vary grid length
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/9k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/10k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/20k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/30k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/40k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/50k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/60k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/70k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/80k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/90k.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/grid_length/100k.in 20

# Vary radius
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/radius/5.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/radius/10.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/radius/15.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/radius/20.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/radius/25.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/radius/30.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/radius/35.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/radius/40.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/radius/45.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/radius/50.in 20

# Vary steps
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/steps/20.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/steps/40.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/steps/60.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/steps/80.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/steps/100.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/steps/120.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/steps/140.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/steps/160.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/steps/180.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/steps/200.in 20

# Vary min velocity
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/1.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/5.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/10.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/15.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/20.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/25.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/30.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/35.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/40.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/45.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/min_vel/50.in 20

# Vary max velocity
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/1.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/5.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/10.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/15.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/20.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/25.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/30.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/35.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/40.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/45.in 20
# srun --partition=xs-4114 perf stat -e task-clock -r 3 ./sim.perf tests/generated/max_vel/50.in 20

# Different Machines
# for (( i=0; i<5; i=i+1 )) do
#   srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 16
# done
# for (( i=0; i<5; i=i+1 )) do
#   srun --partition=w5-3423 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 16
# done
# for (( i=0; i<5; i=i+1 )) do
#   srun --partition=i7-13700 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 16
# done


# 9 boxes vs 4 boxes
# for (( i=0; i<5; i=i+1 )) do
#   srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./9boxes.perf tests/large/100k_density_0.9.in 20
# done
# for (( i=0; i<5; i=i+1 )) do
#   srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/large/100k_density_0.9.in 20
# done

# Self-contained resolves enabled vs disabled
# for (( i=0; i<10; i=i+1 )) do
#   srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./no_self_contained_resolves.perf tests/large/100k_density_0.9.in 20
# done
# for (( i=0; i<10; i=i+1 )) do
#   srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/large/100k_density_0.9.in 20
# done

# Self-contained resolves enabled vs disabled: track
# for (( i=0; i<10; i=i+1 )) do
#   srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./no_self_contained_resolves-track.perf tests/large/100k_density_0.9.in 20
# done
# for (( i=0; i<10; i=i+1 )) do
#   srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./self_contained_resolves-track.perf tests/large/100k_density_0.9.in 20
# done


echo "Job ended at $(date)"