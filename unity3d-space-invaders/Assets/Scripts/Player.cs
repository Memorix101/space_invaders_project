﻿using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEditor;
using UnityEngine;

public class Player : MonoBehaviour
{

    public GameObject BulletGO;
    public GameObject ExploGO;
    public GameObject GameOver_UI;
    public TextMeshProUGUI Score_TMP;

    public int score;

    private Rigidbody2D rb2D;
    private Vector2 velocity;

    private float speed = 4f;

    // Start is called before the first frame update
    void Start()
    {
        score = 0;
        rb2D = GetComponent<Rigidbody2D>();
        rb2D.gravityScale = 0;
    }

    // Update is called once per frame
    void Update()
    {
        transform.position = new Vector3(Mathf.Clamp(transform.position.x, -6f, 6f), transform.position.y, 0);

        Score_TMP.text = score.ToString("0000");

        if (Input.GetKeyDown(KeyCode.Space))
        {
            GameObject LaserGO = Instantiate(BulletGO, transform.GetChild(0).position, Quaternion.identity);
            Rigidbody2D LaserRB = LaserGO.GetComponent<Rigidbody2D>();
            LaserRB.AddForce(transform.up * 250f);
            Destroy(LaserGO, 5f);
        }
    }

    void FixedUpdate()
    {
        float xAxis = Input.GetAxis("Horizontal");
        velocity = new Vector2(xAxis, 0);
        rb2D.MovePosition(rb2D.position + velocity * Time.fixedDeltaTime * speed);
    }

    public void AddScore()
    {
        score += 100;
    }

    void OnTriggerEnter2D(Collider2D col)
    {
        if (col.gameObject.tag != "PlayerBullet")
        {
            //Debug.Log(col.gameObject.name + " : " + gameObject.name + " : " + Time.time);
            GameObject GO = Instantiate(ExploGO, transform.position, Quaternion.identity);
            Destroy(GO, 1f);
            Destroy(col.gameObject);
            GameOver_UI.SetActive(true);
            Destroy(gameObject);
        }
    }
}