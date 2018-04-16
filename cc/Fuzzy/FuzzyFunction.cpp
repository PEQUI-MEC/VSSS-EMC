#include "FuzzyFunction.hpp"
#include <iostream>

FuzzyFunction::FuzzyFunction(int nParticoes, int points, char *functionName, char tipoDasMemberships) : nParticoes(
		nParticoes), nPoints(points), tipoMemberships(tipoDasMemberships) {
	// Alocacao para o array de strings
	this->particoes = (const char **) calloc(nParticoes, sizeof(const char *));

	// Alocacao para o array de pontos
	this->pontos = (float **) calloc(nParticoes, sizeof(float *));
	for (int i = 0; i < nParticoes; i++)
		this->pontos[i] = (float *) calloc(points, sizeof(float));

	this->variableName = (char *) calloc(sizeof(*functionName), sizeof(char *));
	strcpy(this->variableName, functionName);

	//std::cout << "nParticoes " << nParticoes << " this->fuctionName " << this->variableName << " points " << points << " tipoDasMemberships " << tipoDasMemberships << std::endl;
}

FuzzyFunction::~FuzzyFunction() {
	//dtor
	for (int i = 0; i < this->nParticoes; i++)
		free((char *) particoes[i]);
	free(particoes);

	for (int i = 0; i < this->nParticoes; i++)
		free((float *) pontos[i]);
	free(pontos);
}

void FuzzyFunction::addMembership(int indice, const char *const label, float a, float b) {
	float array[] = {a, b};
	this->addMembership(indice, label, array);
}

void FuzzyFunction::addMembership(int indice, const char *const label, float a, float b, float c) {
	float array[] = {a, b, c};
	this->addMembership(indice, label, array);
}

void FuzzyFunction::addMembership(int indice, const char *const label, float a, float b, float c, float d) {
	float array[] = {a, b, c, d};
	this->addMembership(indice, label, array);
}

void FuzzyFunction::addMembership(int indice, const char *const label, const float *const pontos) {
	//std::cout << "i=" << indice << " label=" << label ;
	//Guardo o nome da particao num vetor chamado particoes
	this->particoes[indice] = (const char *) calloc(strlen(label) + 1, sizeof(const char));
	strcpy((char *) this->particoes[indice], label);

	//defino os pontos que fazem parte daquela membership (particao)
	for (int i = 0; i < this->nPoints; i++) {
		this->pontos[indice][i] = pontos[i];
		//std::cout << ", " << pontos[i];
	}
	//std::cout << std::endl;
}

void FuzzyFunction::setValues(float npoints) {
	//essa função serve pra achar o max e o min
	this->vmin = this->pontos[0][0];
	this->vmax = this->vmin;
	for (int i = 0; i < this->nParticoes; i++) {
		for (int j = 0; j < this->nPoints; j++) {
			if (this->pontos[i][j] < this->vmin) {
				this->vmin = this->pontos[i][j];
			}
			if (this->pontos[i][j] > this->vmax) {
				this->vmax = this->pontos[i][j];
			}
		}
	}
	this->dx = (vmax - vmin) / (npoints - 1);
}

int FuzzyFunction::getnParticoes() const {
	return nParticoes;
}

float FuzzyFunction::getDx() const {
	return dx;
}

float FuzzyFunction::getVmax() const {
	return vmax;
}

float FuzzyFunction::getVmin() const {
	return vmin;
}

char *FuzzyFunction::getVariableName() const {
	return variableName;
}

// achar o indice da membership pela label
int FuzzyFunction::getIndexbyLabel(char *label) const {
	for (int i = 0; i < this->nParticoes; i++) {
		//std::cout << "this->particoes[i] = " << this->particoes[i] << " | " << label << std::endl;
		if (strcmp(this->particoes[i], label) == 0) return i;
	}
	std::cout << "Membership " << label << " nao encontrada!" << std::endl;
	return -1;
}

// As funcoes de singleton disponiveis
float FuzzyFunction::singleton(char *label, float x) {

	int indice = getIndexbyLabel(label);
	if (indice == -1) return 0;

	return this->singleton(indice, x);
}

float FuzzyFunction::singleton(int indice, float x) {
	if (tipoMemberships == 'G') {
		return this->singletonGauss(indice, x);
	} else {
		if (nPoints == 4) return this->singletonTrapez(indice, x);
		else return this->singletonTriang(indice, x);
	}
}

float FuzzyFunction::singletonTrapez(int indice, float x) {
	float a = this->pontos[indice][0];
	float b = this->pontos[indice][1];
	float c = this->pontos[indice][2];
	float d = this->pontos[indice][3];
	// Calculo da equacao do trapezio
	if (x < a || x >= d)
		return 0;
	if (x >= a && x < b)
		return (x - a) / (b - a);
	if (x >= b && x < c)
		return 1;
	if (x >= c && x < d)
		return (d - x) / (d - c);

	return -1;
}

float FuzzyFunction::singletonTriang(int indice, float x) {
	float a = this->pontos[indice][0];
	float b = this->pontos[indice][1];
	float c = this->pontos[indice][2];
	// Calculo da equacao do trapezio
	if (x < a || x >= c)
		return 0;
	if (x >= a && x < b)
		return (x - a) / (b - a);
	if (x >= b && x < c)
		return (c - x) / (c - b);

	return -1;
}

float FuzzyFunction::singletonGauss(int indice, float x) {
	float a = this->pontos[indice][0];
	float b = this->pontos[indice][1];
	float c = this->pontos[indice][2];
	return exp((-1) * ((pow((x - b) * (2 * (c - a)), 2)) / (0.2)));
}
