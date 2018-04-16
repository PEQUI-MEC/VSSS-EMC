#ifndef FuzzyController_H
#define FuzzyController_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "FuzzyFunction.hpp"
#include "Rules.hpp"

class FuzzyController {
	public:
		FuzzyController(void);
		~FuzzyController(void);

		std::vector<float> ControladorFuzzy(std::vector<float> &input);

		void defineVariables(std::string file_name);
		void importRules(std::string file_name);

	private:
		std::vector<std::shared_ptr<FuzzyFunction>> input_Variables;
		std::vector<std::shared_ptr<FuzzyFunction>> output_Variables;

		std::vector<std::vector<std::string>> inputVars;
		std::vector<std::vector<std::string>> outputVars;

		std::vector<float> defuzzyfier(std::vector<float> &inputs);
		std::shared_ptr<Rules> Regras;

		float centroidMax(std::shared_ptr<FuzzyFunction> &varOut, float *output);
		int *heightMethod(std::shared_ptr<FuzzyFunction> &varOut, float *output);
		//float centroidSum(FuzzyFunction *varOut, float *output);

		void read_from_file(std::string file_name);
		std::vector<std::string> split(const std::string str);
};

#endif
