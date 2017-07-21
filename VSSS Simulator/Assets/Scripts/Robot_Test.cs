using UnityEngine;

public class Robot_Test : MonoBehaviour {

	[SerializeField]
	private float robotVel = 10;

	private Robot robot;

	private void Awake() {
		robot = GetComponent<Robot> ();
	}
	private void Update () {
		if (Input.GetKey ("q")) {
			robot.ApplyVel (1 * robotVel, 0);
		}
		if (Input.GetKey ("e")) {
			robot.ApplyVel (0, 1 * robotVel);
		}
		if (Input.GetKey ("a")) {
			robot.ApplyVel (-1 * robotVel, 0);
		}
		if (Input.GetKey ("d")) {
			robot.ApplyVel (0, -1 * robotVel);
		}
	}
}
