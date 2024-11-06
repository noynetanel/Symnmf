#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "symnmf.h"

#define BETA 0.5
#define EPSILON 0.0001
#define MAX_ITER 300
#define MAX_LINE_SIZE 1024

int n;
int k;
int d;
int dim;
double** X;
double** W;
double** D;
double** A;
double** H;

double** allocateMatrix(int rows, int cols);

/* Computes the squared Euclidean distance between two points. */
double squaredEuclideanDistance(double *p, double *q, int d) {
    int i;
    double squared_sum = 0;
    for (i = 0; i < d; i++) {
        squared_sum += pow((p[i] - q[i]), 2);
    }
    return squared_sum;
}

/* Displays a matrix with specified rows and columns. */
static void displayMatrix(double** matrix, int rows, int cols) {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            printf("%.4f", matrix[row][col]);
            if (col < cols - 1) {
                printf(",");
            } else {
                printf("\n");
            }
        }
    }
}

/* Counts the number of columns in a file. */
int countColumns(FILE *file) {
    int col_count = 1;
    int ch;
    
    while ((ch = fgetc(file)) != '\n' && ch != EOF) {
        if (ch == ',') {
            col_count++;
        }
    }
    rewind(file);
    return col_count;
}

/* Counts the number of rows in a file. */
int countRows(FILE *file) {
    int row_count = 0;
    char *buffer = NULL;
    size_t buffer_size = 0;

    while (getline(&buffer, &buffer_size, file) != -1) {
        row_count++;
    }
    free(buffer);
    rewind(file);
    return row_count;
}

/* Calculate and output the similarity matrix */
double** symC(double** X) {
    double *b;
    int i, j;
    b = calloc(n * n, sizeof(double));
    if (b == NULL) {
        printf("An Error Has Occurred");
        exit(1);
    }
    A = calloc(n, sizeof(double*));
    if (A == NULL) {
        printf("An Error Has Occurred");
        free(b);
        exit(1);
    }
    for (i = 0; i < n; i++) {
        A[i] = b + i * n;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i != j) {
                A[i][j] = exp(-0.5 * squaredEuclideanDistance(X[i], X[j], d));
            }
        }
    }
    return A;
}

/* Calculate the diagonal degree matrix */
double** ddgC(double** X) {
    double* b;
    double** D;
    int i, j;
    double d_i = 0.0;

    b = calloc(n * n, sizeof(double));
    if (b == NULL) {
        printf("An Error Has Occurred");
        exit(1);
    }
    D = calloc(n, sizeof(double*));
    if (D == NULL) {
        printf("An Error Has Occurred");
        free(b);
        exit(1);
    }

    A = symC(X);  /* Calculate the similarity matrix */

    for (i = 0; i < n; i++) {
        D[i] = b + i * n;
    }
    for (i = 0; i < n; i++) {
        d_i = 0.0;
        for (j = 0; j < n; j++) {
            d_i += A[i][j];
        }
        D[i][i] = d_i;
    }

    return D;
}

/* Calculate the normalized similarity matrix */
double** normC(double** X) {
    int row, col;
    W = allocateMatrix(n, n);  
    D = ddgC(X);  

    for (row = 0; row < n; row++) {
        for (col = 0; col < n; col++) {
            if (D[row][row] != 0 && D[col][col] != 0) {
                W[row][col] = A[row][col] * (1 / sqrt(D[row][row])) * (1 / sqrt(D[col][col]));
            } else {
                W[row][col] = 0.0;
            }
        }
    }

    return W;
}

/* Allocates memory for a matrix with contiguous storage */
double** allocateMatrix(int rows, int cols) {
    int i;
    double *data = calloc(rows * cols, sizeof(double));
    double **matrix = calloc(rows, sizeof(double*));
    if (!data || !matrix) {
        printf("An Error Has Occurred");
        free(data);
        free(matrix);
        exit(1);
    }
    for (i = 0; i < rows; i++) {
        matrix[i] = data + i * cols;
    }
    return matrix;
}

/* Free memory for a matrix */
void freeMatrix(double **matrix) {
    if(matrix)
        return;
    if(!matrix[0])
        free(matrix[0]);
    if(!matrix)
        free(matrix);
}

/* Release memory allocated for a 2D matrix */
void releaseMatrix(double **mat) {
    if (mat)  
        return;
    if (!mat[0])  
        free(mat[0]);  
    free(!mat)
        free(mat);
}


/* Loads data points from a file and stores them in the global matrix X */
static double** readPointsFromFile(const char* fileName) {
    int i, j, r;
    FILE *file = fopen(fileName, "r");
    char *line = NULL;
    char *start, *end;
    size_t len = 0;
    if (!file) return NULL;
    d = countColumns(file);
    n = countRows(file);
    X = (double**)malloc(n * sizeof(double*));
    if (!X) { fclose(file); return NULL; }
    for (i = 0; i < n; i++) {
        X[i] = (double*)malloc(d * sizeof(double));
        if (!X[i]) {
            for (j = 0; j < i; j++) free(X[j]);
            free(X);
            fclose(file);
            return NULL;
        }
    }
    for (i = 0; i < n; i++) {
        if (getline(&line, &len, file) == -1) break;
        start = line;
        for (j = 0; j < d; j++) {
            X[i][j] = strtod(start, &end);
            if (start == end) {
                for (r = 0; r <= i; r++) free(X[r]);
                free(X);
                free(line);
                fclose(file);
                return NULL; }
            start = end;
            while (*start == ',') start++;
        }
    }
    free(line);
    fclose(file);
    return X; }

/*
* A <- B where A and B are matrices of same size j* l
*
* copyTo - the matrix to be overrided 
* copyFrom - the overrides matrix 
* rows - size j
* cols - size l
* returns - 0 if succesful 
*/
int copy_matrix(double **copyTo, double **copyFrom, int rows, int cols){
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            /* a[i][j] <- B[i][j] */
            copyTo[i][j] = copyFrom[i][j];
        }
    }
    return 0;
}

/* assigns the transpose matrix of matrix to a given matrix "transposedMatrix" of size l*m
* rows - size l
* cols - size m
* transposedMatrix - the matrix assigned values to
*/
void transposeMatrix(int rows, int cols, double** matrix, double*** transposedMatrix) {
    int i, j;
    /* aij = aji */
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            (*transposedMatrix)[j][i] = matrix[i][j];
        }
    }
}

/*multiply two matrices by using the given two matrices and their sizes A*B =  ΣΣ(a*b) 
* firstMatrix - matrix A of dimension l * m
* secondMatrix - matrix B of dimension m * n
* rows_matrix1 - the size l
* cols_matrix_1 - the size m
* cols_matrix2 - the size n
*returns - a l * n matrix of doubles
*/
double** multiplyMatrices(double** firstMatrix, double** secondMatrix, int rows_matrix1, int cols_matrix1, int cols_matrix2) {
    int i, j, m;
    double** resultMatrix;
    double* p = calloc(rows_matrix1 * cols_matrix2, sizeof(double));
    /* error handling */
    if (p == NULL) {
        printf("An Error Has Occurred");
        exit(1);
    }
    resultMatrix = calloc(rows_matrix1, sizeof(double *));
    if (resultMatrix == NULL) {
        printf("An Error Has Occurred");
        free(p);
        exit(1);
    }
    for (i = 0; i < rows_matrix1; i++)
        resultMatrix[i] = p + i * cols_matrix2;
    for (i = 0; i < rows_matrix1; i++) {
        for (j = 0; j < cols_matrix2; j++) {
            for (m = 0; m < cols_matrix1; m++) {
                resultMatrix[i][j] += firstMatrix[i][m] * secondMatrix[m][j];
            }
        }
    }

    return resultMatrix;
}


/* Computes the squared Frobenius norm between two matrices
 * matA - the first matrix
 * matB - the second matrix
 * Returns - the squared Frobenius norm between matA and matB
 */
double calculateSquaredFrobeniusNorm(double** matA, double** matB) {
    int row, col;
    double sumDifferenceSquared = 0;

    for (row = 0; row < n; row++) {
        for (col = 0; col < k; col++) {
            double diff = matA[row][col] - matB[row][col];
            sumDifferenceSquared += diff * diff;
        }
    }

    return sumDifferenceSquared;
}


/* UpdateHWithSymmetricFactorization - updates matrix H based on iterative SymNMF algorithm
 * new_H - matrix to be updated
 * base_H - initial H matrix */
void UpdateH(double ***new_H, double **base_H) {
    int i, j;
    double **H_intermediate = allocateMatrix(k, n);
    double **H_mult_Ht, **H_mult_HtH, **W_mult_H;

    freeMatrix(*new_H);
    *new_H = allocateMatrix(n, k);

    W_mult_H = multiplyMatrices(W, base_H, n, n, k);
    transposeMatrix(n, k, base_H, &H_intermediate);
    H_mult_Ht = multiplyMatrices(base_H, H_intermediate, n, k, n);
    H_mult_HtH = multiplyMatrices(H_mult_Ht, base_H, n, n, k);

    for (i = 0; i < n; i++) {
        for (j = 0; j < k; j++) {
            if (H_mult_HtH[i][j] == 0) {
                freeMatrix(*new_H);
                freeMatrix(W_mult_H);
                freeMatrix(H_intermediate);
                freeMatrix(H_mult_Ht);
                freeMatrix(H_mult_HtH);
                printf("An Error Has Occurred");
                exit(1);
            }
            (*new_H)[i][j] = base_H[i][j] * (1 - BETA + BETA * W_mult_H[i][j] / H_mult_HtH[i][j]);
        }
    }

    freeMatrix(W_mult_H);
    freeMatrix(H_intermediate);
    freeMatrix(H_mult_Ht);
    freeMatrix(H_mult_HtH);
}



/* Perform the Symmetric Nonnegative Matrix Factorization (SymNMF) with iterative updates
 * prev_H - initial H matrix
 * Returns - the final H matrix with clustering information
 */
double** symnmfC(double **prev_H) {
    int i;
    double **curr_H;
    double *p = calloc(n * k, sizeof(double));
    if (p == NULL) {
        printf("An Error Has Occurred");
        exit(1);
    }
    curr_H = calloc(n, sizeof(double *));
    if (curr_H == NULL) {
        printf("An Error Has Occurred");
        free(p);
        exit(1);
    }
    for (i = 0; i < n; i++) {
        curr_H[i] = p + i * k;
    }
    /* Iteratively update H */
    for (i = 0; i < MAX_ITER; i++) {
        UpdateH(&curr_H, prev_H);
        
        if (calculateSquaredFrobeniusNorm(prev_H, curr_H) < EPSILON) {
            break;
        }

        copy_matrix(prev_H, curr_H, n, k);
    }
    return curr_H;
}

int main(int argc, char* argv[]) {
    const char* goal;
    const char* filename;
    double** result;
    if (argc != 3) {
        printf("An Error Has Occurred\n");
        return 1; }
    goal = argv[1];
    filename = argv[2];
    X = readPointsFromFile(filename);
    if (!X) {
        printf("An Error Has Occurred");
        exit(1); }
    if (strcmp(goal, "sym") == 0) {
        result = symC(X);
        displayMatrix(result, n, n);
        freeMatrix(result); }  
    else if (strcmp(goal, "ddg") == 0) { 
        result = ddgC(X);
        displayMatrix(result, n, n);
        freeMatrix(A);
        freeMatrix(result); }
    else if (strcmp(goal, "norm") == 0) {
        result = normC(X);
        displayMatrix(result, n, n);
        freeMatrix(result);
        freeMatrix(D);
        freeMatrix(A); }
    else {
        printf("An Error Has Occurred");
        freeMatrix(X);
        return 1;
    }
    freeMatrix(X);
    return 0;
}


