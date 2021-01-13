using UnityEngine;
using UnityEngine.SceneManagement;
using System.Collections;

public class Restart : MonoBehaviour
{

    public BallControl ballScript;
    //public RigidBody ballRb;
    public int forwardSpeed = 560000;

    public void Start()
    {
        //ballScript = GetComponent<BallControl>();
    }

    public void RestartGame()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().name); // loads current scene
        //ballScript.Start();
        //rb.AddForce(0, 0, forwardSpeed * Time.deltaTime);
    }

}
