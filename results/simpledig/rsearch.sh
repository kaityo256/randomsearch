#!/bin/sh
#QSUB -queue F144cpu
#QSUB -node 72
#PBS -l walltime=24:00:00
#PBS -m be
cd $PBS_O_WORKDIR

mpijob ./a.out 
