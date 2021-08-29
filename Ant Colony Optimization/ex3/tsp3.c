 #include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define NUM_CITIES 10000
#define GRID_SIZE 1000

// #define NUM_CITIES 5
// #define GRID_SIZE 5


void createCities();                    //Initialization of Cities
double calculateDistance();              //Returns the distance of the Current Route
void printCities();                     //Prints the list of cities and their coordinates
void printRoute();                      //Print the current Route
int closerCity( int city);              //Returns the city closer to city parameter
double twoCitiesDistance(int city1, int city2); //Calculates the euclidean distance between 2 cities
void printMask();                       //Function for debugging


int Cities[NUM_CITIES][2];
int Route[NUM_CITIES];
int cityTaken[NUM_CITIES]={0};  //Mask array with one in taken cities positions

int main(){
    int i;
    double distance;


    printf("Number of cities: %d\nGrid: %dX%d\n\n", NUM_CITIES,GRID_SIZE,GRID_SIZE );
    createCities();
    // printCities();


    Route[0]=0;     //We begin from the first city
    // printRoute();
    cityTaken[0] =1;   //City zero is chosen in the route
    for(i=1; i<NUM_CITIES; i++){
        Route[i] = closerCity(Route[i-1]);  // The current city is the closest to the previous
        // printRoute();

    }

    distance = calculateDistance();

    printf("Final Distance:%f\n", distance );


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

int closerCity(int city){
    int i,closerCity;
    double minDistance=DBL_MAX;
    double tempDistance;

    for(i=0; i<NUM_CITIES; i++){
        if(cityTaken[i]==0 && i!=city ){    //Check if city is not already in route and is not the se ma to the last one
            // printf("city:%d i:%d \n",city,i );
            tempDistance = twoCitiesDistance(city,i);
            if( tempDistance < minDistance ){
                minDistance = tempDistance;
                closerCity = i;
            }
        }
    }
    // printf("closer city:%d\n", closerCity );
    cityTaken[closerCity]= 1;   //
    // printMask();
    return closerCity;
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
    for(int i=0; i<NUM_CITIES; i++){
        printf("%d ",cityTaken[i] );
    }
    putchar('\n');
}

//
