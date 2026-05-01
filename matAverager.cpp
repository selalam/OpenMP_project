#include <thread>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;
// a class to get more accurate time

class stopwatch{
	
private:
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	bool timing;
	
public:
	stopwatch( ): timing( false ) {
		t1 = std::chrono::high_resolution_clock::time_point::min();
		t2 = std::chrono::high_resolution_clock::time_point::min();
	}
	
	void start( ) {
		if( !timing ) {
			timing = true;
			t1 = std::chrono::high_resolution_clock::now();
		}
	}
	
	void stop( ) {
		if( timing ) {
			t2 = std::chrono::high_resolution_clock::now();
			timing = false;
		}
	}
	
	void reset( ) {
		t1 = std::chrono::high_resolution_clock::time_point::min();
		t2 = std::chrono::high_resolution_clock::time_point::min();;
		timing = false;
	}
	
	// will return the elapsed time in seconds as a double
	double getTime( ) {
		std::chrono::duration<double> elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(t2-t1);
		return elapsed.count();
	}
};



// function takes an array pointer, and the number of rows and cols in the array, and 
// allocates and intializes the two dimensional array to a bunch of random numbers

void makeRandArray( unsigned int **& data, unsigned int rows, unsigned int cols, unsigned int seed )
{
	// allocate the array
	data = new unsigned int*[ rows ];
	for( unsigned int i = 0; i < rows; i++ )
	{
		data[i] = new unsigned int[ cols ];
	}
	
	// seed the number generator
	// you should change the seed to get different values
	srand( seed );
	
	// populate the array
	
	for( unsigned int i = 0; i < rows; i++ )
		for( unsigned int j = 0; j < cols; j++ )
		{
			data[i][j] = rand() % 10000 + 1; // number between 1 and 10000
		}
	
}

void getDataFromFile( unsigned int **& data, char fileName[], unsigned int &rows, unsigned int &cols )
{
	ifstream in;
	in.open( fileName );
	if( !in )
	{
		cerr << "error opening file: " << fileName << endl;
		exit( -1 );
	}
	
	in >> rows >> cols;
	data = new unsigned int*[ rows ];
	for( unsigned int i = 0; i < rows; i++ )
	{
		data[i] = new unsigned int[ cols ];
	}
	
	// now read in the data
	
	for( unsigned int i = 0; i < rows; i++ )
		for( unsigned int j = 0; j < cols; j++ )
		{
			in >> data[i][j];
		}
	
}


int main( int argc, char* argv[] ) 
{
	if( argc < 3 )
	{
		cerr<<"Usage: " << argv[0] << " [input data file] [num of threads to use] " << endl;
		
		cerr<<"or" << endl << "Usage: "<< argv[0] << " rand [num of threads to use] [num rows] [num cols] [seed value]" << endl;
                exit( 0 );
        }
	
	// read in the file
	unsigned int rows, cols, seed;
	unsigned int numThreads;
	unsigned int ** data;
	// convert numThreads to int
	{
		stringstream ss1;
		ss1 << argv[2];
		ss1 >> numThreads;
	}
	
	string fName( argv[1] );
	if( fName == "rand" )
	{
		{
			stringstream ss1;
			ss1 << argv[3];
			ss1 >> rows;
		}
		{
			stringstream ss1;
			ss1 << argv[4];
			ss1 >> cols;
		}
		{
			stringstream ss1;
			ss1 << argv[5];
			ss1 >> seed;
		}
		makeRandArray( data, rows, cols, seed );
	}
	else
	{
		getDataFromFile( data,  argv[1], rows, cols );
	}

/*	//UNCOMMENT if you want to print the data array	
 *	cerr << "data: " << endl;
 *   	for( unsigned int i = 0; i < rows; i++ )
 *	{
 *		for( unsigned int j = 0; j < cols; j++ )
 *		{
 *			cerr << "i,j,data " << i << ", " << j << ", ";
 *			cerr << data[i][j] << " ";
 *		}
 *		cerr << endl;
 *	}
 *	cerr<< endl;
*/
	// tell omp how many threads to use
	omp_set_num_threads( numThreads );
	
	
	stopwatch S1;
	S1.start();

	/////////////////////////////////////////////////////////////////////
	///////////////////////  YOUR CODE HERE       ///////////////////////
	///////////////         Make it parallel!	 ////////////////////
	/////////////////////////////////////////////////////////////////////

	// #pragma omp parallel
	// {
		//  TODO
		// int omp_get_thread_num(); // Returns the thread id
		// int omp_get_num_threads(); // Returns the amount of threads running
		// void 		
	// }

//		#pragma omp critical // This forces single thread execution at a time 'serialize'
		// #pragma omp parallel for reduction(+:count) can only do one variable, but struct allows us to circumvent this
	
	double max = -1.0;
	vector<pair<int, int> > maxLocations;

#pragma omp parallel
	{
		double localMax = -1.0;
		vector<pair<int, int> > localMaxLocations;

#pragma omp for
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				int sum = 0;
				double divisor = 1.0;
				if (i == 0 && j == 0) {
					sum = data[i][j] + data[i][j+1] +
						data[i+1][j] + data[i+1][j+1];
						divisor = 4.0;
				} else if (i == 0 && j == (cols-1)) {
					sum = data[i][j-1] + data[i][j] +
						data[i+1][j-1] + data[i+1][j];
						divisor = 4.0;
				} else if (i == 0) {
					sum = data[i][j-1] + data[i][j] + data[i][j+1] +
						data[i+1][j-1] + data[i+1][j] + data[i+1][j+1];
						divisor = 6.0;
				} else if (i == (rows-1) && j == 0) {
					sum = data[i-1][j] + data[i-1][j+1] +
						data[i][j] + data[i][j+1];
						divisor = 4.0;
				} else if (i == (rows-1) && j == (cols-1)) {
					sum = data[i-1][j-1] + data[i-1][j] +
						data[i][j-1] + data[i][j];
						divisor = 4.0;
				} else if (i == (rows-1)) {
					sum = data[i-1][j-1] + data[i-1][j] + data[i-1][j+1] +
							data[i][j-1] + data[i][j] + data[i][j+1];
						divisor = 6.0;
				} else if (j == 0) {
					sum = data[i-1][j] + data[i-1][j+1] +
							data[i][j] + data[i][j+1] +
							data[i+1][j] + data[i+1][j+1];
						divisor = 6.0;
				} else if (j == (cols-1)) {
					sum = data[i-1][j-1] + data[i-1][j] +
							data[i][j-1] + data[i][j] +
							data[i+1][j-1] + data[i+1][j];
						divisor = 6.0;
				} else {
					sum = data[i-1][j-1] + data[i-1][j] + data[i-1][j+1] +
						data[i][j-1] + data[i][j] + data[i][j+1] +
						data[i+1][j-1] + data[i+1][j] + data[i+1][j+1];
						divisor = 9.0;
				}
				double avg = sum / divisor; // This was avg = sum / 9, which was fully integer division and we would lose decimal precision

				if (avg > localMax) {
					localMax = avg;
					localMaxLocations.clear();
					localMaxLocations.push_back(make_pair(i, j));
				} else if (avg == localMax) {
					localMaxLocations.push_back(make_pair(i, j));
				}
			}
		}

#pragma omp critical
		{
			if (localMax > max) {
				max = localMax;
				maxLocations = localMaxLocations;
			} else if (localMax == max) {
				maxLocations.insert(maxLocations.end(), localMaxLocations.begin(), localMaxLocations.end());
			}
		}
	}

	// TODO Catch (literal) edge cases
	
	
	S1.stop();
	
	// print out the max value here
	sort(maxLocations.begin(), maxLocations.end());
	cerr << "largest average: " << max << endl;
	cerr << "found at cells: ";
	for (unsigned int i = 0; i < maxLocations.size(); i++) {
		cerr << "(" << maxLocations[i].first << ", " << maxLocations[i].second << ")\t";
	}
	cerr << endl;
	cerr << "elapsed time: " << S1.getTime( ) << endl;
}