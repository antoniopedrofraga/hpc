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

void swap_row(double ** &mat, size_t n, int i, int j) {

    for (size_t k = 0; k <= n; k++) {
        double temp = mat[i][k];
        mat[i][k] = mat[j][k];
        mat[j][k] = temp;
    }
}


void forward_elim(double ** &mat, size_t n) {
    for (size_t k = 0; k < n; k++) {

        size_t i_max = k;
        int v_max = mat[i_max][k];

        for (size_t i = k + 1; i < n; i++) {
            if (abs(mat[i][k]) > v_max) {
                v_max = mat[i][k], i_max = i;
            }
        }


        if (!mat[k][i_max])
            return;

        if (i_max != k)
            swap_row(mat, n, k, i_max);


        for (size_t i = k + 1; i < n; i++) {
            double f = mat[i][k] / mat[k][k];

            for (size_t j = k + 1; j <= n; j++) {
                mat[i][j] -= mat[k][j] * f;
            }

            mat[i][k] = 0;
        }
    }
    return;
}


double * gaussian_elimination(double ** &a, double * &b, size_t n) {
    double ** mat = alloc2d(n, n + 1);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j <= n; j++) {
            mat[i][j] = j >= n ? b[i] : a[i][j];
        }
    }

    forward_elim(mat, n);

    double * x = (double*) malloc(sizeof(double) * n); 

    for (int i = n - 1; i >= 0; i--) {
        x[i] = mat[i][n];
        for (size_t j = i + 1; j < n; j++) {
            x[i] -= mat[i][j] * x[j];
        }

        x[i] = x[i] / mat[i][i];
    }

    return x;
}
