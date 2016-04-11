VPATH=src
all: dbscan prescan
dbscan: dbscan.cc
	g++ -O2 -o $@ $< -lEG `root-config --cflags --libs`
prescan: prescan.cc
	g++ -O2 -o $@ $< -lEG `root-config --cflags --libs`
clean:
	rm ./*~ ./*.o ./main

