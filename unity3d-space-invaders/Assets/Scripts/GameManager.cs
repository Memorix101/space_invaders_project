using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class GameManager : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        Screen.SetResolution(640, 480, FullScreenMode.Windowed);
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void RestartLevel()
    {
        SceneManager.LoadScene(0);
    }
}
