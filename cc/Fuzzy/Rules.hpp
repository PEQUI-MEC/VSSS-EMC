#ifndef Rules_H
#define Rules_H

#include "FuzzyFunction.hpp"
#include <stdint.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>

class Rules {
	public:
		Rules();
		Rules(std::string file_name);
		~Rules();

		float *inference(float *inputs,
						 std::vector<std::shared_ptr<FuzzyFunction>> &input_variables,
						 std::shared_ptr<FuzzyFunction> &output_variable);
		std::vector<int> posicaoIgualdades(std::vector<std::string> ruleWords);

	protected:
	private:
		uint8_t input1, input2;
		void generate_baseRuleMatrix(std::string file_name);
		std::vector<std::string> split(const std::string str);
		static double OR(double a, double b);
		static double AND(double a, double b);

		std::vector<std::string> arrayRules;
};

#endif
