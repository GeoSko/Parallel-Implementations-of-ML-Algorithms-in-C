
/*
Neural Network for image Classification using C and OpenMP
Dataset: Fashion MNist
Autor: Skondras Georgios (CEID UPatras)
Test Set Accuracy achieved:65%
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>



//**********Neural Network Parameters**********
#define NUM_OF_ITERATIONS 33
#define INPUT_SIZE 784                         //Size of the input Vector
#define NUM_OF_TRAIN_INPUTS 60000
#define NUM_OF_TEST_INPUTS 10000
#define LEVEL_1_NEURONS 100                     //Number of Level 1 Neurons
#define LEVEL_2_NEURONS 10                      //Number of Level 2 Neurons
#define A 0.0001

#define NUM_OF_TRAIN_INPUTS 60000                                //Learning Rate

//**********File Handling Parameters**********
#define BUFFER_SIZE 8000
#define LINE_LEN 784


double INPUT[NUM_OF_TRAIN_INPUTS][INPUT_SIZE+1];                    //Input Vectors for training
double TEST_INPUT[NUM_OF_TEST_INPUTS][INPUT_SIZE+1];                //Input Vectors for testing
double DESIRED_OUTPUT[NUM_OF_TRAIN_INPUTS][LEVEL_2_NEURONS];        //Desired Output Vectors
double DESIRED_OUTPUT_TEST[NUM_OF_TEST_INPUTS][LEVEL_2_NEURONS];    //Desired Output Vector

double WL1[LEVEL_1_NEURONS][INPUT_SIZE+1];      //Level 1 Neuron Synapses
double WL2[LEVEL_2_NEURONS][LEVEL_1_NEURONS+1]; //Level 2 Neuron Synapses
double DL1[LEVEL_1_NEURONS];                    //Level 1 Internal Neuron State
double DL2[LEVEL_2_NEURONS];                    //Level 2 Internal Neuron State
double OL1[LEVEL_1_NEURONS+1];                  //Level 1 Neuron Outputs
double OL2[LEVEL_2_NEURONS];                    //Level 2 Neuron Outputs
double D2[LEVEL_2_NEURONS];                     //Delta array for final layer
double D1[LEVEL_1_NEURONS];                     //Delta array for first layer
int TRAIN_LABELS[NUM_OF_TRAIN_INPUTS];
int TEST_LABELS[NUM_OF_TRAIN_INPUTS];


void loadTrainingData();                                        //Function that loads the Training Data
void loadTestData();                                            //Function that loads the Training Data
void initializeNN();                                            //Function that initializes the Neural Network Weights with double values in [-1,1]
void activateNN(double *inputVector);                           //Activates the Neural Network
void backwordPass(double *inputVector, double *outputVector);   //Perform Back Propagation
double sigmoid(double x);                                       //Returns the value of the sigmoid Function
double mse(double *a, double *b, int size);                     //Calculates the Mean Squared Error
void printOutputs();                                            //Print Outputs and Desired Outputs
void printInputsDesiredOutputs();                               //Auxiliary function for debugging that prints inputs and outputs
int classDecision(double *output);
int desiredDecision(int input,int train_flag);
int decisionTruth(int output, int desired);
void printData();

int main(){

    int category;
    int desiredCategory;
    int categoryTruth;
    double accuracy;
    int trueDecisioncounter=0;
    loadTrainingData();
    // printInputsDesiredOutputs();
    initializeNN();
    // printData();
    int train_flag=1;
    printf("Beginning Training\n" );
    double t = omp_get_wtime();
    for(int i = 0; i<NUM_OF_ITERATIONS; i++){           //Loop for the training
        trueDecisioncounter=0;
        for(int j=0; j<NUM_OF_TRAIN_INPUTS; j++){
            activateNN(INPUT[j]);
            category = classDecision(OL2);
            desiredCategory=desiredDecision(j,train_flag);
            categoryTruth=  decisionTruth(category,desiredCategory);
            if (categoryTruth){
                trueDecisioncounter++;
            }
            backwordPass(INPUT[j],DESIRED_OUTPUT[j]);
        }
        accuracy = ((double)trueDecisioncounter*100)/NUM_OF_TRAIN_INPUTS;
        printf("Epoch: %d accuracy %.5f%%\n",i+1, accuracy );
    }
    t = omp_get_wtime() -t;

    trueDecisioncounter=0;
    for(int j=0; j<NUM_OF_TRAIN_INPUTS; j++){      //Loop for calculating Training accuracy
        activateNN(INPUT[j]);
        category = classDecision(OL2);
        desiredCategory=desiredDecision(j,train_flag);
        categoryTruth=  decisionTruth(category,desiredCategory);
        if (categoryTruth){
            trueDecisioncounter++;
        }
    }


    printf("\nTotal Training for %d images\nEpochs: %d\nTraining time: %f sec\n",NUM_OF_TRAIN_INPUTS,NUM_OF_ITERATIONS, t);
    printf("Training time per epoch: %f seconds\n",t/NUM_OF_ITERATIONS );
    accuracy = ((double)trueDecisioncounter*100)/NUM_OF_TRAIN_INPUTS;
    printf("\nFinal Training Accuracy: %.5f%% \n\n",accuracy );

    train_flag=0;
    trueDecisioncounter=0;

    loadTestData();
    printf("Testing for 10000 images\n" );          //Loop for Testing and calculating testing accuracy
    for(int j=0; j<NUM_OF_TEST_INPUTS; j++){
        activateNN(TEST_INPUT[j]);
        category = classDecision(OL2);
        desiredCategory=desiredDecision(j,train_flag);
        categoryTruth=  decisionTruth(category,desiredCategory);
        if (categoryTruth){
            trueDecisioncounter++;
        }
    }

    accuracy = ((double)trueDecisioncounter*100)/NUM_OF_TEST_INPUTS;
    printf("Final Test Accuracy: %.5f%% \n",accuracy );


    return 0;
}

void loadTrainingData(){

    FILE* fp = fopen("fashion-mnist_train.csv", "r");

	if (!fp)
		printf("Can't open file\n");
	else {
		char buffer[BUFFER_SIZE];
		int row = 0;
		int column = 0;
        int realRow;
		while (fgets(buffer, BUFFER_SIZE, fp)) {
			column = 0;
			row++;
			if (row == 1)
				continue;
			char* value = strtok(buffer, ", ");			// Splitting the data
			while (value) {
                if(column==0){
                    // printf("%d ",atoi(value));
                    realRow=row-2;
                    TRAIN_LABELS[realRow]=atoi(value);
                    DESIRED_OUTPUT[realRow][atoi(value)]=1;
                    INPUT[realRow][column]=1;        //Set the bias to 1
                }else{
                    INPUT[realRow][column] = atoi(value);
                }
				value = strtok(NULL, ", ");
				column++;
			}
		}
		fclose(fp);
	}

}

void loadTestData(){
    FILE* fp = fopen("fashion-mnist_test.csv", "r");

	if (!fp)
		printf("Can't open file\n");
	else {
		char buffer[BUFFER_SIZE];
		int row = 0;
		int column = 0;
        int realRow;
		while (fgets(buffer, BUFFER_SIZE, fp)) {
			column = 0;
			row++;
			if (row == 1)
				continue;
			char* value = strtok(buffer, ", ");			// Splitting the data
			while (value) {
                if(column==0){
                    // printf("%d ",atoi(value));
                    realRow=row-2;
                    TEST_LABELS[realRow]=atoi(value);
                    DESIRED_OUTPUT_TEST[realRow][atoi(value)]=1;
                    TEST_INPUT[realRow][column]=1;        //Set the bias to 1
                }else{
                    TEST_INPUT[realRow][column] = atoi(value);
                }
				value = strtok(NULL, ", ");
				column++;
			}
		}
		fclose(fp);
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

void activateNN(double *inputVector){

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
            DL1[i]+= inputVector[j]*WL1[i][j];
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
        OL2[i]= sigmoid(DL2[i]);
    }

}

void backwordPass(double *inputVector, double *outputVector){
    //Calculate Level 2 Deltas
    for(int i=0; i<LEVEL_2_NEURONS; i++){   //This loop has few iterations, parallelization here increases the time
        D2[i]=OL2[i]-outputVector[i];
    }

    //Update Level 2 Weights
    for(int i=0; i<LEVEL_2_NEURONS; i++){   //This loops have few iterations, parallelization here increases the time
        for(int j=0; j<LEVEL_1_NEURONS+1; j++){
            WL2[i][j]+=-A*D2[i]*OL1[j];
        }
    }

    //Calculate Level 1 Deltas

    #pragma omp parallel for
    for(int i=0; i<LEVEL_1_NEURONS; i++){
        D1[i]=0;    //Set Delta Values to Zero(in order to Calculate the sum)
        for(int j=0; j<LEVEL_2_NEURONS; j++){
            for(int k=0; k<LEVEL_1_NEURONS+1; k++){
                D1[i]+= D2[j] * WL2[j][k];
            }
        }
        D1[i]*=sigmoid(DL1[i])*(1-sigmoid(DL1[i]));
    }

    //Update Level 1 Weights
    for(int i=0; i<LEVEL_1_NEURONS; i++){
        for(int j=0; j<INPUT_SIZE+1; j++){
            WL1[i][j]+=-A*D1[i]*inputVector[j];
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
        printf("%f\t%f\n",OL2[i],DESIRED_OUTPUT[0][i] );
    }
}

void printInputsDesiredOutputs(){
    printf("\nInput Vectors\t\t\t\t\tDesired Outputs\n\n");
    for(int i=0; i<NUM_OF_TRAIN_INPUTS; i++){
        printf("Vector %d\n",i );
        for(int j=0; j<INPUT_SIZE+1; j++){
            printf("%f ",INPUT[i][j] );
        }

        printf("\t------->\t");
        for(int j=0; j<LEVEL_2_NEURONS; j++)
            printf("%f ",DESIRED_OUTPUT[i][j]);
            printf("\n\n");
    }
}

int classDecision(double *output){
    double max = -1;
    int class;
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        // printf("%f ", OL2[i] );
        if(OL2[i]>max){
            max=OL2[i];
            class=i;
        }
    }
    // putchar('\n');
    // printf("%d ",class );

    return class;
}

int desiredDecision(int input, int train_flag){
    for(int i=0; i<LEVEL_2_NEURONS; i++){
        // printf("%f ", DESIRED_OUTPUT[input][i] );
        if(train_flag){
            if(DESIRED_OUTPUT[input][i]>0.7){
                // printf("%d\n",i);
                return i;
            }

        }else{
            if(DESIRED_OUTPUT_TEST[input][i]>0.7){
                // printf("%d\n",i);
                return i;
            }
        }

    }

}

int decisionTruth(int realDecision,int desired){
    if(realDecision==desired){
        return 1;
    }
    return 0;
}

void printData(){
    printf("\nPrinting Data\n\n");
    printf("Labels\n");
    for(int i=0; i<NUM_OF_TRAIN_INPUTS; i++){
        printf("%d ",TRAIN_LABELS[i]);
    }
    printf("\n\n");

    for(int i=59997; i<60000; i++ ){ //NUM_OF_TRAIN_INPUTS
        printf("i:%d \n", i );
        for(int j=0; j<LEVEL_2_NEURONS; j++ ){
            printf("%f ", DESIRED_OUTPUT[i][j] );
        }
        printf("\n\n" );
    }

    double INPUT[NUM_OF_TRAIN_INPUTS][INPUT_SIZE+1];
    printf("Input Train\n");
    for(int i=59997; i<60000; i++){
        printf("i:%d~ ",i);
        for(int j=0; j<INPUT_SIZE+1; j++){
            printf("%f ",INPUT[i][j]);
        }
        printf("\n\n" );;
    }
    printf("\n\n");

}

//
