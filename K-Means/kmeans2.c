/*
    K-means algorithm
    Author: Georgios Skondras
*/

//***************************** Include Header Files */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>

//***************************** Defining constants */
/*
#define N 100000                     //Number of Vectors
#define Nv 1000                    //Number of Vector Dimensions
#define Nc 100                    //Number of clusters
#define THR_KMEANS 0.000001     //Convergence threshold
*/



#define N 4                     //Number of Vectors
#define Nv 2                    //Number of Vector Dimensions
#define Nc 2                    //Number of clusters
#define THR_KMEANS 0.000001     //Convergence threshold

//***************************** Function Definition */

void InitializeVectors( void );         //Function to construct random Vectors

void InitializeCenters( void );         //Function that selects random Vectors as Cluster Centers

float Classification( void );		//Function that for each Vector it calculates its Distance
                                        //from the Center and updates the Class Array

void UpdateCenters( void );             //Calculate the Centers of a Cluster using the
                                        //mean of the vectors on every Cluster
void Print(float * array, int rows, int cols);
void PrintClasses( void );

float Vec[N][Nv];       //Array with the vectors we want to perform clustering
float Center[Nc][Nv];   //Array with the centers of each cluster
int Classes[N];         //Array that for each vector contains the number of its cluster

//***************************** Main Function */

int main(){
	int i;
	InitializeVectors();
	printf("Initialized Vectors!\n");
	Print( (float *) Vec, N, Nv);
	InitializeCenters();
		printf("Initialized Centers!\n");

	Print( (float *) Center, Nc, Nv);

	for(i=0; i<5; i++){
		Classification();
		UpdateCenters();
		printf("Iteration %d!\n", i);

	}

	Print(( float *) Center, Nc,Nv);
	PrintClasses();

	return 0;
}

//***************************** Function Declarations */

void InitializeVectors(){
	int i,j;

	for(i=0; i<N; i++){
		for(j=0; j<Nv; j++){
			Vec[i][j]= 2*((rand()%100)/1.0 -50);	//Random numbers in [-10,10)
		}
	}
}


void InitializeCenters(){

memcpy(Center, Vec, sizeof(float) *Nc*Nv);
/*
int i,j,vecSelect;
int vecTaken[N] ={0};	//Array with flags. when the value is 1, it means that in this
			//position the vector is already selected as Cluster Center

    for(i=0; i<Nc; i++){
        do {
            vecSelect= rand()%N;
        } while(vecTaken[vecSelect]==1);

        for(j=0; j<Nv; j++){
            Center[i][j]= Vec[vecSelect][j];
            vecTaken[vecSelect]=1;
        }
    }
*/

}


float Classification(){
    int i,j,k;
    float dist,minDist,euclDist,cumDist,temp;
    cumDist = 0;

    for( i = 0; i < N; i++ ){
        minDist= FLT_MAX;
        for( j = 0; j < Nc; j++ ){
            dist = 0;
            for (k = 0; k < Nv; k++) {
		temp = Vec[i][k] - Center[j][k];	 //Calculating the Euclidean Distance
		euclDist = temp * temp;

                dist += euclDist;
            }
            if ( dist < minDist ) {//Always enters here
                minDist = dist;
                Classes[i] = j;
            }
        }
        cumDist += minDist;
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

    for (i = 0; i < Nc; i++) {		//Update newCenters with average of each center
        for (j = 0; j < Nv; j++) {
            newCenter[i][j]=newCenter[i][j]/classCount[i];
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

//
