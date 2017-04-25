#!/bin/bash -l
#SBATCH --output=/home/fas/caines/ly247/scratch60/.lsbatch/%j.out
#SBATCH --partition=day
#SBATCH --job-name=pythia8
#SBATCH --ntasks=1 --nodes=1
#SBATCH --time=24:00:00


DATE=`date +%y%m%d`


echo $PWD
echo "Job Start at `date`"

make main91


echo "/usr/bin/time -v  -a ./main91 &> /home/fas/caines/ly247/Scratch/pythiadata/log/${DATE}.log"
/usr/bin/time -v  -a ./main91 &> /home/fas/caines/ly247/Scratch/pythiadata/log/${DATE}.log


wait


echo "Job End at `date`"


