#!/bin/bash

## This is an example Slurm template job script for A1 that just runs the script and arguments you pass in via `srun`.

#SBATCH --job-name=test
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --mem=4gb
#SBATCH --partition=xs-4114
#SBATCH --time=00:10:00
#SBATCH --output=%x_%j.slurmlog
#SBATCH --error=%x_%j.slurmlog

echo "We are running on $(hostname)"
echo "Job started at $(date)"
#echo "Arguments to srun: $@"

# Runs your script with the arguments you passed in
srun --partition=i7-13700 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 8
srun --partition=i7-13700 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/large/100k_density_0.9.in 8
# srun --partition=i7-13700 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/standard/10k_density_0.7.in 8
# srun --partition=i7-13700 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/standard/10k_density_0.7.in 8
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/large/100k_density_0.9.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/standard/10k_density_0.7.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/standard/10k_density_0.7.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/small/random10.in 20
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./e0957806.perf tests/small/random10.in 20

# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 30
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock -r 3 ./sim.perf tests/large/100k_density_0.9.in 25
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



# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim tests/small/random10.in 20

#srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim.perf tests/large/100k_density_0.9.in 20

# echo "SMALL"
# echo "Arbitrary:"
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim.arbitrary.perf tests/small/random10.in 20
# echo "Arbitrary Repeat:"
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim.arbitrary_repeat.perf tests/small/random10.in 20
# echo "Cluster:"
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim.cluster.perf tests/small/random10.in 20

# echo "MEDIUM"
# echo "Arbitrary:"
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim.arbitrary.perf tests/standard/10k_density_0.7.in 20
# echo "Arbitrary Repeat:"
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim.arbitrary_repeat.perf tests/standard/10k_density_0.7.in 20
# echo "Cluster:"
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim.cluster.perf tests/standard/10k_density_0.7.in 20

# echo "LARGE:"
# echo "Arbitrary:"
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim.arbitrary.perf tests/large/100k_density_0.9.in 20
# echo "Arbitrary Repeat:"
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim.arbitrary_repeat.perf tests/large/100k_density_0.9.in 20
# echo "Cluster:"
# srun --partition=xs-4114 --time=00:10:00 perf stat -e task-clock ./sim.cluster.perf tests/large/100k_density_0.9.in 20


echo "Job ended at $(date)"