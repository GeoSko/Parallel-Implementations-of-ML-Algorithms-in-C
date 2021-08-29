/*
	K-means algorithm
	Author: Georgios Skondras
*/

//***************************** Include Header Files

#include <stdio.h>

//***************************** Defining constants

#define N 6			//Number of Vectors
#define Nv 2			//Number of Vector Dimensions
#define Nc 2			//Number of clusters
#define THR_KMEANS 0.000001	//Convergence threshold

//***************************** Function Definition

void InitializeVectors( void );		//Function to construct random Vectors

void InitializeCenters( void );		//Function that selects random Vectors as Cluster Centers

void CalculateDistance( void );		//Function that for each Vector it calculates its Distance
					//from the Center and updates the Class Array

void UpdateCenters( void );		//Calculate the Centers of a Cluster using the
					//mean of the vectors on every Cluster
int Terminate( void );			//Check if the termination condition is true

float Vec[N][Nv];       //Array with the vectors we want to perform clustering
float Center[Nc][Nv];   //Array with the centers of each cluster
int Classes[N];         //Array that for each vector contains the number of its cluster
float Change;           //Variable with the change of cumulative distance from the centers after every iteration o k-means


//***************************** Main Function

int main(){

    InitializeVectors();
    InitializeCenters();


    //do{
    //    CalculateDistance();
    //    UpdateCenters();

    //}while( Terminate() );
    //printf("Algorithm Finished");

    return 0;
}

//***************************** Function Declarations, not implemented yet

void InitializeVectors( void ){
	//TO DO
}

void InitializeCenters( void ){
	//TO DO
}

void CalculateDistance( void ){
	//TO DO
}

void UpdateCenters( void ){
	//TO DO
}

int Terminate( void ){
	//TO DO
}


















//
