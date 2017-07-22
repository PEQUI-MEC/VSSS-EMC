using System.Collections.Generic;
using UnityEngine;

public class Population {
    public List<Cromossomo> pop;
    public float[] apt;
    public double mutateNodeRate;
    public double mutateSynpRate;
    public double mutateWeigthRate;
    public double crossOverRate;
    //static double learningRate;
    public double mutateRemoveNode;
    public double mutateRemoveSyn;
    public double activeGeneRate;
    public double deactiveGeneRate;
    public double limiarNetEquals;
    public double constantDisjoint;
    public double constantExcess;
    public double constantWeight;


    public Population(int length_population, int num_inputs, int num_outputs, double crossORate, double mutNodeRate, double mutSynpRate, double mutWeigRate, /*double learnRate,*/ double mutRemoveNode, double mutRemoveSyn, double activeRate, double deactiveRate, double limiarNetEqu, double ce, double cd, double cw) {
        pop = new List<Cromossomo>();
        apt = new float[length_population];
        for (int i = 0; i < length_population; i++) {
            pop.Add(new Cromossomo(num_inputs, num_outputs, false));
        }
        Cromossomo.next_innv = num_outputs + num_outputs + 1;
        Cromossomo.all_genes = new List<Gene>();
        crossOverRate = crossORate;
        mutateNodeRate = mutNodeRate;
        mutateSynpRate = mutSynpRate;
        mutateWeigthRate = mutWeigRate;
        //learningRate = learnRate;
        mutateRemoveNode = mutRemoveNode;
        mutateRemoveSyn = mutRemoveSyn;
        activeGeneRate = activeRate;
        deactiveGeneRate = deactiveRate;
        limiarNetEquals = limiarNetEqu;
        constantDisjoint = cd;
        constantExcess = ce;
        constantWeight = cw;
        for (int i = 1, innv_aux = 1; i <= num_inputs; i++) {
            for (int j = num_inputs + 1; j <= num_inputs + num_outputs; j++, innv_aux++) {
                Cromossomo.all_genes.Add(new Gene(innv_aux, i, j, false, 0f));
            }
        }
    }

    // retorna true se as redes forem equivalentes
    private bool sameSpecie(Cromossomo c1, Cromossomo c2) {
        int N = c1.ann.Count > c2.ann.Count ? c1.ann.Count : c2.ann.Count;
        //int M = c1.ann.Count < c2.ann.Count ? c1.ann.Count : c2.ann.Count;
        int excess = 0, disjoint = 0;
        float W = 0, sigma;

        if (c1.ann.Count == 0) {
            excess = N = c2.ann.Count;
        }
        else if (c2.ann.Count == 0) {
            excess = N = c1.ann.Count;
        }
        else {
            c1.ann.Sort(delegate (Gene p, Gene k) {
                return (p.innv.CompareTo(k.innv));
            });
            c2.ann.Sort(delegate (Gene p, Gene k) {
                return (p.innv.CompareTo(k.innv));
            });
            int maior = c1.ann[c1.ann.Count - 1].innv > c2.ann[c2.ann.Count - 1].innv ? c1.ann[c1.ann.Count - 1].innv : c2.ann[c2.ann.Count - 1].innv; //maior innv da menor network
            Gene a, b;
            for (int i = 0; i < maior; i++) {
                a = c1.ann.Find(obj => obj.innv == i);
                b = c2.ann.Find(obj => obj.innv == i);
                if (a != null) {
                    if (b == null) {
                        if (a.innv > maior)
                            excess++;
                        else
                            disjoint++;
                    }
                    else { // existe nos dois
                        W += Mathf.Abs((float)(a.weight - b.weight));
                    }
                }
                else {
                    if (b != null) {
                        if (b.innv > maior)
                            excess++;
                        else
                            disjoint++;
                    }
                }
            }
        }
        if (N != 0) {
            sigma = (float)(constantDisjoint * disjoint / N) + (float)(constantExcess * excess / N) + (float)(constantWeight * W);
        }
        else {
            sigma = (float)(constantWeight * W);
        }
        if (sigma < limiarNetEquals)
            return true;
        else
            return false;
    }

    public Cromossomo CrossOver(Cromossomo pai, Cromossomo mae) {

        Cromossomo filho = new Cromossomo(pai.inputs.Length, pai.outputs.Length, false);


        if (Random.Range(0.0f, 100.0f) < crossOverRate && sameSpecie(pai, mae)) {

            Gene[] pai_array = pai.ann.ToArray();
            Gene[] mae_array = mae.ann.ToArray();

            //copia dados do pai para o filho
            for (int i = 0; i < pai_array.Length; i++) {
                filho.ann.Add(new Gene(pai_array[i].innv, pai_array[i].from, pai_array[i].to, pai_array[i].activated, pai_array[i].weight));
            }

            //verifica se existem dados em comum entre pai e mae
            for (int i = 0; i < mae_array.Length; i++) {
                Gene aux = filho.ann.Find(obj => obj.innv == mae_array[i].innv);
                if (aux == null) { //ligação so existe na rede mãe
                    filho.ann.Add(new Gene(mae_array[i].innv, mae_array[i].from, mae_array[i].to, mae_array[i].activated, mae_array[i].weight));
                }
                else { // ligação presente nas duas redes
                    aux.activated = aux.activated && mae_array[i].activated;
                    aux.weight = (aux.weight + mae_array[i].weight) / 2;
                }
            }

            //adiciona nós do meio à lista
            for (int i = 0; i < filho.ann.Count; i++) {
                if (filho.ann[i].from > filho.inputs.Length + filho.outputs.Length && !filho.hidden_nodes.Contains(filho.ann[i].from))
                    filho.hidden_nodes.Add(filho.ann[i].from);
                if (filho.ann[i].to > filho.inputs.Length + filho.outputs.Length && !filho.hidden_nodes.Contains(filho.ann[i].to))
                    filho.hidden_nodes.Add(filho.ann[i].to);
            }
            filho.hidden_nodes.Sort();
        }
        else { // copia maior entre pai e mae
            Cromossomo melhor = pai.fit > mae.fit ? pai : mae;
            for (int i = 0; i < melhor.ann.Count; i++) {
                filho.ann.Add(new Gene(melhor.ann[i].innv, melhor.ann[i].from, melhor.ann[i].to, melhor.ann[i].activated, melhor.ann[i].weight));
            }
            for (int i = 0; i < melhor.hidden_nodes.Count; i++) {
                filho.hidden_nodes.Add(melhor.hidden_nodes[i]);
            }
        }
        return filho;
    }

    public void Mutacao(Cromossomo it) {
        //add node
        if (Random.Range(0.0f, 100.0f) < mutateNodeRate && it.ann.Count > 0) {
            //busca indice do novo nó dentro da nede neural
            int idxNode;
            if (it.hidden_nodes.Count == 0) {
                idxNode = it.inputs.Length + it.outputs.Length + 1;
            }
            else {
                idxNode = it.hidden_nodes[it.hidden_nodes.Count - 1] + 1;
            }
            //sorteia ligação entre dois nós (Gene)
            Gene l = it.ann[Random.Range(0, it.ann.Count)];
            //caso nó esteja ativo faz a mutação
            if (l.activated) {
                l.activated = false;
                //pesquisa se Gene ja foi catalogado no banco de dados, caso sim atribui o innv desse gene ao novo gene mutado, caso não cataloga novo gene
                //Gene 1
                Gene innv_novo = Cromossomo.all_genes.Find(obj => obj.from == l.from && obj.to == idxNode);
                if (innv_novo != null) {
                    it.ann.Add(new Gene(innv_novo.innv, l.from, idxNode, true, Random.Range(-1.0f, 1.0f)));
                    innv_novo = null;
                }
                else {
                    it.ann.Add(new Gene(Cromossomo.next_innv, l.from, idxNode, true, Random.Range(-1.0f, 1.0f)));
                    Cromossomo.all_genes.Add(new Gene(Cromossomo.next_innv, l.from, idxNode, false, 0));
                    Cromossomo.next_innv++;
                }
                //Gene 2
                innv_novo = Cromossomo.all_genes.Find(obj => obj.from == idxNode && obj.to == l.to);
                if (innv_novo != null) {
                    it.ann.Add(new Gene(innv_novo.innv, idxNode, l.to, true, Random.Range(-1.0f, 1.0f)));
                }
                else {
                    it.ann.Add(new Gene(Cromossomo.next_innv, idxNode, l.to, true, Random.Range(-1.0f, 1.0f)));
                    Cromossomo.all_genes.Add(new Gene(Cromossomo.next_innv, idxNode, l.to, false, 0));
                    Cromossomo.next_innv++;
                }
                it.hidden_nodes.Add(idxNode);
                it.hidden_nodes.Sort();
            }
        }
        //add connection between nodes not connected
        if (Random.Range(0.0f, 100.0f) < mutateSynpRate) {
            int num_neuronios, from_aux, to_aux;
            if (it.hidden_nodes.Count > 0)
                num_neuronios = it.hidden_nodes[it.hidden_nodes.Count - 1];
            else
                num_neuronios = it.inputs.Length + it.outputs.Length;
            for (int i = 0; i < 5; i++) { //5 é o numero de chances dada para se obter dois neuronios não conectados;
                from_aux = Random.Range(1, num_neuronios + 1);
                to_aux = Random.Range(it.inputs.Length + 1, num_neuronios + 1);
                //se neuronios sorteados não sao iguais e se o neuronio de saida do sinal não pertencer a saida da rede e se eles ja não estão conectados => faz a conexão
                if (from_aux != to_aux && (from_aux <= it.inputs.Length || from_aux > it.inputs.Length + it.outputs.Length) && it.ann.Find(obj => obj.from == from_aux && obj.to == to_aux) == null) {
                    Gene innv_novo = Cromossomo.all_genes.Find(obj => obj.from == from_aux && obj.to == to_aux); //pesquisa se a conexão ja foi catalogada
                    if (innv_novo != null) { //usa catalogo da conexão
                        it.ann.Add(new Gene(innv_novo.innv, from_aux, to_aux, true, Random.Range(-1.0f, 1.0f)));
                    }
                    else { //cataloga nova conexão
                        it.ann.Add(new Gene(Cromossomo.next_innv, from_aux, to_aux, true, Random.Range(-1.0f, 1.0f)));
                        Cromossomo.all_genes.Add(new Gene(Cromossomo.next_innv, from_aux, to_aux, false, 0));
                        Cromossomo.next_innv++;
                    }
                    break;
                }
            }
        }
        //mutate weigth of connection
        foreach (Gene i in it.ann) {
            if (Random.Range(0.0f, 100.0f) < mutateWeigthRate) {
                if (Random.Range(0.0f, 100.0f) < 90.0f) {
                    if (Random.Range(0, 2) == 1)
                        i.weight += 1f / Mathf.Exp(Mathf.Pow(Random.Range(-4.0f, 4.0f), 2));
                    else
                        i.weight -= 1f / Mathf.Exp(Mathf.Pow(Random.Range(-4.0f, 4.0f), 2));
                }
            }
            else {
                i.weight = Random.Range(-1.0f, 1.0f);
            }
        }
        //remove node
        if (Random.Range(0.0f, 100.0f) < mutateRemoveNode && it.hidden_nodes.Count > 0) {
            int remov = it.hidden_nodes[Random.Range(0, it.hidden_nodes.Count)];
            it.ann.RemoveAll(obj => obj.to == remov || obj.from == remov);
            it.hidden_nodes.Remove(remov);
            it.hidden_nodes.Sort();
        }
        //remove connection
        if (Random.Range(0.0f, 100.0f) < mutateRemoveSyn && it.ann.Count > 0) {
            it.ann.Remove(it.ann[Random.Range(0, it.ann.Count)]);
        }
        //active gene
        if (Random.Range(0.0f, 100.0f) < activeGeneRate && it.ann.Count > 0)
            it.ann[Random.Range(0, it.ann.Count)].activated = true;

        //deactive gene
        if (Random.Range(0.0f, 100.0f) < deactiveGeneRate && it.ann.Count > 0)
            it.ann[Random.Range(0, it.ann.Count)].activated = false;
    }

    public void selecao() {
        List<Cromossomo> newpop = new List<Cromossomo>();
        int half = pop.Count / 2, i;
        float sum = 0;
        Cromossomo filho;


        //ordena população pela aptidão
        pop.Sort(delegate (Cromossomo a, Cromossomo b) {
            return a.fit.CompareTo(b.fit);
        });

        // normaliza aptidoes para serem positivas -- necessário para a roleta
        normalizaAptidoes();

        //cria vetor de aptidão
        for (i = 0; i < pop.Count; i++)
            apt[i] = sum + (float)pop[i].fit;
        
        //eletismo para metade da população
        for (i = 0; i < half; i++)
            newpop.Add(pop[i]);

        //crossover para a segunda metade da população
        for (; i < pop.Count; i++) {
            filho = CrossOver(pop[roleta()], pop[roleta()]);         //crossover
            Mutacao(filho);                                            //mutação
            newpop.Add(filho);                                         //colocado na nova população
        }

        //população é atualizada
        pop = newpop;
    }

    private int torneio() {
        int i = Random.Range(0, pop.Count), j = Random.Range(0, pop.Count);
        return pop[i].fit > pop[j].fit ? i : j;
    }

    // não funciona pra fitness negativo
    private int roleta() {
        int i;
        double aux = Random.Range(0.0f, apt[apt.Length - 1]);
        
        for (i = 0; i < pop.Count; i++) {
            if (aux <= apt[i])
                break;
        }

        return i;
    }

    private void normalizaAptidoes() {
        double menor = pop[0].fit;
        for(int i = 0; i < pop.Count; i++) {
            pop[i].fit -= menor;
        }
    }
}