#include "FuzzyController.hpp"
#include <stdint.h>

FuzzyController::FuzzyController(void)
{
}

FuzzyController::~FuzzyController(void)
{
	input_Variables.clear();
}

void FuzzyController::read_from_file(string file_name)
{
	//exemplo de implementação utilizando a lib FuzzyVS2014
	ifstream in(file_name);

	string line;
	vector<string> v;

    bool is_input = true;
    
	while ( getline(in,line) )
	{
		stringstream ss(line);
		
		if (line.size()<2){		
				if (is_input) inputVars.push_back(v);
				else outputVars.push_back(v);
			v.clear();
			getline(in,line);
		}
		if (line.compare("INPUT")==0){
			//Pega a proxima linha e guarda o nome e o tipo de memberships no vetor de VarNames
			is_input=true;
			getline(in,line);
			//cout << " line compare = INPUT" << endl;
		}
		if (line.compare("OUTPUT")==0){
			//Pega a proxima linha e guarda o nome e o tipo de memberships no vetor de VarNames
			is_input=false;
			getline(in,line);
			//cout << " line compare = OUTPUT" << endl;
		}
		//tenho que guardar os valores para adicionar as variaveis
		if (is_input){
			// aqui para as inputs
			v.push_back(line);
		}else{
			//aqui para as outputs
			v.push_back(line);
		}
		//cout << "v.size=" << v.size();
		//cout << " line.size=" << line.size();
		//cout << " line=" << line << endl;
	}
	if (is_input) inputVars.push_back(v);
	else outputVars.push_back(v);
	
	//cout << "Quantidade de variaveis de entrada=" << inputVars.size() << endl;
	//cout << "Quantidade de variaveis de saida=" << outputVars.size() << endl;
}

vector<float> FuzzyController::ControladorFuzzy(vector<float> &input)
{
	
	vector<float> output;
	
	if (input.size()!=input_Variables.size()){
		cout << "O numero de variaveis declarados difere do numero de inputs enviados" << endl;
		return output;
	}
	
	output = defuzzyfier(input);
	//cout << "out defuzzyfier: " << output[0];
	
	return output;
}

void FuzzyController::importRules(string file_name)
{
	this->Regras=shared_ptr<Rules>(new Rules(file_name));
}
  
void FuzzyController::defineVariables(string file_name)
{
	//ler as variáves a partir do arquivo 
	read_from_file(file_name);
	vector<string> v;
	
	//inputVars.size() - numero de variáveis de entrada
	//inputVars[i].size() - numero de linhas armazenadas para cada variavel
	for (uint i=0;i<inputVars.size();i++){
		for (uint j=0;j<inputVars[i].size();j++){	
			//cout << inputVars[i][j] << endl;
			v = split(inputVars[i][j]);
			if (j==0)
			{ 	// A primeira linha descreve como e a membership
				// inputVars[i].size() - 1 numero de memberships
				input_Variables.push_back((shared_ptr<FuzzyFunction>)(new FuzzyFunction(
									(inputVars[i].size()-1),
									(v[1].compare("TRAPEZIO") == 0 ? 4 : 3),
									((char *)v[0].c_str()),
									(v[1].compare("GAUSS") == 0 ? 'G' : 'T'))));
			}
			else
			{
				int a= atoi(v[0].c_str());
				float b= atof(v[2].c_str());
				float c= atof(v[3].c_str());
				float d= atof(v[4].c_str());
				if(v.size()-2==4){
					float e= atof(v[5].c_str());
					input_Variables[i]->addMembership(a, (const char*) v[1].c_str(), b, c, d, e);
				}else{
					input_Variables[i]->addMembership(a, (const char*) v[1].c_str(), b, c, d);
				}
			}
		}
		input_Variables[i]->setValues();	
	}
	
	for (uint i=0;i<outputVars.size();i++){
		for (uint j=0;j<outputVars[i].size();j++){	
			v = split(outputVars[i][j]);
			if (j==0)
			{ 	// A primeira linha descreve como e a membership
				// inputVars[i].size() - 1 numero de memberships
				output_Variables.push_back((shared_ptr<FuzzyFunction>)(new FuzzyFunction(
									(outputVars[i].size()-1),
									(v[1].compare("TRAPEZIO") == 0 ? 4 : 3),
									((char *)v[0].c_str()),
									(v[1].compare("GAUSS") == 0 ? 'G' : 'T'))));
			}
			else
			{
				int a= atoi(v[0].c_str());
				float b= atof(v[2].c_str());
				float c= atof(v[3].c_str());
				float d= atof(v[4].c_str());
				if(v.size()-2==4){
					float e= atof(v[5].c_str());
					output_Variables[i]->addMembership(a, (const char*) v[1].c_str(), b, c, d, e);
				}else{
					output_Variables[i]->addMembership(a, (const char*) v[1].c_str(), b, c, d);
				}
			}
		}
		output_Variables[i]->setValues();	
	}
}

vector<float> FuzzyController::defuzzyfier(vector<float> &inputs)
{
	
	vector<float> results;

	for(uint16_t i=0; i<input_Variables.size();i++){
		if(inputs[i] < input_Variables[i]->getVmin() || inputs[i] > input_Variables[i]->getVmax()){
			cout << "input " << inputs[i] << endl;
			cout << "max " << input_Variables[i]->getVmax() << endl;
			cout << "min " << input_Variables[i]->getVmin() << endl;
			cout << "Inputs out of range!" << endl;
			return results;
			}
	}
	float in[inputs.size()];
	for (uint16_t i=0;i<inputs.size();i++) { 
		cout << "Input: " << inputs[i] << endl; 
		in[i] = inputs[i]; 
		}
	
	for (uint8_t i=0; i<output_Variables.size();i++){
		float* output_inference_values = this->Regras->inference(in, input_Variables, output_Variables[i]);
		int* pos = heightMethod(output_Variables[i], output_inference_values);
		results.push_back(pos[0]);
		results.push_back(pos[1]);
		results.push_back(pos[2]);
		//results.push_back(centroidMax(output_Variables[i], output_inference_values));
		//cout << "Result: " << results.back() << endl;
	}
	return results;
}

float FuzzyController::centroidMax(std::shared_ptr<FuzzyFunction> &varOut, float *output)
{
    float max = 0.0, max_ = 0.0;
    float denominador = 0;
    float numerador = 0;
    const float MIN = varOut->getVmin();
    const float MAX = varOut->getVmax();
    const float dx = varOut->getDx();
    for (float j = MIN; j <= MAX; j += dx) {
        for (uint16_t i = 0; i < varOut->getnParticoes(); i++) {
            max_ = varOut->singleton(i, j);
            if (output[i] < max_)
                max_ = output[i];
            if (max_ > max)
                max = max_;
        }
        denominador += max;
        numerador += max * j;
        max = 0.0;
    }
    //essa linha me impede de usar mais de 1 funcao de centroide
    free(output);    
    return fabs(numerador / denominador) > 0.01 ? (numerador / denominador) : 0;
}

int * FuzzyController::heightMethod(std::shared_ptr<FuzzyFunction> &varOut, float *output)
{
    float max = 0;
    int *positions = (int*) calloc(3, sizeof (int));
	positions[0]=-1; positions[1]=-1; positions[2]=-1;
	for (uint16_t j = 0; j < 3; j++) {
		max=0;
		for (uint16_t i = 0; i < varOut->getnParticoes(); i++)
		{
			if (output[i] > max)
			{
				if (positions[0]!=i && positions[1]!=i && positions[2]!=i)
				{
					max = output[i];
					positions[j]=i;
				}
			}
		}
	}
    //essa linha me impede de usar mais de 1 funcao de centroide
    free(output);    
    return positions;
}

vector<string> FuzzyController::split(const std::string str)
{
    string buf; // Have a buffer string
    stringstream ss(str); // Insert the string into a stream
    vector<string> tokens; // Create vector to hold our words
    while (ss >> buf)
        tokens.push_back(buf);
        
    return tokens;
}
