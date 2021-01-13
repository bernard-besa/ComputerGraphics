using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PinKnocked : MonoBehaviour
{

    public Score score;
    public GameObject textObject;
    // Start is called before the first frame update
    void Start()
    {
        score = textObject.GetComponent<Score>();
    }

    // Update is called once per frame
    void Update()
    {

    }

    void OnTriggerEnter(Collider other)
    {
        if (other.tag == "floor")
        {
            score.UpdateScore();
            //Debug.Log("hit");
        }
    }
}
