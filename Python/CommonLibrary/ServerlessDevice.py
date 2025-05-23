import socket
from threading import Thread

import sys
from ControlValue import ControlValue
from Device import Device
from OutputData import OutputData
from ActionOpCodeParser import ActionOpCodeParser
from OpCodeUtilities import OpCodeUtilities

class ServerlessDeviceConnection :

	sock = socket.socket()
	TCP_PORT = 5000
	BUFFER_SIZE = 1500
	deviceData = Device()

	def __init__(self):
		return

	def SendDataToClient(self, data, IP) :
		try :
			self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.sock.connect((IP.GetIPAsString(), self.TCP_PORT))
			self.sock.send(data)
			data = self.sock.recv(self.BUFFER_SIZE)
			return data
		except :
			print 'Failed to connect to client interrupt server'
			return 'Failed'

	def SendDataDirectlyToClientIP(self, outData) :
		clientInterruptCommandByteArray = [chr(0x0A), chr(0x02), chr(outData.inputID), chr(outData.value)]
		clientInterruptCommand = OpCodeUtilities().GetStringFromByteArray(clientInterruptCommandByteArray)
		return self.SendDataToClient(clientInterruptCommand, outData.destinationIP)

	def SendOutput(self, outputID, value) :
		outputList = self.deviceData.QueueOutput(outputID, value)
		print len(outputList)

		for x in range(0, len(outputList)) :
			print self.SendDataDirectlyToClientIP(outputList[x])

	def SetDeviceData(self, deviceData) :
		self.deviceData = deviceData

	def ParseReceivedData(self, data) :
		retVal = ActionOpCodeParser().GetActionOpCodeFromByteArray(data, self.deviceData)
		return retVal

	def StartHeepDeviceServerThread(self) :
		t = Thread( target = self.StartHeepDeviceServer, args=() )
		t.setDaemon(True)
		t.start()

	def StartHeepDeviceServer(self) :
		host = ''
		backlog = 5
		size = 1024

		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.bind((host, self.TCP_PORT))
		sock.listen(backlog)

		while 1:
			client, address = sock.accept()
			data = client.recv(size)

			returnData = self.ParseReceivedData(data)

			print returnData

			if returnData:
				client.send(returnData)
				print returnData
				print self.deviceData.DeviceMemory.miscMemory

			client.close()