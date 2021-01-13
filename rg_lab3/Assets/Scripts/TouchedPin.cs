using UnityEngine;

public class TouchedPin : MonoBehaviour
{

    public BallControl ballControl;

    void OnCollisionEnter(Collision collisionInfo)
    {
        if(collisionInfo.collider.tag == "pin")
        {
            ballControl.enabled = false;
        }
    }
}
