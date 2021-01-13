using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class BallControl : MonoBehaviour
{

    public float forwardSpeed = 100000f;
    public float turnSpeed = 4000f;
    public float pushSpeed = 1500f;
    public Rigidbody rb;
    public bool pressedRight = false;
    public bool pressedLeft = false;
    public bool random;
    // Start is called before the first frame update
    void Start()
    {
        rb.AddForce(0, 0, forwardSpeed * Time.deltaTime);
        var rand = new System.Random();
        random = rand.Next(2) == 0;
    }

    void Update()
    {
        if (Input.GetKey(KeyCode.D))
        {
            pressedRight = true;
        }

        if (pressedRight && !Input.GetKey(KeyCode.D))
        {
            pressedRight = false;
        }

        if (Input.GetKey(KeyCode.A))
        {
            pressedLeft = true;
        }

        if (pressedLeft && !Input.GetKey(KeyCode.A))
        {
            pressedLeft = false;
        }
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        //adding random force left or right
        rb.AddForce(random ? -pushSpeed * Time.deltaTime : !random ? pushSpeed * Time.deltaTime : 0,
            0, 0);
        //adding turning force
        rb.AddForce(pressedLeft ? -turnSpeed * Time.deltaTime : pressedRight ? turnSpeed * Time.deltaTime : 0,
            0, 0);
    }
}
