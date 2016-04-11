#!/bin/bash
source ~/conf.sh
if [ $1=="pre" ]; then
	/global/homes/b/b0pan/project/unified/script/dbscan/prescan
elif [ $1=="db" ]; then
	/global/homes/b/b0pan/project/unified/script/dbscan/dbscan
fi
