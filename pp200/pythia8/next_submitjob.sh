#!/bin/bash -l
#SBATCH --output=/home/fas/caines/ly247/scratch60/.lsbatch/%j.out
#SBATCH --partition=day
#SBATCH --job-name=gepy8
#SBATCH --ntasks=1 --nodes=1
#SBATCH --time=24:00:00


# to run the program do `sbatch next_submitjob.sh`

DATE=`date +%y%m%d`


echo $PWD
echo "Job Start at `date`"

#make mainppbar300GeV


seed=$1
echo "/usr/bin/time -v  -a ./mainMimicPeterSkands200GeV  $seed &> /home/fas/caines/ly247/Scratch/pythiadata/log/pp200_${seed}_${DATE}.log"
/usr/bin/time -v  -a ./mainMimicPeterSkands200GeV $seed   &> /home/fas/caines/ly247/Scratch/pythiadata/log/pp200_${seed}_${DATE}.log


wait


echo "Job End at `date`"


