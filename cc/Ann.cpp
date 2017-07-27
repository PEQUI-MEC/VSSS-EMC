#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

using namespace std;

class Synapse {
public:
  int id;
  int from;
  int to;
  bool activated;
  double weight;

  Gene(int nid, int nfrom, int nto, bool state, double nweight) {
    id = nid;
    from = nfrom;
    to = nto;
    activated = state;
    weight = nweight;
  }
};

class Neuron {
public:
  float output;
  bool calculated;
};

class Ann {
public:
  static int nextId;
  static vector<Synapse> allSynapses;
  vector<Synapse> synapses;
  vector<int> hidden_nodes;
  int ninputs, noutputs;
  double * inputs, * outputs, fit, bias = -1;

  Ann (const char * txtFileName) {
		std::ifstream txtFile;
		txtFile.open(txtFileName);
		std::string line, line1, line2, line3, line4;

		if (txtFile.is_open()) {
      // verifica a versão
			getline(txtFile, line); // line.c_str() - string, atof - float, atoi = int
      if(line.c_str() != "v1.0") {
        cout << "Ann version not suported!\n";
        return;
      }

      // le o número de entradas atribui às variaveis e aloca vetores
      getline(txtFile, line);
      ninputs = atoi(line);
      inputs = (double *) malloc(ninputs * sizeof(double));
      // le o número de saídas atribui às variaveis e aloca vetores
      getline(txtFile, line);
      noutputs = atoi(line);
      outputs = (double *) malloc(noutputs * sizeof(double));

      // !TODO talvez tenha que inicializar o vector, não sei
      //synapses = new vector<Synapse>;
      //hidden_nodes = new vector<int>;

      // lê o número de synapses e aloca vetores
      getline(line);
      int n = atoi(line);
      for(int i = 0; i < n; i++) {
        getline(line);
        getline(line1);
        getline(line2);
        getline(line3);
        getline(line4);

      }
      // pra cada proxima linha, le os valores das synapses, atribui e adiciona ao vetor
      // lê o número de hidden_nodes
      // para cada hidden_node, adiciona um int ao vetor

			txtFile.close();
		}
    else {
      cout << "Error opening .ann file!\n";
    }
  }

};
