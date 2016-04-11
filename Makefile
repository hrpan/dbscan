all: dbscan prescan
dbscan: dbscan.cc
	g++ -O2 -g -o dbscan dbscan.cc -lEG `root-config --cflags` `root-config --libs` -lTreePlayer
prescan: prescan.cc
	g++ -O2 -g -o prescan prescan.cc -lEG `root-config --cflags` `root-config --libs` -lTreePlayer
clean:
	rm ./*~ ./*.o ./main

