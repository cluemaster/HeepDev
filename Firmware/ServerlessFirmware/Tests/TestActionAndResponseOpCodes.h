#include "../ActionAndResponseOpCodes.h"
#include "UnitTestSystem.h"

void PrintOutputBuffer()
{
	for(int i = 0; i < outputBufferLastByte; i++)
	{
		cout << (int)outputBuffer[i] << " ";
	}
	cout << endl;
}

void PrintBuffer(unsigned char *buffer, unsigned long bufferSize)
{
	for(int i = 0; i < bufferSize; i++)
	{
		cout << (int)buffer[i] << " ";
	}
	cout << endl;
}

void TestClearOutputBufferAndAddChar()
{
	std::string TestName = "Add Char to Output Buffer and Clear";

	ClearOutputBuffer();
	AddNewCharToOutputBuffer('3');

	ExpectedValue valueList [2];
	valueList[0].valueName = "Num Chars before clear";
	valueList[0].expectedValue = 1;
	valueList[0].actualValue = outputBufferLastByte;

	ClearOutputBuffer();

	valueList[1].valueName = "Num Chars after clear";
	valueList[1].expectedValue = 0;
	valueList[1].actualValue = outputBufferLastByte;

	CheckResults(TestName, valueList, 2);
}

void TestMemoryDumpROP()
{
	std::string TestName = "Memory Dump ROP";

	for(int i = 0; i < STANDARD_ID_SIZE; i++)
	{
		deviceIDByte[i] = i+1;
	}

	ClearDeviceMemory();
	SetDeviceName("Jacob");
	ClearOutputBuffer();
	FillOutputBufferWithMemoryDump();

	PrintOutputBuffer();

	ExpectedValue valueList[10];
	valueList[0].valueName = "Memory Dump";
	valueList[0].expectedValue = MemoryDumpOpCode;
	valueList[0].actualValue = outputBuffer[0];

	valueList[1].valueName = "Device ID 1";
	valueList[1].expectedValue = 1;
	valueList[1].actualValue = outputBuffer[1];

	valueList[2].valueName = "Device ID 2";
	valueList[2].expectedValue = 2;
	valueList[2].actualValue = outputBuffer[2];

	valueList[3].valueName = "Device ID 3";
	valueList[3].expectedValue = 3;
	valueList[3].actualValue = outputBuffer[3];

	valueList[4].valueName = "Device ID 4";
	valueList[4].expectedValue = 4;
	valueList[4].actualValue = outputBuffer[4];

	CheckResults(TestName, valueList, 5);
}

void TestHeepDeviceCOP()
{
	std::string TestName = "Is Heep Device COP";

	ClearDeviceMemory();
	SetDeviceName("Jacob");
	ClearOutputBuffer();
	ClearInputBuffer();

	inputBuffer[0] = 0x09;
	inputBuffer[1] = 0x00;
	ExecuteControlOpCodes();

	ExpectedValue valueList[10];
	valueList[0].valueName = "Memory Dump";
	valueList[0].expectedValue = MemoryDumpOpCode;
	valueList[0].actualValue = outputBuffer[0];

	valueList[1].valueName = "Device ID 1";
	valueList[1].expectedValue = 1;
	valueList[1].actualValue = outputBuffer[1];

	valueList[2].valueName = "Device ID 2";
	valueList[2].expectedValue = 2;
	valueList[2].actualValue = outputBuffer[2];

	valueList[3].valueName = "Device ID 3";
	valueList[3].expectedValue = 3;
	valueList[3].actualValue = outputBuffer[3];

	valueList[4].valueName = "Device ID 4";
	valueList[4].expectedValue = 4;
	valueList[4].actualValue = outputBuffer[4];

	CheckResults(TestName, valueList, 5);
}

void TestNumberFromBuffer()
{
	std::string TestName = "Get Number from Buffer";

	unsigned char myBuffer [4];
	myBuffer[0] = 0x01;
	myBuffer[1] = 0xFF;

	unsigned int counter = 0;
	unsigned int retVal = GetNumberFromBuffer(myBuffer, &counter, 2);

	ExpectedValue valueList[1];
	valueList[0].valueName = "Returned Number";
	valueList[0].expectedValue = 511;
	valueList[0].actualValue = retVal;

	CheckResults(TestName, valueList, 1);
}

void TestSetValSuccess()
{
	std::string TestName = "Test Set Val COP";

	ClearControls();
	SetDeviceName("Test");
	Control theControl;
	theControl.controlName = "Test Control";
	theControl.controlID = 0;
	theControl.controlDirection = 1;
	theControl.controlType = 1;
	theControl.highValue= 100;
	theControl.lowValue = 0;
	theControl.curValue = 50;
	AddControl(theControl);

	ClearInputBuffer();
	inputBuffer[0] = 0x0A;
	inputBuffer[1] = 0x02;
	inputBuffer[2] = 0x00;
	inputBuffer[3] = 0x04;
	ExecuteControlOpCodes();

	ExpectedValue valueList[2];
	valueList[0].valueName = "Returned Op Code";
	valueList[0].expectedValue = SuccessOpCode;
	valueList[0].actualValue = outputBuffer[0];

	valueList[1].valueName = "Control Value";
	valueList[1].expectedValue = 4;
	valueList[1].actualValue = GetControlValueByID(0);

	CheckResults(TestName, valueList, 2);
}

void TestSetValFailure()
{
	std::string TestName = "Test Set Val COP Failures";

	ClearControls();
	SetDeviceName("Test");
	Control theControl;
	theControl.controlName = "Test Control";
	theControl.controlID = 0;
	theControl.controlDirection = 1;
	theControl.controlType = 1;
	theControl.highValue= 100;
	theControl.lowValue = 0;
	theControl.curValue = 50;
	AddControl(theControl);

	ClearInputBuffer();
	inputBuffer[0] = 0x0A;
	inputBuffer[1] = 0x02;
	inputBuffer[2] = 0x01;
	inputBuffer[3] = 0x04;
	ExecuteControlOpCodes();

	ExpectedValue valueList[2];
	valueList[0].valueName = "Returned Op Code";
	valueList[0].expectedValue = ErrorOpCode;
	valueList[0].actualValue = outputBuffer[0];

	valueList[1].valueName = "Control Value";
	valueList[1].expectedValue = 50;
	valueList[1].actualValue = GetControlValueByID(0);

	CheckResults(TestName, valueList, 2);
}

void TestSetPositionOpCode()
{
	std::string TestName = "Test Set Position COP";

	ClearControls();
	SetDeviceName("Test");

	ClearInputBuffer();
	inputBuffer[0] = 0x0B;
	inputBuffer[1] = 0x04;
	inputBuffer[2] = 0x01;
	inputBuffer[3] = 0x01;
	inputBuffer[4] = 0x10;
	inputBuffer[5] = 0x10;
	ExecuteControlOpCodes();
	heepByte deviceID [STANDARD_ID_SIZE] = {0x06, 0x04, 0x06, 0x01};
	int x = 0; int y = 0; unsigned int xyMemPosition = 0; 
	GetXYFromMemory_Byte(&x, &y, deviceID, &xyMemPosition);

	ExpectedValue valueList[4];
	valueList[0].valueName = "x";
	valueList[0].expectedValue = 0x0101;
	valueList[0].actualValue = x;

	valueList[1].valueName = "y";
	valueList[1].expectedValue = 0x1010;
	valueList[1].actualValue = y;

	ClearInputBuffer();
	inputBuffer[0] = 0x0B;
	inputBuffer[1] = 0x04;
	inputBuffer[2] = 0xF1;
	inputBuffer[3] = 0x02;
	inputBuffer[4] = 0xB2;
	inputBuffer[5] = 0x3C;
	ExecuteControlOpCodes();
	GetXYFromMemory_Byte(&x, &y, deviceID, &xyMemPosition);

	valueList[2].valueName = "x";
	valueList[2].expectedValue = 0xF102;
	valueList[2].actualValue = x;

	valueList[3].valueName = "y";
	valueList[3].expectedValue = 0xB23C;
	valueList[3].actualValue = y;

	CheckResults(TestName, valueList, 4);
}

void TestSetVertxCOP()
{
	std::string TestName = "Test Set Vertex COP";

	ClearVertices();
	ClearDeviceMemory();
	ClearInputBuffer();

	inputBuffer[0] = 0x0C;
	inputBuffer[1] = 0x04;

	inputBuffer[2] = 0xF1;
	inputBuffer[3] = 0x02;
	inputBuffer[4] = 0xB2;
	inputBuffer[5] = 0x3C;

	inputBuffer[6] = 0x1A;
	inputBuffer[7] = 0x2D;
	inputBuffer[8] = 0x40;
	inputBuffer[9] = 0x02;

	inputBuffer[10] = 0x01;
	inputBuffer[11] = 0x02;

	inputBuffer[12] = 0xC0;
	inputBuffer[13] = 0xD0;
	inputBuffer[14] = 0x20;
	inputBuffer[15] = 0x02;
	ExecuteControlOpCodes();

	Vertex_Byte newVertex;
	int success = GetVertexAtPointer_Byte(vertexPointerList[0], &newVertex);

	heepByte trueTxID [STANDARD_ID_SIZE] = {0xF1,0x02,0xB2,0x3C};
	heepByte trueRxID [STANDARD_ID_SIZE] = {0x1A, 0x2D, 0x40, 0x02};

	ExpectedValue valueList [8];
	valueList[0].valueName = "TXID";
	valueList[0].expectedValue = 1;
	valueList[0].actualValue = CheckBufferEquality(newVertex.txID, trueTxID, STANDARD_ID_SIZE);

	valueList[1].valueName = "RXID";
	valueList[1].expectedValue = 1;
	valueList[1].actualValue = CheckBufferEquality(newVertex.rxID, trueRxID, STANDARD_ID_SIZE);

	valueList[2].valueName = "TX Control ID";
	valueList[2].expectedValue = 0x01;
	valueList[2].actualValue = newVertex.txControlID;

	valueList[3].valueName = "RX Control ID";
	valueList[3].expectedValue = 0x02;
	valueList[3].actualValue = newVertex.rxControlID;

	valueList[4].valueName = "IP 4";
	valueList[4].expectedValue = 0xC0;
	valueList[4].actualValue = newVertex.rxIPAddress.Octet4;

	valueList[5].valueName = "IP 3";
	valueList[5].expectedValue = 0xD0;
	valueList[5].actualValue = newVertex.rxIPAddress.Octet3;

	valueList[6].valueName = "IP 2";
	valueList[6].expectedValue = 0x20;
	valueList[6].actualValue = newVertex.rxIPAddress.Octet2;

	valueList[7].valueName = "IP 1";
	valueList[7].expectedValue = 0x02;
	valueList[7].actualValue = newVertex.rxIPAddress.Octet1;

	CheckResults(TestName, valueList, 8);
}

void TestAddMOPOpCode()
{
	std::string TestName = "Test Set MOP COP";

	ClearDeviceMemory();
	ClearInputBuffer();

	// Add a random clients name
	inputBuffer[0] = 0x13;
	inputBuffer[1] = 0x0B;

	inputBuffer[2] = 0x06;
	inputBuffer[3] = 0x01;
	inputBuffer[4] = 0x02;
	inputBuffer[5] = 0x03;
	inputBuffer[6] = 0x04;
	inputBuffer[7] = 0x05;

	inputBuffer[8] = 'J';
	inputBuffer[9] = 'a';
	inputBuffer[10] = 'm';

	inputBuffer[11] = 'e';
	inputBuffer[12] = 's';

	unsigned int beforeMemory = curFilledMemory;

	ExecuteControlOpCodes();

	unsigned int afterMemory = curFilledMemory;

	// Traverse the new memory by updating XY twice
	heepByte deviceID[STANDARD_ID_SIZE] = {0x01, 0x02, 0x03, 0x04};
	UpdateXYInMemory_Byte(1234, 161, deviceID);
	unsigned int beforeTraversal = curFilledMemory;

	UpdateXYInMemory_Byte(2321, 5101, deviceID);
	unsigned int afterTraveresal = curFilledMemory;

#ifdef USE_INDEXED_IDS
	unsigned int expectedMemory = ID_SIZE + STANDARD_ID_SIZE + 2 + 2 + ID_SIZE + 5 + 2 + ID_SIZE + 4;
#else
	unsigned int expectedMemory = 21;
#endif

	ExpectedValue valueList [4];
	valueList[0].valueName = "Before Memory Size";
	valueList[0].expectedValue = 0;
	valueList[0].actualValue = beforeMemory;

	valueList[1].valueName = "After Memory Size";
#ifdef USE_INDEXED_IDS
	valueList[1].expectedValue = ID_SIZE + STANDARD_ID_SIZE + 2 + 2 + ID_SIZE + 5;
#else
	valueList[1].expectedValue = 11;
#endif
	valueList[1].actualValue = afterMemory;

	valueList[2].valueName = "Before Traversal Memory Size";
	valueList[2].expectedValue = expectedMemory;
	valueList[2].actualValue = beforeTraversal;

	valueList[3].valueName = "After Traversal Memory Size";
	valueList[3].expectedValue = expectedMemory;
	valueList[3].actualValue = afterTraveresal;

	CheckResults(TestName, valueList, 4);
}

void TestDeleteMOPOpCode()
{
	std::string TestName = "Test Delete MOP COP";

	ClearDeviceMemory();
	ClearInputBuffer();

	heepByte deviceID[STANDARD_ID_SIZE] = {0x01, 0x02, 0x03, 0x04};
	char* device1Name = "Jacob";
	SetDeviceNameInMemory_Byte(device1Name, strlen(device1Name), deviceID);
	char* device2Name = "James";
	SetDeviceNameInMemory_Byte(device2Name, strlen(device2Name), deviceID);
	UpdateXYInMemory_Byte(1234, 161, deviceID);

	// Add a random clients name
	inputBuffer[0] = 0x15;
	inputBuffer[1] = 0x0B;

	inputBuffer[2] = 0x06;
	inputBuffer[3] = 0x01;
	inputBuffer[4] = 0x02;
	inputBuffer[5] = 0x03;
	inputBuffer[6] = 0x04;
	inputBuffer[7] = 0x05;

	inputBuffer[8] = 'J';
	inputBuffer[9] = 'a';
	inputBuffer[10] = 'm';

	inputBuffer[11] = 'e';
	inputBuffer[12] = 's';

#ifdef USE_INDEXED_IDS
	unsigned char valAtSpotBeforeDeleteion = deviceMemory[15];
#else
	unsigned char valAtSpotBeforeDeleteion = deviceMemory[11];
#endif
	
	ExecuteControlOpCodes();

#ifdef USE_INDEXED_IDS
	unsigned char valAtSpotAfterDeletion = deviceMemory[15];
#else
	unsigned char valAtSpotAfterDeletion = deviceMemory[11];
#endif

	ExpectedValue valueList [2];
	valueList[0].valueName = "Before OpCode";
	valueList[0].expectedValue = DeviceNameOpCode;
	valueList[0].actualValue = valAtSpotBeforeDeleteion;

	valueList[1].valueName = "After OpCode";
	valueList[1].expectedValue = FragmentOpCode;
	valueList[1].actualValue = valAtSpotAfterDeletion;

	CheckResults(TestName, valueList, 2);
}


void TestFindNextAckIndex()
{
	std::string TestName = "Test Find Next Ack Index";

	ClearAckBuffer();
	ackBuffer[0] = 10; // Timeout
	ackBuffer[1] = 0; // Num Retries
	ackBuffer[2] = 0x02;
	ackBuffer[3] = 0x04;
	ackBuffer[4] = 0x01;
	ackBuffer[5] = 0x02;
	ackBuffer[6] = 0x03;
	ackBuffer[7] = 0x04;

	ackBuffer[8] = 4; // Timeout
	ackBuffer[9] = 1; // Num Retries
	ackBuffer[10] = 0x09;
	ackBuffer[11] = 0x00;

	ackBuffer[12] = 2; // Timeout
	ackBuffer[13] = 0; // Num Retries
	ackBuffer[14] = 0x02;
	ackBuffer[15] = 0x02;
	ackBuffer[16] = 0x03;
	ackBuffer[17] = 0x09;

	int nextindex = GetNextOpenAckPositionPointer();

	ExpectedValue valueList[1];
	valueList[0].valueName = "Index";
	valueList[0].expectedValue = 18;
	valueList[0].actualValue = nextindex;

	CheckResults(TestName, valueList, 1);
}

void TestByteDisplacement()
{
	std::string TestName = "Test Byte Displacement";

	heepByte valueLeft = 10;
	heepByte valueRight = 30;
	heepByte Displacement1 = GetByteValueDisplacementMovingRight(valueLeft, valueRight);

	valueLeft = 130;
	valueRight = 30;
	heepByte Displacement2 = GetByteValueDisplacementMovingRight(valueLeft, valueRight);

	ExpectedValue valueList[2];
	valueList[0].valueName = "Displacement no Wrap";
	valueList[0].expectedValue = 20;
	valueList[0].actualValue = Displacement1;

	valueList[1].valueName = "Displacement Wrap";
	valueList[1].expectedValue = 155;
	valueList[1].actualValue = Displacement2;

	CheckResults(TestName, valueList, 2);
}

void TestDeleteAckAtIndex()
{
	std::string TestName = "Test Delete Ack at Index";

	ClearAckBuffer();
	ackBuffer[0] = 10; // Timeout
	ackBuffer[1] = 0; // Num Retries
	ackBuffer[2] = 0x02;
	ackBuffer[3] = 0x04;
	ackBuffer[4] = 0x01;
	ackBuffer[5] = 0x02;
	ackBuffer[6] = 0x03;
	ackBuffer[7] = 0x04;

	ackBuffer[8] = 4; // Timeout
	ackBuffer[9] = 1; // Num Retries
	ackBuffer[10] = 0x09;
	ackBuffer[11] = 0x00;

	ackBuffer[12] = 2; // Timeout
	ackBuffer[13] = 0; // Num Retries
	ackBuffer[14] = 0x02;
	ackBuffer[15] = 0x02;
	ackBuffer[16] = 0x03;
	ackBuffer[17] = 0x09;

	DeleteAckDataAtIndex(8);
	int valueAt8 = ackBuffer[8];

	DeleteAckDataAtIndex(0);
	int valueAt01 = ackBuffer[0];

	DeleteAckDataAtIndex(0);
	int valueAt02 = ackBuffer[0];

	ExpectedValue valueList[3];
	valueList[0].valueName = "Value After 1 Delete";
	valueList[0].expectedValue = 2;
	valueList[0].actualValue = valueAt8;

	valueList[1].valueName = "Value After 2 Delete";
	valueList[1].expectedValue = 2;
	valueList[1].actualValue = valueAt01;

	valueList[2].valueName = "Value After 3 Delete";
	valueList[2].expectedValue = 0;
	valueList[2].actualValue = valueAt02;

	CheckResults(TestName, valueList, 3);
}

void TestHandleAckBufferTimeouts()
{
	std::string TestName = "Test Ack Timeout";

	ClearAckBuffer();
	ackBuffer[0] = 10; // Timeout
	ackBuffer[1] = 0; // Num Retries
	ackBuffer[2] = 0x02;
	ackBuffer[3] = 0x04;
	ackBuffer[4] = 0x01;
	ackBuffer[5] = 0x02;
	ackBuffer[6] = 0x03;
	ackBuffer[7] = 0x04;

	ackBuffer[8] = 253; // Timeout
	ackBuffer[9] = 1; // Num Retries
	ackBuffer[10] = 0x09;
	ackBuffer[11] = 0x00;

	ackBuffer[12] = 246; // Timeout
	ackBuffer[13] = 0; // Num Retries
	ackBuffer[14] = 0x02;
	ackBuffer[15] = 0x02;
	ackBuffer[16] = 0x03;
	ackBuffer[17] = 0x09;

	HandleAckBufferTimeouts();

	ExpectedValue valueList[3];
	valueList[0].valueName = "Value at 0 after Delete";
	valueList[0].expectedValue = 253;
	valueList[0].actualValue = ackBuffer[0];

	valueList[1].valueName = "Proof of Retry";
	valueList[1].expectedValue = 2;
	valueList[1].actualValue = ackBuffer[1];

	valueList[2].valueName = "Data moved on successfully";
	valueList[2].expectedValue = 246;
	valueList[2].actualValue = ackBuffer[4];

	CheckResults(TestName, valueList, 3);
}

void TestActionAndResponseOpCodes()
{
	TestClearOutputBufferAndAddChar();
	TestMemoryDumpROP();
	TestHeepDeviceCOP();
	TestNumberFromBuffer();
	TestSetValSuccess();
	TestSetValFailure();
	TestSetPositionOpCode();
	TestSetVertxCOP();
	TestAddMOPOpCode();
	TestDeleteMOPOpCode();
	TestFindNextAckIndex();
	TestByteDisplacement();
	TestDeleteAckAtIndex();
	TestHandleAckBufferTimeouts();
}