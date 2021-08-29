#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <omp.h>

#define NUM_CITIES 10000
#define GRID_SIZE 1000

#define INITIAL_PHERORMONE 100
#define A 0.9

// #define NUM_CITIES 5
// #define GRID_SIZE 5

#define NUM_ANTS 4
#define NUM_ITERATIONS 10

void createCities();                    //Initialization of Cities
double calculateDistance();              //Returns the distance of the Current Route
void printCities();                     //Prints the list of cities and their coordinates
void printRoutes();                      //Print the current Route
int nextCity( int city, int ant);              //Returns the city next to city parameter
double twoCitiesDistance(int city1, int city2); //Calculates the euclidean distance between 2 cities
void printMask();                       //Prints a mask-array with info of wich cities are aleady in a route. Function for debugging
void deployAnts();                      //Deploys ants randomly in cities
void singleAntTour(int ant);                  //Takes an ant and as a parameter and tours it arount the cities
void allAntsTour(int iteration);                         //Does a tour for all ants
void updateBestRoute(int iteration);                   //Updates the final results. Calculates the shortest route, and saves
                                        //the route itself, its length and the ant that perfomed it in struct
                                        //Final results
void printBestResults();                //Prints the best results
double cityScore(int city1, int city2);             //Returns the score of each city according to distance and pherormone
void initializePherormones();                       //Sets a small positive values to pherormones
void clearAntMask(int ant);
void swap(int* city1, int* city2);


typedef struct Results {
   int ant;
   int iteration;
   int Route[NUM_CITIES];
   double distance;

} Result;
Result FinalResult;

int Cities[NUM_CITIES][2];
int Route[NUM_ANTS][NUM_CITIES];
int cityTaken[NUM_ANTS][NUM_CITIES]={0};  //Mask array with one in taken cities positions
double pherormone[NUM_CITIES][NUM_CITIES]; //Table with info about pherormones

int main(){
   // int ant=0;
   double distance;
   FinalResult.distance = DBL_MAX;

   printf("Number of cities: %d\nGrid: %dX%d\n\n", NUM_CITIES,GRID_SIZE,GRID_SIZE );
   createCities();
   // printCities();
   // deployAnts();
   initializePherormones();
   // printPherormones();

   // allAntsTour(0);
   // printRoutes();
   // printPherormones();




   for(int iter =0; iter<NUM_ITERATIONS; iter++){
       deployAnts();
       allAntsTour(iter);
       // printRoutes();
   }
   printBestResults();

   // printPherormones();

   return 0;
}

void createCities(){
   int i,j;
   for(i = 0; i< NUM_CITIES; i++){
       for(j = 0; j<2; j++){
           Cities[i][j] = rand()%GRID_SIZE;
       }
   }
   printf("Cities Created\n\n" );
}

void printCities(){
   int i,j;
   printf("Cities:\n");
   for(i=0; i<NUM_CITIES; i++){
       printf("%d: ", i );
       for(j=0; j<2; j++){
           printf("%d ", Cities[i][j] );
       }
       putchar('\n');
   }
   putchar('\n');
}

void printRoutes(){
   int ant;
   printf("Ant routes:\n");
   for(ant=0; ant<NUM_ANTS; ant++){
       printf("%d: ", ant );
       for (int j = 0; j < NUM_CITIES; j++) {
           printf("%d -> ", Route[ant][j]);
       }
       printf("%d\n", Route[ant][0]);
       putchar('\n');
   }
}

int nextCity(int city, int ant){
   int i,bestCity;
   double bestScore=-1;
   double tempScore;

   for(i=0; i<NUM_CITIES; i++){
       if(cityTaken[ant][i]==0 && i!=city ){    //Check if city is not already in route and is not the se ma to the last one
           // printf("city:%d i:%d \n",city,i );
           tempScore = cityScore(city,i);
           if( tempScore > bestScore ){
               bestScore = tempScore;
               bestCity = i;
           }
       }
   }
   // printf("best city:%d\n", bestCity );
   cityTaken[ant][bestCity] = 1;   //
   // printMask();
   return bestCity;
}

double twoCitiesDistance( int city1, int city2){
   double distance=0;
   int j;

   for(j=0; j<2; j++){
           distance += ((Cities[city1][j] - Cities[city2][j]) * (Cities[city1][j] - Cities[city2][j]));
   }
   distance=sqrt(distance);
   return distance;

}

void printMask(){
    printf("Ant masks:\n");
    for(int ant=0; ant<NUM_ANTS; ant++){
        printf("%d: ", ant );
        for(int j=0; j<NUM_CITIES; j++){
           printf("%d ",cityTaken[ant][j] );
       }
       printf("\n\n");
    }
}

void deployAnts(){
    int ant;
    int antStart[NUM_ANTS]; //Array with the beginning city of each ant
    for(ant=0; ant<NUM_ANTS; ant++){
        antStart[ant] = rand()%NUM_CITIES;
        Route[ant][0] = antStart[ant];
        cityTaken[ant][antStart[ant]] =1;
        // printf("Ant: %d deployed at city: %d \n",ant, antStart[ant] );
    }
    // putchar('\n');
    // printf("All Ants deployed\n\n");
}

void singleAntTour(int ant){
    int nextNode,node;
    for(int i=1; i<NUM_CITIES; i++){
        node = Route[ant][i-1];
        nextNode = nextCity(node, ant);
        Route[ant][i] = nextNode;  // The current city is the closest to the previous
        if(node>nextNode){    //update the pherormone
            swap(&node,&nextNode);
        }
        #pragma omp critical
        pherormone[node][nextNode] = (1-A)*pherormone[node][nextNode]+A/twoCitiesDistance(node,nextNode);
        // printRoutes();
        // printMask();
        // printf("%f %f \n", (1-A)*pherormone[node][nextNode] , A/twoCitiesDistance(node,nextNode));
    }

}

void allAntsTour(int iteration){
    int ant;
    // memset(Route, 0, sizeof(Route)); //Reset the Routes to zero
    #pragma omp parallel for
    for(ant=0; ant<NUM_ANTS; ant++){
        singleAntTour(ant);
        clearAntMask(ant);  //Reset the masks to zero for specific ant

    }

    updateBestRoute(iteration);
}

void updateBestRoute(int iteration){
    double tempDistance,distance,minDistance;
    int bestAnt;
    printf("Beggining tour number %d\n\n", iteration );
    minDistance=DBL_MAX;
    for(int ant=0; ant<NUM_ANTS; ant++){    //This loop calulates the distance that every ant covered
        distance=0;
        for(int i=0; i<NUM_CITIES-1; i++){
            tempDistance=0;
            for(int j=0; j<2; j++){
                tempDistance += ((Cities[Route[ant][i]][j] - Cities[Route[ant][i+1]][j]) * (Cities[Route[ant][i]][j] - Cities[Route[ant][i+1]][j]));
            }
            // printf("%f\n",sqrt(tempDistance) );
            distance+=sqrt(tempDistance);
        }
        tempDistance=0;
        for(int j=0; j<2; j++){ //Last to first
            tempDistance += (Cities[Route[ant][NUM_CITIES-1]][j] - Cities[Route[ant][0]][j]) * (Cities[Route[ant][NUM_CITIES-1]][j] - Cities[Route[ant][0]][j]);
        }
        distance+=sqrt(tempDistance);
        printf("Ant %d Distance: %f\n",ant, distance );

        if(distance<minDistance){   //Update the best solution so far
            bestAnt=ant;
            minDistance=distance;
        }

    }
    if(minDistance<FinalResult.distance){   //Update the best solution so far
        FinalResult.ant=bestAnt;
        FinalResult.distance=minDistance;
        FinalResult.iteration=iteration;
        memcpy((&FinalResult)->Route, Route[bestAnt], NUM_CITIES*sizeof(int));
    }
}

void printBestResults(){
    printf("BEST RESULTS:\n\nBest ant: %d\nShortest distance: %f\nFound in tour: %d\n\n",FinalResult.ant,FinalResult.distance, FinalResult.iteration );

}

void initializePherormones(){
    for(int i=0; i<NUM_CITIES; i++){
        for(int j=0; j<NUM_CITIES; j++){
            if(i<j){
                pherormone[i][j]=INITIAL_PHERORMONE;
            }
        }
    }
}

double cityScore(int city1, int city2){
    double score, cityTemp;

    if(city1>city2){
        swap(&city1,&city2);
    }
    #pragma omp critical
    score = pherormone[city1][city2]/twoCitiesDistance(city1, city2);
    return score;
}



void swap(int* city1, int* city2){
    int temp;

    temp=*city1;
    *city1=*city2;
    *city2=temp;
}

void clearAntMask(int ant){
    for(int i=0; i<NUM_CITIES; i++){
        cityTaken[ant][i]=0;
    }
}

//
