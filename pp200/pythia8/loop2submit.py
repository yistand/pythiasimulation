#!/usr/bin/python

import os

for i in range(1,11) :
	print("sbatch next_submitjob.sh {}171121".format(i))
	os.system("sbatch next_submitjob.sh {}171121".format(i))


