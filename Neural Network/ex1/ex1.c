#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// #define INPUT_SIZE 12
// #define LEVEL_1_NEURONS 100
// #define LEVEL_2_NEURONS 10

#define INPUT_SIZE 2
#define LEVEL_1_NEURONS 2
#define LEVEL_2_NEURONS 2

double INPUT[INPUT_SIZE+1];                     //Input Vector


double WL1[LEVEL_1_NEURONS][INPUT_SIZE+1];      //Level 1 Neuron Synapses
double WL2[LEVEL_2_NEURONS][LEVEL_1_NEURONS+1]; //Level 2 Neuron Synapses

double DL1[LEVEL_1_NEURONS];                    //Level 1 Internal Neuron State
double DL2[LEVEL_2_NEURONS];                    //Level 2 Internal Neuron State

double OL1[LEVEL_1_NEURONS+1];                    //Level 1 Neuron Outputs
double OL2[LEVEL_2_NEURONS];                    //Level 2 Neuron Outputs

void printNNInfo();                             //Function to print Neural Network Info(Inputs and Outputs)
void printWeights();                            //Function to print all the weights of the Neural Network
void initializeInput();                         //Function that initializes the Input Vector
void initializeNN();                            //Function that initializes the Neural Network Weights
void activateNN(double *Vector);                              //Activates the Neural Network
void activateNeuron();                          //Output of Neuron

int main(){

    initializeInput();
    initializeNN();
    printWeights();

    activateNN(INPUT);
    printNNInfo();
    return 0;
}

void printNNInfo(){
    printf("Input vector\n\n");
    for(int i=0; i<INPUT_SIZE+1; i++){
        printf("X%d: %f\n",i, INPUT[i]);
    }
    putchar('\n');


    printf("Internal Neuron State\n\n");
    printf("Level 1 Internal State\n");
    for(int i=0; i<LEVEL_1_NEURONS; i++){
        printf("Neuron %d: %f\n", i, DL1[i]);
    }
    putchar('\n');
    printf("Level 2 Internal State\n");
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        printf("Neuron %d: %f\n", i,DL2[i]);
    }

    printf("Neuron Outputs\n\n");
    printf("Level 1 Outputs\n");
    for(int i=0; i<LEVEL_1_NEURONS+1; i++){
        printf("Neuron %d: %.20f\n", i,OL1[i]);
    }
    putchar('\n');
    printf("Level 2 Outputs\n");
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        printf("Neuron %d: %.20f\n", i,OL2[i]);
    }
}

void printWeights(){
    printf("Level 1 weights\n\n");
    for(int i=0; i<LEVEL_1_NEURONS; i++){
        printf("Neuron %d\n", i );
        for(int j=0; j<INPUT_SIZE+1; j++){
            printf("W%dX%d: %f\n",i,j,WL1[i][j] );
        }
        putchar('\n');
    }

    printf("Level 2 weights\n\n");
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        printf("Neuron %d\n", i );
        for(int j=0; j<LEVEL_1_NEURONS+1; j++){
            printf("W%dX%d: %f\n",i,j,WL2[i][j] );
        }
        putchar('\n');
    }
}

void initializeInput(){
    INPUT[0]=1;
    for(int i=1; i<INPUT_SIZE+1; i++){
        INPUT[i]= (float)rand()/(float)(RAND_MAX/2)-1 ;
    }
}

void initializeNN(){

    // Set bias weights to 1
    for(int i=0; i<LEVEL_1_NEURONS; i++){
        WL1[i][0]=1;
    }
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        WL2[i][0]=1;
    }

    for(int i=0; i<LEVEL_1_NEURONS; i++){   //Set values to level 1 weights
        for(int j=1; j<INPUT_SIZE+1; j++){
            WL1[i][j]=(float)rand()/(float)(RAND_MAX/2)-1;
        }
    }

    for(int i=0; i<LEVEL_2_NEURONS; i++){   //Set values to level 2 weights
        for(int j=1; j<LEVEL_1_NEURONS+1; j++){
            WL2[i][j]=(float)rand()/(float)(RAND_MAX/2)-1;
        }
    }

    OL1[0]=1; //Set Level 2 bias to 1

}

void activateNN(double *Vector){

    printf("\n\n**** Neural Network Activation ****\n\n");
    //Clear Previous Internal States
    for(int i=0; i<LEVEL_1_NEURONS; i++){
        DL1[i]=0;
    }
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        DL2[i]=0;
    }

    //Calculate Level 1 Internal States
    for(int i=0; i<LEVEL_1_NEURONS; i++){
        for(int j=0; j<INPUT_SIZE+1; j++){
            DL1[i]+= Vector[j]*WL1[i][j];
        }
    }

    //Calculate Level 1 Outputs
    for(int i=1; i<LEVEL_1_NEURONS+1; i++){     //We don't change the first position of the array(bias=1)
        OL1[i]=1.0/(1.0+exp(-DL1[i-1]));
    }

    //Calculate Level 2 Internal States

    for(int i=0; i<LEVEL_2_NEURONS; i++){
        for(int j=0; j<LEVEL_1_NEURONS+1; j++){
            DL2[i]+= OL1[j]*WL2[i][j];
        }
    }

    //Calculate Level 2 Outputs
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        OL2[i]=1.0/(1.0+exp(-DL2[i]));
    }

}








//
