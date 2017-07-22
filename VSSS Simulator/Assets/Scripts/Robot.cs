using UnityEngine;

public class Robot : MonoBehaviour {

	private const float fatorDeDesaceleracao = 20;

	// aloca globalmente pelo desempenho (melhor que alocar na update)
	private float leftTorque;
    private float rightTorque;
    public float desempenho = 0;

    private Vector3 lastPos;

	[SerializeField]
	private GameObject[] robotTag;
    [SerializeField]
    private GameObject light;

    public Rigidbody rb;

    [SerializeField]
    private Transform centerOfMass;
    public Transform ball;

	[SerializeField]
	private WheelCollider rightWheel;
	[SerializeField]
	private WheelCollider leftWheel;

    private void Start() {
        // !TODO o centro de massa faz os robôs girarem feito loucos
        //rb.centerOfMass = centerOfMass.position;
        lastPos = Vector3.zero;
    }

    private void Update() {
		// armazena aqui pra evitar consultas desnecessárias em tempo de execução
		leftTorque = leftWheel.motorTorque;
		rightTorque = rightWheel.motorTorque;

        // desacelera o robô de acordo com o fator de desaceleração e o tempo
		if (InRange (leftTorque)) {
			leftWheel.motorTorque += (rightTorque < 0 ? -1 : 1) * Time.deltaTime * fatorDeDesaceleracao;
		} else {
			leftWheel.motorTorque = 0;
		}
		if (InRange (rightTorque)) {
			rightWheel.motorTorque += (rightTorque < 0 ? -1 : 1) * Time.deltaTime * fatorDeDesaceleracao;
		} else {
			rightWheel.motorTorque = 0;
		}

        // calcula o desempenho
        desempenho += 10 / (1 + 10 * Mathf.Pow(Vector3.Distance(transform.position, ball.position), 3));
        //Debug.Log(1 / (1 + Mathf.Pow(Vector3.Distance(transform.position, lastPos), 2)));
        desempenho -= 1 / (1 + Mathf.Pow(Vector3.Distance(transform.position, lastPos), 2));

        lastPos = transform.position;
	}
		
	public void ApplyVel(float left, float right) {
        // a frente do robô é a tag maior, multiplica valores por -1 pra não ter que rotacionar no robô
        // multiplica por 5 porque a saída da rede é entre [0, 1] e a velocidade máxima é 5
        if (left != 0)
			leftWheel.motorTorque = -left;
		if (right != 0)
			rightWheel.motorTorque = -right;
	}

    public void SetWorldDestination(Vector2 dest) {
        Debug.Log("Going to (" + dest.x + ", " + dest.y + ")");

        // !TODO ajustar orientação (subtrair onde seria o zero)
        float currentTheta = transform.rotation.y * Mathf.Deg2Rad;
        Vector2 currentPos = transform.position;
        float d = 1f;
        float targetTheta = (Mathf.Atan2((dest.y - currentPos.y) * 1.3f / 480, (dest.x - currentPos.x) * 1.5f / 640));
        bool backward;
        int m;
        float Vr, Vl, V = 0, vmax = 5;

        currentTheta = Mathf.Atan2(Mathf.Sin(currentTheta), Mathf.Cos(currentTheta));

        if (((Mathf.Atan2(Mathf.Sin(targetTheta - currentTheta + Mathf.PI / 2), Mathf.Cos(targetTheta - currentTheta + Mathf.PI / 2)))) < 0) {
            backward = true;
        }
        else {
            backward = false;
        }
        if (backward) {
            currentTheta = currentTheta + Mathf.PI;
            currentTheta = Mathf.Atan2(Mathf.Sin(currentTheta), Mathf.Cos(currentTheta));
        }


        if (backward) {
            m = -1;
        }
        else {
            m = 1;
        }

        // !TODO esse fixedPos é setado em outro lugar do código do controle
        bool fixedPos = false;

        if (fixedPos) {
            d = Mathf.Sqrt(Mathf.Pow((dest.y - currentPos.y), 2) + Mathf.Pow((dest.x - currentPos.x), 2)) / 30;

            if (d > 1)
                d = 1;
            else {
                // !TODO histWipe();
                if (d < 0.3)
                    d = 0;
            }
        }

        float LOS = Mathf.Atan2(Mathf.Sin(targetTheta - currentTheta), Mathf.Cos(targetTheta - currentTheta));



        if (fixedPos && ((LOS < Mathf.PI / 6 && LOS > -Mathf.PI / 6) || (LOS > 5 * Mathf.PI / 6 && LOS < -5 * Mathf.PI / 6))) {
            Vr = (d * (m - Mathf.Sin(LOS)));
            Vl = (d * (m + Mathf.Sin(LOS)));

        }
        else if (fixedPos && (Mathf.Sqrt(Mathf.Pow((dest.y - currentPos.y), 2) + Mathf.Pow((dest.x - currentPos.x), 2)) < 30)) {
            Vl = (Mathf.Abs(LOS) / LOS) * (d * (1 + Mathf.Sin(Mathf.Abs(LOS)))) / 8;
            Vr = -(Mathf.Abs(LOS) / LOS) * (d * (1 + Mathf.Sin(Mathf.Abs(LOS)))) / 8;

        }
        else {

            Vr = (d * (m - Mathf.Sin(LOS)));
            Vl = (d * (m + Mathf.Sin(LOS)));
        }
        //cout<<"LOS "<<LOS*180/PI<<" | "<<Vr<<" | "<<Vl<<" | d "<<d<<endl;


        if (Mathf.Abs(Vl) > 1) {
            Vl = 1 * Vl / Mathf.Abs(Vl);
        }
        if (Mathf.Abs(Vr) > 1) {
            Vr = 1 * Vr / Mathf.Abs(Vr);
        }

        // !TODO atualizar status
        int status = 0, role = 3;
        int previous_status;

        switch (status) {
            // 0 = estado de jogo, 1 = pegando a bola, 2 = acelerando
            case 0:
                //if(V<vmax)
                //V=V+0.05;
                //else if(V>vmax)
                //V=V-0.05;
                //else
                V = vmax;
                break;
            case 1:
                V = V - 0.2f;
                if (V < vmax - 1)
                    V = vmax - 1;
                break;
            case 2:
                V = V + 0.1f;
                if (V > vmax + 1)
                    V = vmax + 1;
                break;
            case 3:
                V = vmax;
                break;
            case 4:
                //std::cout << "GIRA GIRA GIRA"<<endl; ;
                V = vmax + 1;
                if (currentPos.y < dest.y) {
                    if (role == 3) {
                        Vl = 1;
                        Vr = -1;
                    }
                    else {
                        Vr = 1;
                        Vl = -1;
                    }
                }
                else {
                    if (role == 3) {
                        Vl = -1;
                        Vr = 1;
                    }
                    else {
                        Vr = -1;
                        Vl = 1;
                    }
                }
                break;
            default:
                V = vmax;
                break;
        }
        previous_status = status;
        //std::cout << "Vr " <<Vr<<"Vl "<<Vl<<std::endl;
        Vl = V * Vl;
        Vr = V * Vr;


        if (check_collision()) {
            //verifica se esta perto da bola
            if (Mathf.Sqrt(Mathf.Pow((Ball.y - currentPos.y), 2) + Mathf.Pow((Ball.x - currentPos.x), 2)) < 20) {
                //std::cout<<"gira"<<std::endl;
                //verifica o lado que deve girar
                if (role == 3) {
                    if (dest.y < 240) {
                        Vr = vmax;
                        Vl = -vmax;
                        spin = true;
                    }
                    else {
                        Vr = -vmax;
                        Vl = vmax;
                        spin = true;
                    }
                }
                else {
                    if (dest.y > 240) {
                        Vr = vmax;
                        Vl = -vmax;
                        spin = true;
                    }
                    else {
                        Vr = -vmax;
                        Vl = vmax;
                        spin = true;
                    }


                }
            }
            else { //se nao esta perto da bola, o robo deve dar re
                if (spin) {
                    spin = false;
                    count_collisions = MAX_COLLISIONS;
                    histWipe();
                    return;
                }
                else {
                    float temp = -vmax * (Vr + Vl) / Mathf.Abs(Vr + Vl);
                    Vr = temp;
                    Vl = temp;
                }
            }
        }
    }

	public void SetDestination(Vector2 dest) {
        SetWorldDestination(new Vector2(VSS.ConvertPosition(dest.x, false), VSS.ConvertPosition(dest.y, true)));
	}

	public void SetTag(bool isEnemy, int id) {
		robotTag [id + (isEnemy? 3 : 0)].SetActive (true);
	}

    /* como o torque na roda pode ser positiva ou negativa, não podemos somar um valor maior que o faltante
     * para zerar o torque. esse cálculo verifica se a soma vai causar um loop infinito fazendo com que o robô
     * nunca pare
     */
    private bool InRange(float n) {
		if (Mathf.Abs(n) <= 2 * Time.deltaTime * fatorDeDesaceleracao && n != 0)
			return true;
		return false;
	}

    private void OnCollisionEnter(Collision collision) {
        if(collision.transform.CompareTag("Barrier")) {
            desempenho -= 100;
        }
        else if (collision.transform.CompareTag("Ball")) {
            desempenho += 100;
        }
    }

    public void Glow(bool now) {
        if (now)
            light.SetActive(true);
        else
            light.SetActive(false);
    }
}
