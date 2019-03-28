using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MobileSupport : MonoBehaviour
{

    private void Awake()
    {
#if UNITY_ANDROID || UNITY_IOS
        GetComponent<Canvas>().enabled = true;
#endif
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
