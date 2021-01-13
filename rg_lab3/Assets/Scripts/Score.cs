using UnityEngine;
using UnityEngine.UI;

public class Score : MonoBehaviour
{
    public int pinsKnockedDown;
    public Text scoreText;
    public Text strikeText;
    public int totalPins = 10;

    // Start is called before the first frame update

    void Update()
    {
        scoreText.text = "Pins: " + pinsKnockedDown.ToString();
    }

    public void UpdateScore()
    {
        if(pinsKnockedDown < totalPins)
        {
            pinsKnockedDown++;
        }
        if(pinsKnockedDown >= totalPins) {
            strikeText.gameObject.SetActive(true);
        }
    }
}
