using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Goal : MonoBehaviour {

    [SerializeField]
    private VSS vss;

    [SerializeField]
    private bool rightGoal;

    private void OnTriggerEnter(Collider other) {
        if(other.CompareTag("Ball")) {
            vss.AnnounceGoal(rightGoal);
        }
    }

}
