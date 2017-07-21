using UnityEngine;
using UnityEngine.UI;

public class Game_UI : MonoBehaviour {

    [SerializeField]
    private GameObject telaMenu;
    [SerializeField]
    private GameObject telaInGame;
    [SerializeField]
    private GameObject telaFileLocation;

    [SerializeField]
    private Text txtScore;

    [SerializeField]
    private InputField inputFile;

    [SerializeField]
    private NEAT neat;

    private bool leftSavePopupOpened;

    private void Start() {
        inputFile.text = Application.dataPath + "/vss.ann";
    }

    public void StartNeat() {
        telaMenu.SetActive(false);
        telaInGame.SetActive(true);
    }

	public void UpdateScore(int team1, int team2) {
        txtScore.text = team1.ToString() + " X " + team2.ToString();
    }

    public void OpenFilePopup(bool left) {
        Time.timeScale = 0;
        telaFileLocation.SetActive(true);
        leftSavePopupOpened = left;
    }
    public void CloseFilePopup() {
        Time.timeScale = 1;
        telaFileLocation.SetActive(false);
    }

    public void SaveAnn() {
        if (inputFile.text == "")
            return;
        neat.SaveAnn(leftSavePopupOpened, inputFile.text);
        telaFileLocation.SetActive(false);
        Time.timeScale = 1;
    }

}