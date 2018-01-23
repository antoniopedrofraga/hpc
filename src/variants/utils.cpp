#include "utils.h"

double *** alloc3d(int l, int m, int n) {
    double *data = new double [l*m*n];
    double ***array = new double **[l];
    for (int i=0; i<l; i++) {
        array[i] = new double *[m];
        for (int j=0; j<m; j++) {
            array[i][j] = &(data[(i*m+j)*n]);
        }
    }
    return array;
}

double ** alloc2d(int n, int m) {
    double *data = new double [m*n];
    double **array = new double *[n];
    for (int i=0; i<n; i++) {
        array[i] = &(data[i*m]);
    }
    return array;
}

double * gaussian_elimination(double ** mat, size_t n) {
    double * x = malloc(sizeof(double) * n); 

    for (int i = n - 1; i >= 0; i--) {
        x[i] = mat[i][n];
        for (size_t j=i+1; j<n; j++) {
            x[i] -= mat[i][j] * x[j];
        }
 
        x[i] = x[i] / mat[i][i];
    }
    
    return x;
}
