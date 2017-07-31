#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>

using namespace std;

class Synapse {
public:
  int id;
  int from;
  int to;
  bool activated;
  double weight;

  Synapse(int nid, int nfrom, int nto, bool state, double nweight) {
    id = nid;
    from = nfrom;
    to = nto;
    activated = state;
    weight = nweight;
  }
};

class Neuron {
public:
  double output;
  bool calculated;

  Neuron() {
  }
};

class Ann {
public:
  static int nextId;
  static vector<Synapse> allSynapses;
  vector<Synapse> synapses;
  vector<int> hidden_nodes;
  int ninputs, noutputs;
  double * outputs, fit, bias = -1;

  Ann (const char * txtFileName) {
		std::ifstream txtFile;
		txtFile.open(txtFileName);
		string line, line1, line2, line3, line4;

		if (txtFile.is_open()) {
      // verifica a versão
			getline(txtFile, line); // line.c_str() - string, atof - float, atoi = int
      if(line != "v1.0") {
        cout << "Ann version not suported!\n";
        return;
      }

      // le o número de entradas atribui às variaveis e aloca vetores
      getline(txtFile, line);
      ninputs = atoi(line.c_str());
      // le o número de saídas atribui às variaveis e aloca vetores
      getline(txtFile, line);
      noutputs = atoi(line.c_str());
      outputs = (double *) malloc(noutputs * sizeof(double));

      // !TODO talvez tenha que inicializar o vector, não sei
      //synapses = vector<Synapse>;
      //hidden_nodes = vector<int>;

      // lê o número de synapses e aloca vetores
      getline(txtFile, line);
      int n = atoi(line.c_str());
      // pra cada proxima linha, le os valores das synapses, atribui e adiciona ao vetor
      for(int i = 0; i < n; i++) {
        getline(txtFile, line);
        getline(txtFile, line1);
        getline(txtFile, line2);
        getline(txtFile, line3);
        getline(txtFile, line4);

        synapses.push_back(Synapse(atoi(line.c_str()), atoi(line1.c_str()), atoi(line2.c_str()), (bool)atoi(line3.c_str()), stod(line4.c_str())));
      }

      // lê o número de hidden_nodes
      getline(txtFile, line);
      n = atoi(line.c_str());
      // para cada hidden_node, adiciona um int ao vetor
      for(int i = 0; i < n; i++) {
        getline(txtFile, line);
        int t = atoi(line.c_str());
        hidden_nodes.push_back(t);
      }

			txtFile.close();
		}
    else {
      cout << "Error opening .ann file!\n";
    }
  } // Constructor

  // criar um vetor "inputs = (double *) malloc(ninputs * sizeof(double));"" antes de passar pro execute
  // a saída fica disponível no vetor outputs
  void Execute(double * inputs) {
    bool flag1 = false, flag2;
    int numNodes = hidden_nodes.size() + ninputs + noutputs;
    Neuron * net = new Neuron[numNodes + 1];
    vector<int> pending;// = vector<int>;

    // cria os neurônios que estarão nas extremidades das synapses
    for(int i = 1; i <= numNodes; i++) {
      net[i] = Neuron();
    }
    // inicializa os "neurônios" de entrada
    // ("neurônios" pois não é calculado só são realmente entrada dos próximos neurônios)
    for(int i = 0; i < ninputs; i++) {
      net[i + 1].output = (float) inputs[i];
      net[i + 1].calculated = true;
    }
    // adiciona todos os outros neurônios como pendentes
    for(int i = ninputs + 1; i <= numNodes; i++) {
      pending.push_back(i);
    }
    // faz o cálculo dos neurônios intermediários
    while(!flag1) {
      flag1 = true;
      for(int k = 0; k < pending.size(); k++) {
        int i = pending[k];
        double sum = 0;
        vector<Synapse> parents = FindAllParents(i); // encontra todas as conexões que chegam em i
        flag2 = false; // identifica se alguma conexão não foi previamente setada
        // faz a soma ponderada
        for(int l = 0; l < parents.size(); l++) {
          if(!net[parents[l].from].calculated) { // se alguma entrada não foi calculada
            flag2 = true;
            break;
          }
          sum += parents[l].weight * net[parents[l].from].output;
        } // for l
        sum += bias;
        if(!flag2) { // se o neurônio i tem todas as entradas calculadas
          net[i].output = 1 /(1 + exp(-sum));
          net[i].calculated = true;
          pending.erase(pending.begin() + k); // faz o erase pra não ser calculado de novo pelo while externo
          flag1 = false; // garante que os não verificados serão verificados novamente devido a mudança
        }
      } // for k
    }
    // passa os resultados da "ultima camada" para o vetor de saídas
    // ("última camada" pois na verdade não há camadas, os neurônios tratados são só as folhas da árvore)
    for(int i = ninputs + 1, j = 0; i <= ninputs + noutputs; i++, j++) {
      outputs[j] = net[i].output;
    } // for i, j
  } // Execute

  // encontra todas as conexões que chegam em i
  vector<Synapse> FindAllParents(int node) {
    vector<Synapse> parents;// = vector<Synapse>;
    for(int i = 0; i < synapses.size(); i++) {
      if(synapses[i].to == node) {
        parents.push_back(synapses[i]);
      }
    }
    return parents;
  }
};
