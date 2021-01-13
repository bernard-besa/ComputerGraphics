using UnityEngine;

public class FollowPin : MonoBehaviour
{
    public Transform pinTransform;
    // Update is called once per frame
    void Update()
    {
        transform.rotation = pinTransform.rotation;
        transform.position = new Vector3(pinTransform.position.x, pinTransform.position.y + 0.4f, pinTransform.position.z);
    }
}
