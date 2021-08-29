#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_CITIES 10000
#define GRID_SIZE 1000
#define NUM_ITERATIONS 20000

// #define NUM_CITIES 5
// #define GRID_SIZE 5
// #define NUM_ITERATIONS 4

void createCities();                    //Initialization of Cities
void createRoute();                     //Initialization of Routes in the current(random) order
double calculateDistance();              //Returns the distance of the Current Route
void printCities();                     //Prints the list of cities and their coordinates
void printRoute();                      //Print the current Route
void swapCities(int city1, int city2);  //Swaps two random cities


int Cities[NUM_CITIES][2];
int Route[NUM_CITIES];

int main(){
    int i, city1, city2, success, fail;
    double oldDistance,newDistance;

    success=0;
    fail=0;

    printf("Number of cities: %d\nGrid: %dX%d\n\n", NUM_CITIES,GRID_SIZE,GRID_SIZE );
    createCities();
    // printCities();
    createRoute();
    // printRoute();
    newDistance = calculateDistance();
    printf("Initial Distance: %f\n", newDistance );

    for (i = 0; i < NUM_ITERATIONS; i++) {
        oldDistance= calculateDistance();
        do {
            city1=rand()%NUM_CITIES;
            city2=rand()%NUM_CITIES;
        } while(city1==city2);
        swapCities(city1, city2);
        newDistance = calculateDistance();

        if (newDistance>oldDistance) {
            swapCities(city1, city2);
            fail++;
            // printf("Swap failed\n");
        }else{
            success++;
            // printf("Swap succeeded\n");
            // printRoute();
        }

    }
    newDistance = calculateDistance();


    printf("Number of iterations: %d\nNumber of succesful swaps:%d\nNumber of failing swaps:%d\n",NUM_ITERATIONS,success,fail );
    printf("Final Distance: %f\n", newDistance );


    return 0;
}

void createCities(){
    int i,j;

    for(i = 0; i< NUM_CITIES; i++){
        for(j = 0; j<2; j++){
            Cities[i][j] = rand()%GRID_SIZE;
        }
    }
}

void createRoute(){
    int i;

    for (i = 0; i < NUM_CITIES; i++) {
        Route[i]=i;
    }
}

void printCities(){
    int i,j;
    printf("Cities:\n\n");
    for(i=0; i<NUM_CITIES; i++){
        printf("%d: ", i );
        for(j=0; j<2; j++){
            printf("%d ", Cities[i][j] );
        }
        putchar('\n');
    }
    putchar('\n');
}

void printRoute(){
    int i;
    printf("Route:\n\n");
    for (i = 0; i < NUM_CITIES; i++) {
        printf("%d -> ", Route[i]);
    }
    printf("%d\n", Route[0]);
    putchar('\n');

}

double calculateDistance(){
    int i,j;
    double distance=0;
    double tempDistance;

    for(i=0; i<NUM_CITIES-1; i++){
        tempDistance=0;
        for(j=0; j<2; j++){
            tempDistance += ((Cities[Route[i]][j] - Cities[Route[i+1]][j]) * (Cities[Route[i]][j] - Cities[Route[i+1]][j]));
        }
        // printf("%f\n",sqrt(tempDistance) );
        distance+=sqrt(tempDistance);
    }
    tempDistance=0;
    for(j=0; j<2; j++){ //Last to first
        tempDistance += (Cities[Route[NUM_CITIES-1]][j] - Cities[Route[0]][j]) * (Cities[Route[NUM_CITIES-1]][j] - Cities[Route[0]][j]);
    }
    // printf("%f\n",sqrt(tempDistance) );
    distance+=sqrt(tempDistance);

    return distance;
}

void swapCities(int city1, int city2){

    int temp;

    temp = Route[city1];
    Route[city1]=Route[city2];
    Route[city2]=temp;
}


//
