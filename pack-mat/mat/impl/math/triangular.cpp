

/**
 * Single precision Upper Triangular Transform.
 * This function transforms a matrix to an upper triangular form.
 * The calculation is performed by row elimination with single precision.
 * @param m - number of rows
 * @param n - number of columns
 * @param A - pointer to first element of the matrix
 */
void __upper_triangle_f(int m, int n, float * A) {
    int c, r, _r, i, p;
    float P, k;
    int ip;
    int dim;

    dim = (m < n) ? m : n;

    for (p = 0; p < dim; p++) {

        ip = p * n;

        /* Select the PIVOT */
        P = A[ip + p];

        for (r = p + 1; r < m; r++) {
            _r = r * n;

            if (A[ _r + p ] == 0) continue;

            /* Calculate the multiply factor */
            k = (-1) * A[ _r + p ] / P;

            for (c = 0; c < n; c++) {
                i = _r + c;
                A[i] = A[i] + k * A[ ip + c ];
            }
        }
    }
}

/**
 * Double precision Upper Triangular Transform.
 * This function transforms a matrix to an upper triangular form.
 * The calculation is performed by row elimination with double precision.
 * @param m - number of rows
 * @param n - number of columns
 * @param A - pointer to first element of the matrix
 */
void __upper_triangle_d(int m, int n, double * A) {
    int c, r, _r, i, p;
    double P, k;
    int ip;
    int dim;

    dim = (m < n) ? m : n;

    for (p = 0; p < dim; p++) {

        ip = p * n;

        /* Select the PIVOT */
        P = A[ip + p];

        for (r = p + 1; r < m; r++) {
            _r = r * n;

            if (A[ _r + p ] == 0) continue;

            /* Calculate the multiply factor */
            k = (-1) * A[ _r + p ] / P;

            for (c = 0; c < n; c++) {
                i = _r + c;
                A[i] = A[i] + k * A[ ip + c ];
            }
        }
    }
}


/**
 * Single precision Lower Triangular Transform.
 * This function transforms a matrix to a lower triangular form using
 * row elimination with single precision.
 * 
 * @param dim - 
 * @param A - 
 */
void __lower_triangle_f(int dim, float * A) {
    int w, r, _r, i, p;
    float P, k;
    int ip;
    
    for (p = dim - 1; p >= 0; p--) {

        ip = p * dim;
        P = A[ip + p];

        for (r = p - 1; r >= 0; r--) {

            _r = r * dim;

            if (A[_r + p] == 0) continue;

            k = (-1) * P / A[r * dim + p];

            for (w = 0; w < dim; w++) {
                i = _r + w;
                A[i] = k * A[i] + A[ ip + w ];
            }
        }
    }
}

/**
 * Double precision Lower Triangular Transform.
 * This function transforms a matrix to a lower triangular form using
 * row elimination with double precision.
 * 
 * @param dim - 
 * @param A - 
 */
void __lower_triangle_d(int dim, double * A) {
    int w, r, _r, i, p;
    double P, k;
    int ip;
    
    for (p = dim - 1; p >= 0; p--) {

        ip = p * dim;
        P = A[ip + p];

        for (r = p - 1; r >= 0; r--) {

            _r = r * dim;

            if (A[_r + p] == 0) continue;

            k = (-1) * P / A[r * dim + p];

            for (w = 0; w < dim; w++) {
                i = _r + w;
                A[i] = k * A[i] + A[ ip + w ];
            }
        }
    }
}
