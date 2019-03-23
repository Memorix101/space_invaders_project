using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Enemy : MonoBehaviour
{
    public GameObject BulletGO;
    public GameObject ExploGO;

    private Rigidbody2D rb2D;

    private float enemyPosX, enemyPosY;
    private bool goLeft = false;
    private float shootTimer;
    public bool shoot = false;
    private float shootTimeLimit;
    private float startPos;
    private const int moveSpeed = 2;

    private int spriteSize = 1; //27px in Unity units

    private GameObject player;

    // Start is called before the first frame update
    void Start()
    {
        player = GameObject.FindGameObjectWithTag("Player");

        startPos = transform.position.x;
        enemyPosX = transform.position.x;
        enemyPosY = transform.position.y;
        shootTimer = 0f;

        shootTimeLimit = Random.Range(3, 30);

        goLeft = false;
        shoot = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (!goLeft)
        {
            enemyPosX += moveSpeed * Time.deltaTime;
        }

        if (goLeft)
        {
            enemyPosX -= moveSpeed * Time.deltaTime;
        }

        if (enemyPosX >= startPos + spriteSize && !goLeft)
        {
            goLeft = true;
        }

        if (enemyPosX <= startPos - spriteSize && goLeft)
        {
            goLeft = false;
        }

        shootTimer += 1 * Time.deltaTime;

        if (shootTimer >= shootTimeLimit)
        {
            shootTimer = 0f;
            shoot = true;

            GameObject BeamGO = Instantiate(BulletGO, transform.GetChild(0).position, Quaternion.identity);
            Rigidbody2D LaserRB = BeamGO.GetComponent<Rigidbody2D>();
            LaserRB.AddForce(-transform.up * 250f);
            Destroy(BeamGO, 5f);
        }
        else
        {
            shoot = false;
        }

        transform.position = new Vector3(enemyPosX, enemyPosY, 0);
    }

    void OnTriggerEnter2D(Collider2D col)
    {
        if (col.gameObject.tag != "EnemyBullet")
        {
            //Debug.Log(col.gameObject.name + " : " + gameObject.name + " : " + Time.time);
            GameObject GO = Instantiate(ExploGO, transform.position, Quaternion.identity);

            if(player != null)
                player.GetComponent<Player>().AddScore();

            Destroy(GO, 1f);
            Destroy(col.gameObject);
            Destroy(gameObject);
        }
    }
}
