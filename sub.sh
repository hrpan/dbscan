#/bin/bash
#qsub -j y -l h_vmem=4G -l projectio=1 -o /global/homes/b/b0pan/project/unified/script/dbscan/jobLog/ dbscan.sh
if [ $1 == pre ]; then
	echo "Submitting prescan job"
elif [ $1 == db ]; then
	echo "Submitting dbscan job"
fi
qsub -j y -l h_vmem=4G -l projectio=1 -o /global/homes/b/b0pan/project/unified/script/dbscan/jobLog/ scan.sh $1

