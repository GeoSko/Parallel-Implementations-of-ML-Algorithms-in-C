
/*
    K-means algorithm
    Author: Georgios Skondras
*/

// *******************************************************************
#pragma GCC optimize("O3","unroll-loops","omit-frame-pointer","inline", "unsafe-math-optimizations")
//************************************************************


//***************************** Include Header Files */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <omp.h>

//***************************** Defining constants */

#define N 100000                     //Number of Vectors
#define Nv 1000                   //Number of Vector Dimensions
#define Nc 100                    //Number of clusters
#define THR_KMEANS 0.000001     //Convergence threshold
#define NUM_THREADS 4

//***************************** Function Definition */

void InitializeVectors( void );         //Function to construct random Vectors

void InitializeCenters( void );         //Function that selects random Vectors as Cluster Centers

float Classification( void );		//Function that for each Vector it calculates its Distance
                                        //from the Center and updates the Class Array

void UpdateCenters( void );             //Calculate the Centers of a Cluster using the
                                        //mean of the vectors on every Cluster
void Print(float * array, int rows, int cols); //Generic function to print arrays
void PrintClasses( void );			//Function to print classes
void CenterFreq( void );			//Print the number of vectors that belong to every centers
void EraseClasses( void );				//Erase all classes, put invalid number -1


float Vec[N][Nv];       //Array with the vectors we want to perform clustering
float Center[Nc][Nv];   //Array with the centers of each cluster
int Classes[N];         //Array that for each vector contains the number of its cluster

//***************************** Main Function */

int main(){

	int i=1;
	float change,oldDistance, newDistance;
	InitializeVectors();
	//printf("Initialized Vectors!\n");
	//Print( (float *) Vec, N, Nv);
	InitializeCenters();
	//printf("Initialized Centers!\n");
	//Print( (float *) Center, Nc, Nv);
	//PrintClasses();
	int thread_num;

	omp_set_num_threads(NUM_THREADS);
	#pragma omp parallel
	{
		// I want one thread to run this code
		#pragma omp single nowait
		{
			thread_num = omp_get_num_threads(); // Check if if the os opened the number of threads i wanted
			printf("Number of available threads:%d\n", thread_num);
		}
	}
	newDistance = FLT_MAX;
	do{
		oldDistance = newDistance;
		newDistance = Classification();
		change = fabs( newDistance - oldDistance );	//Calculating the termination variable
		UpdateCenters();
		printf("Iteration %d distance: %f\n",i,newDistance);
		i++;
		if( i > 16){
			break;
		}
	// }while( change > THR_KMEANS);
	}while(1);

	// Print(( float *) Center, Nc,Nv); //Printing the Centers
	//PrintClasses();			//Printing the Classes
	// CenterFreq();			//Printing the Center Frequency
	// printf("Finished at %d iterations\n", i-1);
	return 0;
}

//***************************** Function Declarations */

void InitializeVectors(){
	int i,j;

	for(i=0; i<N; i++){
		for(j=0; j<Nv; j++){
			Vec[i][j]= 2*((rand()%100)/100.0 -0.5);	//Random numbers in [-1,1)
		}
	}
}


void InitializeCenters(){

memcpy(Center, Vec, sizeof(float) * Nc * Nv);

}


float Classification(){
	int i,j,k;
	float dist,minDist,cumDist,temp;
	cumDist = 0;
	float euclDist;

	#pragma omp parallel
	{
		// *** Parallelization comments:
		// ~private(j,k,dist,euclDist) : these variables are used in the nested loops, so the need to be private for each thread
		// to avoid race conditions
		// ~reduction(+:cumDist) : cumDist calculates the cummulative distance (sum of distances) of all vectors from their centers
		// this reduction directive is used for synchronization and mutual exclusion, while many threads read/write from the same variable
		// so every thread calculates its own cumDist and when they join the whole cumDist is calculated
		// ~reduction(min:minDist) the same as cumDist above, but for calculating the minimum distance of a vector from all the Centers
		// ~schedule(dynamic,50) directive is used to set how the loop iteration will be divided in chunk sizes. I tried several options,
		// static ,dynimic,auto and guided and this was the fastest

		#pragma omp for private(j,k,dist,euclDist) reduction(+:cumDist) reduction(min:minDist)schedule(dynamic,50)
		for( i = 0; i < N; i++ ){
			minDist= FLT_MAX;
			for( j = 0; j < Nc; j++ ){
				dist = 0;
				#pragma omp simd
				for (k = 0; k < Nv; k++) {
					euclDist = ( Vec[i][k] - Center[j][k]) * ( Vec[i][k] - Center[j][k] );
					dist += euclDist;
				}
				if ( dist < minDist ) { //Always enters here
					minDist = dist;
					Classes[i] = j;
				}
			}
			cumDist += minDist;
		}
	}

	return cumDist;
}

void UpdateCenters(){
	int i,j;
	int classCount[Nc]={0};
	float newCenter[Nc][Nv]={0};

	//Add all values to newCenters
	for (i = 0; i < N; i++) {
        	for (j = 0; j < Nv; j++) {
            		newCenter[Classes[i]][j]+= Vec[i][j];
        	}
        (classCount[Classes[i]])++;
    	}

    	for (i = 0; i < Nc; i++){	    	//Update newCenters with average of each center
			for (j = 0; j < Nv; j++) {
			if(classCount[i] != 0){ //Avoid division by zero, if a center has
						//no vectors
				newCenter[i][j]=newCenter[i][j]/classCount[i];
			}
        	}
    	}
    	memcpy(Center, newCenter, Nc*Nv*sizeof(float));
}

void Print(float * array, int rows, int cols){
	int i,j;

	for( i = 0; i < rows; i++){
		for( j=0; j< cols; j++){
			printf("%f ", array[i*cols+ j]);
		}
		putchar('\n');
	}
	putchar('\n');

}

void PrintClasses( void ){
	int i;

	for(i=0;i<N;i++){
		printf("%d ", Classes[i]);
	}
	putchar('\n');
}

void CenterFreq( void ){
	int i;
	int centerFreq[Nc] ={0};
	for( i = 0; i< N; i++){
		centerFreq[Classes[i]]++;
	}
	for (i=0; i<Nc; i++){
		printf("Center %d has %d children\n", i,centerFreq[i]);
	}
}

//
