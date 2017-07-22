using System.Collections;
using System.IO;
using UnityEngine;

public class NEAT : MonoBehaviour {
    
	// AG
    [SerializeField] 
	private int sizePop; // tamanho da população

	// ANN
	[SerializeField]
	private int numInputs; // número de entradas da rede (posição de todos os robôs e da bola)
	[SerializeField] 
	private int numOutputs; // número de saídas (velocidade de cada roda de cada robô
	[SerializeField] 
	private double mutateNodeRate; // taxa de mutação de adição de nó
	[SerializeField] 
	private double mutateSynpRate; // taxa de mutação de adição de sinapse
	[SerializeField] 
	private double mutateWeigthRate; // taxa de mutação de pesos
	[SerializeField] 
	private double crossOverRate; // taxa de cruzamento
	[SerializeField] 
	private double mutateRemoveNode; // taxa de mutação de remoção de nó (rede degenerativa)
	[SerializeField] 
	private double mutateRemoveSyn; // taxa de mutação de remoção de sinapse
	[SerializeField] 
	private double activeGeneRate; // 
	[SerializeField] 
	private double deactiveGeneRate; //
	[SerializeField] 
	private double limiarNetEqual; // limiar de semelhança entre redes
	[SerializeField] 
	private double ce; // influência da presença de neurônios
	[SerializeField]
	private double cd; // influência da ausência de neurônios
	[SerializeField] 
	private double cw; // influência dos pesos no cálculo da similaridade

	private Population pop;

	private int generation = 0;
	private int fieldWatched = 0;
    private int gamesEnded;

    [SerializeField]
    private bool manual = false;

    [SerializeField] 
	private GameObject prefabField;

	[SerializeField]
	private Transform cam;

	private VSS[] fields;

    [SerializeField]
    private Game_UI gui;

    private Robot robotSelected;

    void Start () {
        Time.timeScale = 1;
       
        // NEAT
        pop = new Population(sizePop, numInputs, numOutputs, crossOverRate, mutateNodeRate, mutateSynpRate, mutateWeigthRate, mutateRemoveNode, mutateRemoveSyn, activeGeneRate, deactiveGeneRate, limiarNetEqual, ce, cd, cw);

        // código de implementação especifica
		if (sizePop == 0 || sizePop % 2 == 1)
			Debug.LogError ("O tamanho da população deve ser par e maior que 0, os times se enfrentam 2 a 2.");
		fields = new VSS[sizePop / 2];
		// instancia campos
        for (int i = 0; i < sizePop; i += 2) {
			GameObject go = GameObject.Instantiate(prefabField, new Vector3((i / 2) * 150, 0, 0), Quaternion.identity) as GameObject;
			fields [i/2] = go.GetComponent<VSS> ();
            fields [i/2].SetID(i / 2, this);
            if(!manual)
                fields [i/2].SetTeams(pop.pop[i], pop.pop[i + 1]);
        }
			
		ApplyCamPosition (fields [0].camPos.transform);

        if(!manual)
            StartCoroutine(Run());
	}

    private void Update() {
        // navega com a camera entre os campos
		if (Input.GetKeyDown (KeyCode.UpArrow)) {
			fieldWatched = (fieldWatched < fields.Length - 1) ? fieldWatched + 1 : 0;
			ApplyCamPosition (fields [fieldWatched].camPos.transform);
            UpdateScore(fieldWatched);
        }
		else if (Input.GetKeyDown (KeyCode.DownArrow)) {
			fieldWatched = (fieldWatched > 0) ? fieldWatched - 1 : fields.Length - 1;
			ApplyCamPosition (fields [fieldWatched].camPos.transform);
            UpdateScore(fieldWatched);
		}

        // acelera o tempo
        if(Input.GetKeyDown (KeyCode.F)) {
            Time.timeScale += 0.2f;
        }
        else if(Input.GetKeyDown(KeyCode.B)) {
            Time.timeScale -= 0.2f;
        }

        // se ainda há times jogando
        if (gamesEnded < sizePop / 2) {
            // só pode passar a geração se houver uma em jogo
            if (Input.GetKeyDown(KeyCode.P))
                PassGeneration();
        }

        // faz os robôs irem onde for clicado
        if(Input.GetMouseButtonDown(0)) {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;

            if (Physics.Raycast(ray, out hit)) {
                if(hit.transform.CompareTag("Robot")) {
                    if (robotSelected != null && robotSelected.transform != hit.transform) {
                        robotSelected.Glow(false);
                    }
                    robotSelected = hit.transform.GetComponent<Robot>();
                    robotSelected.Glow(true);
                }
                else if(robotSelected != null) {
                    robotSelected.SetWorldDestination(new Vector2(hit.point.z, hit.point.x)); // z é x e x é y
                    robotSelected.Glow(false);
                    robotSelected = null;
                }
            }
        }
    }

    private void PassGeneration() {
        if (manual)
            return;
        Debug.Log("Finalizando e calculando fitness");
        for (int i = 0; i < sizePop / 2; i++) {
            fields[i].Finaliza();
        }
        Debug.Log("Aplicando operadores");
        pop.selecao();
        Debug.Log("Distribuindo redes");
        for(int i = 0; i < sizePop; i += 2) {
            fields[i/2].SetTeams(pop.pop[i], pop.pop[i + 1]);
        }
        Debug.Log("Reposicionando robôs");
        for (int i = 0; i < sizePop / 2; i++) {
            fields[i].Reset();
        }
    }

	private IEnumerator Run() {
        while(true) {
            generation++;
            Debug.Log("------- Geração " + generation + " -------");

            // reseta o contador de jogos acabados
            gamesEnded = 0;
            Debug.Log("Iniciando jogos");
            for (int i=0; i < sizePop / 2; i++) {
				fields [i].Inicia ();
            }
            // espera o tempo de jogo passar
            Debug.Log("Aguardando fim dos jogos");
            while (gamesEnded < sizePop / 2)
			    yield return new WaitForSeconds(0.02f);
            PassGeneration();
        }
    }

    public void GameEnded() {
        gamesEnded++;
    }

    // uma função para no futuro fazer uma animaçãozinha
	private void ApplyCamPosition(Transform dest) {
		cam.transform.position = dest.position;
		//cam.transform.rotation = dest.rotation;
	}

    private void UpdateScore(int fieldId) {
        Vector2 scoreWatched = fields[fieldWatched].GetScore();
        gui.UpdateScore((int)scoreWatched.x, (int)scoreWatched.y);
    }
    public void UpdateScore(int team1, int team2, int fieldId) {
        if(fieldId == fieldWatched) {
            gui.UpdateScore(team1, team2);
        }
    }

    public void SaveAnn(bool left, string where) {
        Cromossomo rede = left ? fields[fieldWatched].team1 : fields[fieldWatched].team2;

        if (File.Exists(where)) {
            File.Delete(where);
        }

        // cria o arquivo
        using (FileStream fs = File.Create(where)) {
            AddText(fs, "v1.0\n\n"); // versão
            AddText(fs, rede.inputs.Length.ToString() + " " + rede.outputs.Length.ToString() + "\n\n"); // número de entradas e saídas
            AddText(fs, rede.ann.Count.ToString() + "\n\n"); // número de genes
            for(int i = 0; i < rede.ann.Count; i++) {
                AddText(fs, rede.ann[i].innv + " " + rede.ann[i].from + " " + rede.ann[i].to + " " + rede.ann[i].activated + " " + rede.ann[i].weight + "\n\n"); // número de genes
            }
            for (int i = 0; i < rede.hidden_nodes.Count; i++) {
                AddText(fs, rede.hidden_nodes[i].ToString() + (i < rede.hidden_nodes.Count - 1 ? " " : "\n\n")); // o operador ternário insere um espaço no final da linha enquanto esse não for o último valor
            }
            AddText(fs, "-end\n"); // finaliza o arquivo
        }

        Debug.Log("File \"" + where + "\" saved successfully!");
    }
    private static void AddText(FileStream fs, string value) {
        byte[] info = new System.Text.UTF8Encoding(true).GetBytes(value);
        fs.Write(info, 0, info.Length);
    }
}
