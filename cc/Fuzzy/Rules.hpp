#ifndef Rules_H
#define Rules_H

#include "FuzzyFunction.hpp"
#include <stdint.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>

using namespace std;

class Rules{
public:
	Rules();
	Rules(std::string file_name);
	~Rules();
	
	float* inference(	float* inputs, 
						std::vector<std::shared_ptr<FuzzyFunction>> &input_variables,
						std::shared_ptr<FuzzyFunction> &output_variable);
	vector<int> posicaoIgualdades(vector<string> ruleWords);
	
protected:
private:
	uint8_t input1, input2;
	void generate_baseRuleMatrix(string file_name);
	vector<string> split(const std::string str);
	static double OR(double a, double b);
	static double AND(double a, double b);

	vector<string> arrayRules;
};


#endif
