all:
	g++ -std=c++11 -fopenmp -ggdb matAverager.cpp -o matavg

opt:
	g++ matAverager.cpp -o matavg -fopenmp -O3
