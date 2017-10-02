#ifndef FuzzyController_H
#define FuzzyController_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#include "FuzzyFunction.hpp"
#include "Rules.hpp"


class FuzzyController
{
public:
	FuzzyController(void);
	~FuzzyController(void);
	
	vector<float> ControladorFuzzy(vector<float> &input);

	void defineVariables(string file_name);
    void importRules(string file_name);

private:
	vector<shared_ptr<FuzzyFunction>> input_Variables;
    vector<shared_ptr<FuzzyFunction>> output_Variables;    
    
    vector<vector<string>> inputVars;
    vector<vector<string>> outputVars;

    vector<float> defuzzyfier(vector<float> &inputs);
	shared_ptr<Rules> Regras;
	
	float centroidMax(std::shared_ptr<FuzzyFunction> &varOut, float *output);
	int* heightMethod(std::shared_ptr<FuzzyFunction> &varOut, float *output);
	//float centroidSum(FuzzyFunction *varOut, float *output);

	void read_from_file(string file_name);
	vector<string> split(const std::string str);


};

#endif
