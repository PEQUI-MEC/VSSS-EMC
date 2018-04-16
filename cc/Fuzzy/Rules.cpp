#include "Rules.hpp"
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::stringstream;

Rules::Rules(std::string file_name) {
	generate_baseRuleMatrix(file_name);
}

Rules::~Rules() {
}

void Rules::generate_baseRuleMatrix(std::string file_name) {
	cout << "Importing Base Rule" << endl;

	//Read from file_name
	std::ifstream in(file_name);
	if (!in) {
		printf("Erro na abertura do arquivo");
		getchar();
		exit(0);
	}
	string line, field;

	while (getline(in, line))                // get next line in file
	{
		stringstream ss(line);
		arrayRules.push_back(line);
		//cout << line << endl;
	}
	//cout << "----------------------------------" << endl;
}

float *Rules::inference(float *inputs,
						std::vector<std::shared_ptr<FuzzyFunction>> &input_variables,
						std::shared_ptr<FuzzyFunction> &output_variable) {
	float *output_variable_array = (float *) calloc(output_variable->getnParticoes(), sizeof(float));

	for (uint16_t i = 0; i < output_variable->getnParticoes(); i++) {
		output_variable_array[i] = 0;
	}

	float temp = 1;
	vector<float> local;
	for (uint16_t k = 0; k < this->arrayRules.size(); k++) {
		//Esse loop é para cada regra
		local.clear();
		vector<string> ruleWords = this->split(arrayRules[k]);

		vector<int> igualdades = this->posicaoIgualdades(ruleWords);

		//comparar o nome da variavel para saber se está nessa regra
		//cout << "Ruleoutput: " << ruleWords[igualdades[igualdades.size()-1]-1] << " OutputVar: " << output_variable->getVariableName() << endl;
		//Eliminar as regras que não tem a ver com a output necessaria
		if (strcmp(output_variable->getVariableName(), ruleWords[igualdades[igualdades.size() - 1] - 1].c_str()) == 0) {

			for (uint16_t m = 0; m < igualdades.size() - 1; m++) {
				uint16_t n;
				//Pegar a primeira variavel e verificar e verificar qual input é ela
				for (n = 0; n < input_variables.size(); n++) {
					if (strcmp(input_variables[n].get()->getVariableName(), ruleWords[igualdades[m] - 1].c_str()) ==
						0)
						break;
				}

				//n agora possui a posição no vetor que corresponde a mesma input que tem na regra
				local.push_back(input_variables[n].get()->singleton((char *) (ruleWords[igualdades[m] + 1].c_str()),
																	inputs[n]));
				//cout << " singleton = " << ruleWords[igualdades[m]-1] << "/" << ruleWords[igualdades[m]+1] << "/" << inputs[n];
			}

			temp = local[0];
			uint16_t contLocal = 1;

			if (local.size() > 1) {
				for (uint16_t l = 0; l < ruleWords.size(); l++) {
					if (ruleWords[l] == "AND") temp = AND(temp, local[contLocal++]);
					if (ruleWords[l] == "OR") temp = OR(temp, local[contLocal++]);
				}
			}
			char *membershipNameTemp = (char *) ruleWords[igualdades[igualdades.size() - 1] + 1].c_str();
			int indiceMembershipTemp = output_variable->getIndexbyLabel(membershipNameTemp);
			if (output_variable_array[indiceMembershipTemp] < temp)
				output_variable_array[indiceMembershipTemp] = temp;
			temp = 1;
		}
	}

	for (int i = 0; i < output_variable->getnParticoes(); i++) {
		cout << output_variable_array[i] << " ";
	}
	cout << endl;

	return output_variable_array;
}

// Funcoes auxiliares 
//Operator AND
double Rules::AND(double a, double b) {
	//cout << "AND" << endl;
	if (a < b)
		return a;
	return b;
}

//Operator OR
double Rules::OR(double a, double b) {
	//cout << "OR" << endl;
	if (a > b)
		return a;
	return b;
}

vector<string> Rules::split(const std::string str) {
	string buf; // Have a buffer string
	stringstream ss(str); // Insert the string into a stream
	vector<string> tokens; // Create vector to hold our words
	while (ss >> buf)
		tokens.push_back(buf);

	return tokens;
}

vector<int> Rules::posicaoIgualdades(vector<string> ruleWords) {
	vector<int> igual;
	for (uint16_t l = 0; l < ruleWords.size(); l++) {
		//cout << "ruleWords[" << l << "] => " << ruleWords[l] << endl;
		if ((ruleWords[l] == "=") || (ruleWords[l] == "is") || (ruleWords[l] == "IS")) igual.push_back(l);
	}
	return igual;
}
