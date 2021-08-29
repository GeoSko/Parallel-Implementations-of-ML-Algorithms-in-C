
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define INPUT_SIZE 12                           //Size of the input Vector
#define LEVEL_1_NEURONS 100                     //Number of Level 1 Neurons
#define LEVEL_2_NEURONS 10                      //Number of Level 2 Neurons


#define NUM_OF_ITERATIONS 1000                  //The number of training epochs

#define A 0.1                                   //Learning Rate

double INPUT[INPUT_SIZE+1];                     //Input Vector
double DESIRED_OUTPUT[LEVEL_2_NEURONS];         //Desired Output Vector
double WL1[LEVEL_1_NEURONS][INPUT_SIZE+1];      //Level 1 Neuron Synapses
double WL2[LEVEL_2_NEURONS][LEVEL_1_NEURONS+1]; //Level 2 Neuron Synapses
double DL1[LEVEL_1_NEURONS];                    //Level 1 Internal Neuron State
double DL2[LEVEL_2_NEURONS];                    //Level 2 Internal Neuron State
double OL1[LEVEL_1_NEURONS+1];                  //Level 1 Neuron Outputs
double OL2[LEVEL_2_NEURONS];                    //Level 2 Neuron Outputs
double D2[LEVEL_2_NEURONS];                     //Delta array for final layer
double D1[LEVEL_1_NEURONS];                     //Delta array for first layer

void initializeInputOutput();                   //Function that initializes the Input Vector
void initializeNN();                            //Function that initializes the Neural Network Weights
void activateNN( double *Vector);                              //Activates the Neural Network
void activateNeuron();                          //Output of Neuron
void backwordPass();                            //Perform Back Propagation
double sigmoid(double x);                       //Returns the value of the sigmoid Function
double mse(double *a, double *b, int size);   //Calculates the Mean Squared Error
void printOutputs();                            //Print Outputs and Desired Outputs

int main(){

    initializeInputOutput();
    initializeNN();
    // printf("Error\n\nIteration\tMSE\n");
    double t = omp_get_wtime();
    for(int i=0; i<NUM_OF_ITERATIONS; i++){
        activateNN(INPUT);
        // printf("%d\t\t%f:\n", i+1, mse(OL2,DESIRED_OUTPUT,LEVEL_2_NEURONS) );
        backwordPass();
    }
    t = omp_get_wtime() -t;
    printf("Training for %d epochs took total %f seconds to execute \n",NUM_OF_ITERATIONS, t);
    printf("Average time per epoch %f seconds\n",t/NUM_OF_ITERATIONS );
    activateNN(INPUT);
    // printOutputs();
    printf("Final MSE %f:\n", mse(OL2,DESIRED_OUTPUT,LEVEL_2_NEURONS) );

    return 0;
}

void initializeInputOutput(){
    INPUT[0]=1;
    for(int i=1; i<INPUT_SIZE+1; i++){
        INPUT[i]= (float)rand()/(float)(RAND_MAX/2)-1 ;
    }

    for(int i=0; i<LEVEL_2_NEURONS; i++){
        DESIRED_OUTPUT[i]= (float)rand()/(float)(RAND_MAX);
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

    // printf("\n\n**** Neural Network Activation ****\n\n");
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
        OL1[i]=sigmoid(DL1[i-1]);
    }

    //Calculate Level 2 Internal States
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        for(int j=0; j<LEVEL_1_NEURONS+1; j++){
            DL2[i]+= OL1[j]*WL2[i][j];
        }
    }

    //Calculate Level 2 Outputs
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        // OL2[i]=1.0/(1.0+exp(-DL2[i]));
        OL2[i]= sigmoid(DL2[i]);
    }

}

void backwordPass(){
    //Calculate Level 2 Deltas
    // #pragma omp parallel for
    for(int i=0; i<LEVEL_2_NEURONS; i++){   //This loop has few iterations, parallelization here increases the time
        D2[i]=OL2[i]-DESIRED_OUTPUT[i];
    }

    //Update Level 2 Weights
    #pragma omp parallel for
    for(int i=0; i<LEVEL_2_NEURONS; i++){   //This loops have few iterations, parallelization here increases the time
        for(int j=0; j<LEVEL_1_NEURONS+1; j++){
            WL2[i][j]+=-A*D2[i]*OL1[j];
        }
    }

    //Calculate Level 1 Deltas
    for(int i=0; i<LEVEL_1_NEURONS; i++){   //Set Delta Values to Zero(in order to Calculate the sum)
        D1[i]=0;                            //This loops have few iterations, parallelization here increases the time
    }

    #pragma omp parallel for
    for(int i=0; i<LEVEL_1_NEURONS; i++){
        for(int j=0; j<LEVEL_2_NEURONS; j++){
            for(int k=0; k<LEVEL_1_NEURONS+1; k++){
                D1[i]+= D2[j] * WL2[j][k];
            }
        }
        D1[i]*=sigmoid(DL1[i])*(1-sigmoid(DL1[i]));
    }

    //Update Level 1 Weights
    // #pragma omp parallel for
    for(int i=0; i<LEVEL_1_NEURONS; i++){
        for(int j=0; j<INPUT_SIZE+1; j++){
            WL1[i][j]+=-A*D1[i]*INPUT[j];
        }
    }
}

double sigmoid(double x){
    return 1.0/(1.0+exp(-x));
}

double mse(double *a, double *b, int size) {
	double error = 0;
	for (int i = 0; i < size; i++) {
		error += (b[i] - a[i])*(b[i] - a[i]);
	}
	return error / size;
}

void printOutputs(){
    printf("\nNetwork Output\tDesirted Output\n" );
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        printf("%f\t%f\n",OL2[i],DESIRED_OUTPUT[i] );
    }
}
