VPATH=src
all: dbscan 
dbscan: dbscan.cc
	g++ -O2 -o $@ $< -lEG `root-config --cflags --libs`
clean:
	rm ./*~ ./*.o ./main

