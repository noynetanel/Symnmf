#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define BETA 0.5
#define EPISLON 1e-4
#define MAX_ITER 300
#define MAX_LINE_SIZE 1024 

/*
 * Computes the squared Euclidean distance between two points.
 */
double euclideanDistance(double *p, double *q, int d)
{   int i;
    double squared_sum = 0;
    for (i = 0; i < d; i++) {
        squared_sum += pow((p[i] - q[i]), 2);
    }
    return sqrt(squared_sum);
}

/*
 * Displays a matrix with specified rows and columns.
 */
static void displayMatrix(double* matrix, int rows, int cols) {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            printf("%.4f", matrix[row * cols + col]);
            if (col < cols - 1) {
                printf(", ");
            } else {
                printf("\n");
            }
        }
    }
}

/*
 * Determines the number of points and the dimensions from the input file.
 */
static int getDimensionsAndCount(FILE *file, int *n, int *d) {
    char line[MAX_LINE_SIZE];
    *n = 0;
    *d = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) > 1) { /* Ignore any blank lines */
            (*n)++;
            if (*d == 0) {
                char* tmp = line;
                while (*tmp) {
                    if (*tmp == ',') {
                        (*d)++;
                    }
                    tmp++;
                }
                (*d)++; /* The number of elements exceeds the count of commas by one */
            }
        }
    }
    
    return (*n > 0 && *d > 0) ? 0 : -1;
}

/*
 * Reads data points from the file into a pre-allocated array.
 */
static int readPointsIntoArray(FILE *file, double* dataPoints, int n, int d) {
    char line[MAX_LINE_SIZE];
    int pointIndex = 0, dimIndex;

    fseek(file, 0, SEEK_SET);
    while (fgets(line, sizeof(line), file) && pointIndex < n) {
        if (strlen(line) > 1) { /* Ignore any blank lines */
            char* token = strtok(line, ",");
            dimIndex = 0;
            while (token != NULL && dimIndex < d) {
                dataPoints[pointIndex * d + dimIndex++] = atof(token);
                token = strtok(NULL, ",");
            }
            pointIndex++;
        }
    }
    return (pointIndex == n) ? 0 : -1;
}

/*
 This function loads data points from a specified file and stores them in a single-dimensional array. 
 It also updates the provided variables to indicate the total number of points and the dimensionality of each point.
 */
static double* readPointsFromFile(const char* fileName, int* n, int* d) {
    FILE *file = fopen(fileName, "r");
    double* dataPoints = NULL;

    if (!file) { 
        return NULL;
    }

    /* Get the number of points and their dimensions */
    if (getDimensionsAndCount(file, n, d) != 0) {
        fclose(file);
        return NULL;
    }

    /* allocate memory for dataPoints array */
    dataPoints = (double*)malloc(*n * *d * sizeof(double));
    if (dataPoints == NULL) {
        fclose(file);
        return NULL;
    }

    /* reads the data points into the allocated array */
    if (readPointsIntoArray(file, dataPoints, *n, *d) != 0) {
        free(dataPoints);
        dataPoints = NULL;
    }

    fclose(file);
    return dataPoints;
}

/*Calculate and output the similarity matrix*/
static double* sym(double* X, int n, int d){
    double *A = (double *)malloc(n * n * sizeof(double));
    int i,j;
    double Aij;
    double * Xi;
    double * Xj;
    if(A == NULL){
        exit(1);
    }
    for(i = 0; i < n; i++){
       for(j = 0; j < n; j++){
        Aij = 0.0;
        if(i!=j){      
          Xi = X + i*d;
          Xj = X + j*d;
          Aij = exp(-0.5*euclideanDistance(Xi, Xj, d));
        }
        A[i*n +j]= Aij; 
       } 
    }
    displayMatrix(A, n, n);
    return A; 
}

// continue from calcDdg!!!!
/////
////// good luck !!!


int main(int argc, char* argv[]) {


}



