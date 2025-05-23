class HeepIPAddress:

	def __init__(self, Octet4, Octet3, Octet2, Octet1):
		self.Octet4 = Octet4
		self.Octet3 = Octet3
		self.Octet2 = Octet2
		self.Octet1 = Octet1
		return

	def GetIPAsString(self) :
		myString = str(self.Octet4) + '.' + str(self.Octet3) + '.' + str(self.Octet2) + '.' + str(self.Octet1)
		return myString

	def GetIPAsByteArray(self) :
		byteArray = []
		byteArray.append(chr(self.Octet4))
		byteArray.append(chr(self.Octet3))
		byteArray.append(chr(self.Octet2))
		byteArray.append(chr(self.Octet1))

		return byteArray

	def SetIPFromString(self, theString) :
		splitString = theString.split('.')
		if len(splitString) != 4 :
			return

		self.Octet4 = int(splitString[0])
		self.Octet3 = int(splitString[1])
		self.Octet2 = int(splitString[2])
		self.Octet1 = int(splitString[3])