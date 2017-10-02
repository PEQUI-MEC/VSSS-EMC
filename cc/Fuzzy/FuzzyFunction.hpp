#ifndef FuzzyFunction_H
#define FuzzyFunction_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

class FuzzyFunction {
public:
    FuzzyFunction(int nParticoes, int points, char *functionName, char tipoDasMemberships);
    ~FuzzyFunction();

    int getnParticoes() const;
    float getDx() const;
    float getVmax() const;
    float getVmin() const;
	int getIndexbyLabel(char* label) const;
    void setValues(float npoints = 101);
    char *getVariableName() const;
  
    float singleton(char *label, float x);
    float singleton(int indice, float x);

   	void addMembership(int indice, const char * const label, float a, float b);
   	void addMembership(int indice, const char * const label, float a, float b, float c);
   	void addMembership(int indice, const char * const label, float a, float b, float c, float d);

protected:
    float vmin;
    float vmax;
    float dx;
    float **pontos;
    char *variableName;

private:
	void addMembership(int indice, const char * const label, const float * const pontos);
	float singletonTrapez(int indice, float x);
	float singletonGauss(int indice, float x);
	float singletonTriang(int indice, float x);


    const char **particoes;
    const int nParticoes;
    const int nPoints;
    const char tipoMemberships;
};
#endif // FuzzyFunction_H
