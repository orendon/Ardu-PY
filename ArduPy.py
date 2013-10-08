import serial,time

class Arduino:

	def __init__(self,_board='UNO',_port='/dev/ttyACM0'):
		self.port=serial.Serial(_port)
		self.board=_board
		self.port.baudrate=115200
		self.port.timeout=1
		time.sleep(1)
		if(self.board=='UNO'):
			self.maxADC=5
			self.pwmPins=[3,5,6,9,10,11]
			self.maxIO=13
		elif(self.board=='MEGA'):
			self.maxADC=15
			self.pwmPins=range(2,13)
			self.maxIO=53

	def digitalOut(self,Pin,Value=1):
		if not self.__checkParameters(Pin,Value,1):
			return -1
		command=[0x3A,0x03,0x00,Pin,Value]
		for i in command:
			self.port.write(chr(i))
		return self.__getReceivedData()

	def digitalIN(self,Pin):
		if not self.__checkParameters(Pin):
			return -1
		command=[0x3A,0x02,0x01,Pin]
		for i in command:
			self.port.write(chr(i))
		return self.__getReceivedData()

	def voltageRead(self,Pin):
		self.port.flushOutput()
		if Pin>self.maxADC:
			print "The maximum ADC pin is",self.maxADC
			return -1
		command=[0x3A,0x02,0x02,Pin]
		for i in command:
			self.port.write(chr(i))
		return self.__getReceivedData()

	def serPosition(self,Pin,Value):
		if not self.__checkParameters(Pin,Value,180):
			return -1
		command=[0x3A,0x03,0x03,Pin,Value]
		for i in command:
			self.port.write(chr(i))
		time.sleep(1)
		return self.__getReceivedData()

	def PWM(self,Pin,Value):
		if not self.__checkParameters(Pin,Value):
			return -1
		if not Pin in self.pwmPins:
			print "The select pin is not available for PWM try one of this",self.pwmPins
			return -1
		command=[0x3A,0x03,0x04,Pin,Value]
		for i in command:
			self.port.write(chr(i))
		return self.__getReceivedData()

	def toneGen(self,Pin,_Frec,dur):
		if not self.__checkParameters(Pin,Value=0):
			return -1
		Frec=_Frec
		frec=[0,0]
		while Frec>=100:
			Frec-=100;
			frec[0]+=1
		frec[1]=Frec
		command=[0x3A,0x05,0x05,Pin,frec[0],frec[1],dur]
		for i in command:
			self.port.write(chr(i))
		return self.__getReceivedData()

	def __getReceivedData(self):
		inputBuffer=self.port.readline()
		datalen=len(inputBuffer)-1
		self.port.flushInput()
		time.sleep(0.5)
		if ord(inputBuffer[0])==ord(':') and ord(inputBuffer[datalen])==0x0A:
			if ord(inputBuffer[1])==ord('K'):
				if ord(inputBuffer[2])==2:
					return ((ord(inputBuffer[3])-11)*100)+(ord(inputBuffer[4])-11)
				else:
					return ord(inputBuffer[3])
			else:
				return (256-ord(inputBuffer[3]))*-1
		else:
			return -1

	def __checkParameters(self,Pin,Value=0,maxValue=255):
		self.port.flushOutput()
		if Pin>self.maxIO or Value>maxValue:
			if Pin>self.maxIO:
				print "Select pin out of range, for this board the limit is ",self.maxIO
			else:
				print "The Max Value for that variable is ",maxValue
			return False
		else:
			return True