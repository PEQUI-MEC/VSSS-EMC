using System.Collections.Generic;
using UnityEngine;

public class Gene {
    public int innv;
    public int from;
    public int to;
    public bool activated;
    public double weight;

    public Gene(int innv, int from, int to, bool state, double weight) {
        this.innv = innv;
        this.from = from;
        this.to = to;
        this.activated = state;
        this.weight = weight;
    }
}

public class Cromossomo {
    public static int next_innv;
    public static double bias = -1;
    public static List<Gene> all_genes;
    public List<Gene> ann;
    public List<int> hidden_nodes;
    public double[] inputs;
    public double[] outputs;
    public double fit;


    private class neuron {
        public float output;
        public bool calculated;
    }

    public Cromossomo(int n_in, int n_out, bool startBasicSyn) {
        inputs = new double[n_in];
        outputs = new double[n_out];
        ann = new List<Gene>();
        hidden_nodes = new List<int>();
        if (startBasicSyn) {
            for (int i = 1, innv_aux = 1; i <= n_in; i++) {
                for (int j = n_in + 1; j <= n_in + n_out; j++, innv_aux++) {
                    ann.Add(new Gene(innv_aux, i, j, true, Random.Range(-1.0f, 1.0f)));
                }
            }
        }
    }

    public void Execute() {
        bool flag1 = false, flag2;
        int numNodes = hidden_nodes.Count + outputs.Length + inputs.Length;
        neuron[] rede = new neuron[numNodes + 1]; // desconsiderando a posição 0
        List<int> naoVerificados = new List<int>();


        for (int i = 1; i <= numNodes; i++)
            rede[i] = new neuron();

        for (int i = 0; i < inputs.Length; i++) {
            rede[i + 1].output = (float)inputs[i];
            rede[i + 1].calculated = true;
        }
        for (int i = inputs.Length + 1; i <= hidden_nodes.Count + outputs.Length + inputs.Length; i++) {
            naoVerificados.Add(i);
        }

        while (!flag1) {
            flag1 = true;
            for (int k = 0; k < naoVerificados.Count; k++) {
                int i = naoVerificados[k];
                float soma = 0;
                List<Gene> parents = ann.FindAll(obj => obj.to == i); // pega todas as conexões que chegam no no 'i'
                flag2 = false; //flag para identificar se alguma conexão não foi previamente setada
                foreach (Gene it in parents) {
                    if (!rede[it.from].calculated) { //caso alguma entrada n foi calculada
                        flag2 = true;
                        break;
                    }
                    soma += (float)it.weight * rede[it.from].output;
                }
                soma += (float)bias;
                if (!flag2) { //se o neuronio i tem todas as entradas calculadas
                    rede[i].output = 1 / (1 + Mathf.Exp(-soma));
                    rede[i].calculated = true;
                    naoVerificados.RemoveAt(k);
                    flag1 = false; //garante que os não verificados serão verificados novamente devida mudança(acrecimo da saida do neuronio 'i')
                }
            }
        }
        for (int i = inputs.Length + 1, j = 0; i <= inputs.Length + outputs.Length; i++, j++) {
            outputs[j] = rede[i].output;
        }
    }

    public void SetInputs(double[] inVet) {
        if (inVet.Length != inputs.Length) {
            Debug.LogError("As entradas fornecidas não estão no msm formato(tamanhos diferentes)!!");
            return;
        }
        for (int i = 0; i < inputs.Length; i++) {
            inputs[i] = inVet[i];
        }
    }
}