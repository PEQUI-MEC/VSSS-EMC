#ifndef LS_H_INCLUDED
#define LS_H_INCLUDED
#include <stdlib.h>

class LS{
    private:
        int sampNum; // Number of samples stored
        int polyOrder; // Order of the poly interpolation

        double**    P; // Matrix that will hold the voltage readings
        double*     R; // Vector that will hold polynomial coefficients

        // Preenche a matriz G usando os pontos em P
        void fillMatrixG( double** G, double** P, int m, int n);

        // Atribui valor 0 a todos os elementos de M
        void zeroMatrix( double** M, int m, int n);

        // Atribui valor 0 a todos os elementos de V
        void zeroVector( double* V, int m);

        // Aloca espalo de memória para a matriz
        double** intializeMatrix( int m, int n);

        // Aloca espalo de memória para o vetor
        double* initializeVector( int m);

        // Copia o conteúdo da matriz A e do vetor B para a matriz M
        void prepareMatrix( double** M, double** A, double* B, int m);

        void retrosubstitutionU( double** M, double* R, int m);

        void retrosubstitutionL( double** M, double* R, int m);

        void lu( double** A, double** L, double** U, int m);

        void solveSystem( int polyOrder, double** A, double** L, double** U, double** M, double* R, double* B);

        // Faz a transpose de G em T
        void transpose( double** G, double** T, int m, int n);

        // Multiplica G por T
        void multiplyMatrix( double** G, double** T, double** A, int m, int n, int o);

        // Multiplica T por yP
        void multiplyMatrix( double** P, double** T, double* B, int m, int n);

        // This is the function that will interpolate, given the points and parameters
        int interpolate();

    public:

        LS();

        void init(int sampNum,int polyOrder);

        // This will add a new value to the voltages array
        void addValue(double newValue);

        // This will give you the position estimate for a certain future sample
        double estimate(int futureSample);
};

#endif
