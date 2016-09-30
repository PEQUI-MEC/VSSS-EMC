#include "LS.h"

LS::LS(){
    this->sampNum   = 1;
    this->polyOrder = 1;
}

void LS::init(int sampNum,int polyOrder){

    this->sampNum   = sampNum;
    this->polyOrder = polyOrder;

  // This is initializing the matrixes for the interpolation
    P = intializeMatrix( sampNum, 2);
    zeroMatrix(P, sampNum, 2);
    R = initializeVector( polyOrder+1);
    for ( int i = 0; i < sampNum; i++ )
        P[i][0] = i;
}

// This will give you the position estimate for a certain future sample
double LS::estimate(int futureSample){
    interpolate();
	return (sampNum + futureSample)*R[0] + R[1];
}

// This will add a new value to the voltages array
void LS::addValue(double newValue){
    for ( int i = 1; i < sampNum; i++ )
        P[i-1][1] = P[i][1];
    P[sampNum-1][1] = newValue;
}

// Preenche a matriz G usando os pontos em P
void LS::fillMatrixG( double** G, double** P, int m, int n){
    double aux = 1;
    for (int i = 0; i < m; i++){
        aux = 1;
        for (int j = n; j >= 0; j--){
            G[i][j] = aux;
            aux *= P[i][0];
        }
    }
}

// Atribui valor 0 a todos os elementos de M
void LS::zeroMatrix( double** M, int m, int n){
    for( int i = 0; i < m; i++)
        for( int j = 0; j < n; j++)
            M[i][j] = 0;
}

// Atribui valor 0 a todos os elementos de V
void LS::zeroVector( double* V, int m){
    for( int i = 0; i < m; i++)
        V[i] = 0;
}

// Aloca espalo de memória para a matriz
double** LS::intializeMatrix( int m, int n){
    double** M;

    M = (double**)calloc(m, sizeof(double*));

    for( int i = 0; i < m; i++) {
    	M[i] = (double*)calloc(n, sizeof(double));
    }
    return M;
}

// Aloca espalo de memória para o vetor
double* LS::initializeVector( int m){
    double* V;

    V = (double*)calloc(m, sizeof(double));

    return V;
}

// Copia o conteúdo da matriz A e do vetor B para a matriz M
void LS::prepareMatrix( double** M, double** A, double* B, int m){
    for (int i = 0; i < m; i++){
        for (int j = 0; j < m; j++)
            M[i][j] = A[i][j];
        M[i][m] = B[i];
    }
}


void LS::retrosubstitutionU( double** M, double* R, int m){

    // Este loop Sobe a matriz escalonada achando o valor de cada variável
    for (int i = m-1; i >= 0; i--){

        // Preenche o vetor R com B
        R[i] = M[i][m];

        // Subtrai de R todos os valores ponderados entre B e o pivô
        for (int j = (m-1); j > i; j--)
            R[i] -= M[i][j]*R[j];

        // Divide R pelo pivô
        R[i] /= M[i][i];
    }

}

void LS::retrosubstitutionL( double** M, double* R, int m){

    // Este loop Sobe a matriz escalonada achando o valor de cada variável
    for (int i = 0; i < m; i++){

        // Preenche o vetor R com B
        R[i] = M[i][m];

        // Subtrai de R todos os valores ponderados entre B e o pivô
        for (int j = 0; j < i; j++)
            R[i] -= M[i][j]*R[j];

        // Divide R pelo pivô
        R[i] /= M[i][i];
    }

}

void LS::lu( double** A, double** L, double** U, int m){

    // Zera os elementos de L e U triangularizando-as. Preenche com 1 os elementos da diagonal de L
    for (int i = 0; i < m; i++){
        for (int j = 0; j < m; j++){
            if( i > j)
                U[i][j] = 0;
            else if(i < j)
                L[i][j] = 0;
            else
                L[i][j] = 1;
        }
    }

    double sum = 0; // Variável auxiliar que armazena os valores a serem subtraídos a cada passo

    // A cada passada deste laço, define-se uma linha de U e uma coluna de L
    for (int i = 0; i < m; i++){

        // A cada passada deste laço, define-se uma linha de U
        for (int j = i; j < m; j++){
            sum = 0;
            for (int k = 0; k < i; k++)
                sum += L[i][k]*U[k][j];
            U[i][j] = A[i][j] - sum;
        }

        // A cada passada deste laço, define-se uma coluna de L
        for (int j = i+1; j < m; j++){
            sum = 0;
            for (int k = 0; k < j; k++)
                sum += L[j][k]*U[k][i];
            L[j][i] = (A[j][i] - sum)/U[i][i];
        }

    }
}


void LS::solveSystem( int polyOrder, double** A, double** L, double** U, double** M, double* R, double* B){
    lu( A, L, U, polyOrder+1);
    prepareMatrix( M, L, B, polyOrder+1);
    retrosubstitutionL( M, R, polyOrder+1);
    prepareMatrix( M, U, R, polyOrder+1);
    retrosubstitutionU( M, R, polyOrder+1);
}

// Faz a transpose de G em T
void LS::transpose( double** G, double** T, int m, int n){
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            T[j][i] = G[i][j];
}

// Multiplica G por T
void LS::multiplyMatrix( double** G, double** T, double** A, int m, int n, int o){
    double sum = 0;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++){
            sum = 0;
            for (int k = 0; k < o; k++)
                sum += G[i][k] * T[k][j];
            A[i][j] = sum;
        }
}

// Multiplica T por yP
void LS::multiplyMatrix( double** P, double** T, double* B, int m, int n){
    for (int i = 0; i < n; i++){
        B[i] = 0;
        for (int j = 0; j < m; j++)
            B[i] += T[i][j] * P[j][1];
    }
}

// This is the function that will interpolate, given the points and parameters
int LS::interpolate( ){

    double**    G = intializeMatrix( sampNum, polyOrder+1);
    double**    T = intializeMatrix( polyOrder+1, sampNum);
    double**    A = intializeMatrix( polyOrder+1, polyOrder+1);
    double**    L = intializeMatrix( polyOrder+1, polyOrder+1);
    double**    U = intializeMatrix( polyOrder+1, polyOrder+1);
    double**    M = intializeMatrix( polyOrder+1, polyOrder+1+1);

    double*     B = initializeVector( polyOrder+1);
    double*     V = initializeVector( polyOrder+1);

    zeroMatrix( G, sampNum, polyOrder+1);
    zeroMatrix( T, polyOrder+1, sampNum);
    zeroMatrix( A, polyOrder+1, polyOrder+1);
    zeroMatrix( L, polyOrder+1, polyOrder+1);
    zeroMatrix( U, polyOrder+1, polyOrder+1);
    zeroMatrix( M, polyOrder+1, polyOrder+1+1);

    zeroVector( B, polyOrder+1);
    zeroVector( R, polyOrder+1);
    zeroVector( V, polyOrder+1);

    // Preenche matriz G com Px
    fillMatrixG( G, P, sampNum, polyOrder);

    // Obtém a transpose de G em T
    transpose(G, T, sampNum, polyOrder+1);

    // Multiplica a trasposta T por G e jogo em A
    multiplyMatrix(T, G, A, polyOrder+1, polyOrder+1, sampNum);

    // Multiplica a trasposta T por Py e jogo em B
    multiplyMatrix( P, T, B, sampNum, polyOrder+1);

    solveSystem( polyOrder, A, L, U, M, R, B);


    free(G);
    free(T);
    free(A);
    free(L);
    free(U);
    free(M);
    free(B);

    return 0;
}
