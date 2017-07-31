#include <iostream>
#include <string>
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
  double output;
  bool calculated;
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
      // le o número de saídas atribui às variaveis e aloca vetores
      getline(txtFile, line);
      noutputs = atoi(line);
      outputs = (double *) malloc(noutputs * sizeof(double));

      // !TODO talvez tenha que inicializar o vector, não sei
      synapses = new vector<Synapse>;
      hidden_nodes = new vector<int>;

      // lê o número de synapses e aloca vetores
      getline(line);
      int n = atoi(line);
      // pra cada proxima linha, le os valores das synapses, atribui e adiciona ao vetor
      for(int i = 0; i < n; i++) {
        getline(line);
        getline(line1);
        getline(line2);
        getline(line3);
        getline(line4);

        synapses.push_back(Synapse(atoi(line), atoi(line1), atoi(line2), (bool)atoi(line3), stod(line4)));
      }

      // lê o número de hidden_nodes
      getline(line);
      n = atoi(line);
      // para cada hidden_node, adiciona um int ao vetor
      for(int i = 0; i < n; i++) {
        getline(line);
        int t = atoi(line);
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
    int numNodes = hidden_nodes.size() + ninputs + noututs;
    Neuron * net = new Neuron[numNodes + 1];
    vector<int> pending = new vector<int>;

    // cria os neurônios que estarão nas extremidades das synapses
    for(int i = 1; i <= numNodes; i++) {
      net[i] = Neuron();
    }
    // inicializa os "neurônios" de entrada
    // ("neurônios" pois não é calculado só são realmente entrada dos próximos neurônios)
    for(int i = 0; i < ninputs; i++) {
      rede[i + 1].output = (float) inputs[i];
      rede[i + 1].calculated = true;
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
          soma += parents[l].weight * net[parents[l].from].output;
        } // for l
        soma += bias;
        if(!flag2) { // se o neurônio i tem todas as entradas calculadas
          net[i].output = 1 /(1 + exp(-soma));
          net[i].calculated = true;
          pending.pop_back(k); // faz o pop pra não ser calculado de novo pelo while externo
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
    vector<Synapse> parents = new vector<Synapse>;
    for(int i = 0; i < synapses.size(); i++) {
      if(synapse[i].to == node) {
        parents.push_back(synapse[i]);
      }
    }
    return parents;
  }
};
