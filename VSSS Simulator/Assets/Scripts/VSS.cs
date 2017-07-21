using UnityEngine;

public class VSS : MonoBehaviour {

	public bool emJogo;
    private bool ended;

    private int fieldId;
    [SerializeField] private int tamTeam = 3;
	private int score1;
	private int score2;

    [SerializeField]
    private float ballStoppedLimit = 5;
    private float fitnessBola = 0;
    private float ballTimer;

    [SerializeField]
    private GameObject prefabRobot;

	private Robot[] robots1;
	private Robot[] robots2;

    private Vector3 lastBallPos;

	public Transform camPos;
	[SerializeField]
	private Transform ball;
	[SerializeField]
	private Transform ballStartPoint;
	[SerializeField]
	private Transform[] robots1StartPoint;
	[SerializeField]
	private Transform[] robots2StartPoint;

    private NEAT neat;

	public Cromossomo team1;
	public Cromossomo team2;

	private void Start () {
        // seta a tag de cada um e faz o posicionamento inicial (os dois times têm o mesmo tamanho)
        robots1 = new Robot[tamTeam];
        robots2 = new Robot[tamTeam];

		for (int i = 0; i < tamTeam; i++) {
            // instancia os robôs nas posições iniciais
            robots1[i] = GameObject.Instantiate(prefabRobot, robots1StartPoint[i]).GetComponent<Robot>();
            robots2[i] = GameObject.Instantiate(prefabRobot, robots2StartPoint[i]).GetComponent<Robot>();

            // joga eles pra dentro do objeto do campo
            robots1[i].transform.SetParent(this.transform);
            robots2[i].transform.SetParent(this.transform);

            // fala pra eles quem é a bola, pra calcular o desempenho de cada robô
            robots1[i].ball = robots2[i].ball = ball;

            // conserta a escala
            robots1[i].transform.localScale = robots2[i].transform.localScale = Vector3.one;

            // seta as tags
            robots1[i].SetTag(false, i);
            robots2[i].SetTag(true, i);
        }

        // reposiciona a bola
        ball.localPosition = ballStartPoint.transform.localPosition;

        lastBallPos = Vector3.zero;
    }

	public void SetTeams(Cromossomo team1, Cromossomo team2) {
		this.team1 = team1;
		this.team2 = team2;
	}

	private void Update() {
		if (emJogo) {
            double[] networkInput1 = new double[team1.inputs.Length];
            double[] networkInput2 = new double[team2.inputs.Length];
            int i = 0;

            // fitness da bola
            // convertposition retorna um valor entre 0 e 1, jogamos num intervalo entre -1 e 1
            fitnessBola += (ConvertPosition(ball.localPosition.z, false) - 0.5f) * 2;

            // verifica se a bola ficou parada por muito tempo
            if(ball.position == lastBallPos) {
                ballTimer += Time.deltaTime;
                if(ballTimer >= ballStoppedLimit) {
                    EndGame();
                }
            }
            lastBallPos = ball.position;

            // pega a posição dos robos e converte para entrada da rede
            for (int j = 0; j < tamTeam; j++) {
                // z representa x na imagem, x representa y
                networkInput1[i] = ConvertPosition(robots1[j].transform.localPosition.z, false);
                networkInput2[i++] = ConvertPosition(robots2[j].transform.localPosition.z, false);
          
                networkInput1[i] = ConvertPosition(robots1[j].transform.localPosition.x, true);
                networkInput2[i++] = ConvertPosition(robots2[j].transform.localPosition.x, true);
            }
            // acrescenta posição da bola no final dos dois vetores
            networkInput1[i] = networkInput2[i] = ConvertPosition(ball.transform.localPosition.z, false);
            i++;
            networkInput1[i] = networkInput2[i] = ConvertPosition(ball.transform.localPosition.x, true);

            // passa para a rede
            team1.SetInputs(networkInput1);
            team2.SetInputs(networkInput2);

            // executa
            team1.Execute();
            team2.Execute();

            // manda um ApplyVel com a saida da rede
            i = 0;
            for(int j = 0; j < tamTeam; j++) {
                robots1[j].ApplyVel(((float)team1.outputs[i] - 0.5f) * 2, ((float)team1.outputs[i + 1] - 0.5f) * 2);
                robots2[j].ApplyVel(((float)team2.outputs[i] - 0.5f) * 2, ((float)team2.outputs[i + 1] - 0.5f) * 2);
                i += 2;
            }
        }
	}

	public void Reset() {
		// faz o posicionamento inicial
		for (int i = 0; i < tamTeam; i++) {
			robots1 [i].transform.position = robots1StartPoint [i].position;
			robots2 [i].transform.position = robots2StartPoint [i].position;

            robots1[i].transform.localRotation = robots1StartPoint[i].localRotation;
            robots2[i].transform.localRotation = robots2StartPoint[i].localRotation;

            robots1 [i].rb.velocity = Vector3.zero;
            robots1 [i].rb.angularVelocity = Vector3.zero;

            robots2 [i].rb.velocity = Vector3.zero;
            robots2 [i].rb.angularVelocity = Vector3.zero;

            robots1 [i].desempenho = 0;
            robots2 [i].desempenho = 0;
        }
        ball.GetComponent<Rigidbody>().velocity = Vector3.zero;
        ball.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
        ball.transform.localPosition = ballStartPoint.transform.localPosition;
        ball.transform.localRotation = ballStartPoint.transform.localRotation;
        fitnessBola = 0;
    }

	public void Inicia() {
        ballTimer = 0;
        ended = false;
		emJogo = true;
	}

	public void Finaliza() {
		emJogo = false;
        ended = true;
		Fitness ();
    }

	private void Fitness() {
        // saldo de gols + (inverso da distancia da bola + posição da bola ((x - 0.5)*10) )(acumulativo)
        team1.fit = 100 * score1 - 10 * score2;
        team2.fit = 100 * score2 - 10 * score1;

        // o desempenho de cada robô é dada pela distancia da bola, acumulativo
        for (int i = 0; i < tamTeam; i++) {
            team1.fit += robots1[i].desempenho;
            team2.fit += robots2[i].desempenho;
        }

        // se a bola tiver no campo esquerdo, é ruim para o time1 então subtrai (fitnessbola seria negativo, + com - dá - duh)
        team1.fit += fitnessBola / 10;
        // se a bola tiver no campo direito, é ruim para o time2 (fitnessbola é positivo, então subtrai)
        team2.fit -= fitnessBola / 10;
    }

    // converte a posição local do robô em relação à pista para o intervalo [0, 1]
    public static float ConvertPosition(float pos, bool y) {
        if(y) { // position.x representa y
            return (pos - 26.7f) / 1216.7f; // o maior valor que x assume é 1243.4
        }
        else { // position.z representa x
            return (pos + 1542.1f) / 1415.6f; // o maior valor que z assume é -126.5
        }
    }

    public void AnnounceGoal(bool team1goal) {
        // evitar que o mesmo gol seja contabilizado mais de uma vez
        if (!emJogo)
            return;
        // para o jogo
        emJogo = false;
        // recalcula placar
        if (team1goal) score1++;
        else score2++;
        // atualiza a gui
        neat.UpdateScore(score1, score2, fieldId);
        // reposiciona
        //Reset();
        //emJogo = true;
        // finaliza o jogo
        EndGame();
    }

    public void SetID(int fieldId, NEAT neat) {
        this.neat = neat;
        this.fieldId = fieldId;
    }

    public Vector2 GetScore() {
        return new Vector2(score1, score2);
    }

    private void EndGame() {
        if(!ended) {
            ended = true;
            emJogo = false;
            neat.GameEnded();
        }
    }
}
