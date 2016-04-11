#/bin/bash
qsub -j y -l h_vmem=4G -l projectio=1 -o /global/homes/b/b0pan/project/unified/script/dbscan/jobLog/ dbscan.sh
