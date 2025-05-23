﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using Heep;

public class FakeDevice : MonoBehaviour {

	HeepDevice myDevice;

	public GameObject theImage;
	public bool sendAnalytics;
	public Button OutputOnBtn;
	public Button OutputOffBtn;
	public Button SendBufferBtn;
	public InputField bufferField;

	void OnApplicationQuit()
	{
		Debug.Log("Application ending after " + Time.time + " seconds");
		myDevice.CloseDevice ();
	}

	void CreateHeepDevice()
	{
		List <byte> ID = new List<byte>();
		for(byte i = 0; i < 4; i++)
		{
			ID.Add(i);
		}

		DeviceID myID = new DeviceID(ID);
		myDevice = new HeepDevice (myID);

		myDevice.LoadDeviceMemoryFromFile ();

		Control theControl = Control.CreateControl (Control.CtrlInputOutput.input, Control.CtrlType.OnOff, "First", sendAnalytics);
		myDevice.AddControl (theControl);
		Control newControl = Control.CreateControl (Control.CtrlInputOutput.output, Control.CtrlType.OnOff, "Second", sendAnalytics);
		myDevice.AddControl (newControl);
		Control bufferControl = new BufferControl (0, Control.CtrlInputOutput.output, Control.CtrlType.buffer, 10, 0, 0, "Buffer", true);
		myDevice.AddControl (bufferControl);
		myDevice.SetDeviceNameStartup ("Unity");
		myDevice.StartListening ();
	}

	IEnumerator setControlsOnTimer()
	{
		int curValue = 0;

		while (true) {
			Debug.Log ("Setting to " + curValue);
			myDevice.SetControlByID (1, curValue);
			yield return new WaitForSeconds (2);

			if (curValue == 0)
				curValue = 1;
			else
				curValue = 0;
		}
	}

	public void SetOutputOn()
	{
		myDevice.SetControlByID (1, 1);
	}

	public void SetOutputOff()
	{
		myDevice.SetControlByID (1, 0);
	}

	public void SendBuffer()
	{
		string bufferData = bufferField.text;
		Debug.Log (bufferData);

		List<byte> bufferList = new List<byte> ();
		for (int i = 0; i < bufferData.Length; i++) {
			bufferList.Add ((byte)bufferData [i]);
		}

		myDevice.SetControlBufferByID (2, bufferList);
	}

	// Use this for initialization
	void Start () {
		CreateHeepDevice ();
		//StartCoroutine (setControlsOnTimer());
		OutputOnBtn.onClick.AddListener(SetOutputOn);
		OutputOffBtn.onClick.AddListener(SetOutputOff);
		SendBufferBtn.onClick.AddListener (SendBuffer);
	}

	// Update is called once per frame
	void Update () {
		if (Input.GetKey("escape"))
			Application.Quit();

		if (myDevice.GetControlValueByID (0) == 0) {
			theImage.GetComponent<Image> ().color = Color.red;
		} else {
			theImage.GetComponent<Image> ().color = Color.blue;
		}
			
	}
}
