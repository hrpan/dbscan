VPATH=src:obj
all: dbscan dbscan.o Point.o main.o
dbscan: main.o Point.o dbscan.o
	g++ -O2 -o $@ $^ `root-config --cflags --libs`
main.o: main.cc
	g++ -O2 -c $< -o obj/$@  `root-config --cflags --libs`
Point.o: Point.cc
	g++ -O2 -c $< -o obj/$@  
dbscan.o: dbscan.cc
	g++ -O2 -c $< -o obj/$@  `root-config --cflags --libs`
clean:
	rm ./*~ ./*.o ./main

