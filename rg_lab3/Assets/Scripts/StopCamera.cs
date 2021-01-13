using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StopCamera : MonoBehaviour
{
    public FollowPlayer followScript;
    public RectTransform scoreText;
    public GameObject tryAgain;

    void Start()
    {
        followScript = GetComponent<FollowPlayer>();
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.tag == "cameraWall")
        {
            followScript.enabled = false;
        }
    }
}
