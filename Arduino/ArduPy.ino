#include <Servo.h> 

uint8_t maxPin=13;
uint8_t maxADC=6;
char data[10];
static uint8_t len=0;
int AT=-1;
uint8_t params[4]={0,0,0,0};
static Servo myservo;


void setup()
{
	Serial.begin(115200);
}

void loop()
{
	if(Serial.available())
	{
		readCommand();
		response(performAction());					
	}
}

int performAction()
{
	if(AT!=-1)
	{
		switch(AT)
		{
			case 0: 
				return digitalOut();
			case 1:
				return digitalIn();
			case 2:
				return voltajeRead();
			case 3:
				return serPosition();
			case 4:
				return PWM();
			case 5:
				return toneGen();
			default:
				return -4; // Bad AT Command
		}
	}
	else
	{
		return -1;
	}
}

void readCommand()
{
	if(Serial.find(":"))
	{
		delay(20);
		len=Serial.read();
		Serial.readBytes(data,len);
		AT=data[0];
		for(int i=0;i<=(len-1);i++)
		{
			params[i]=data[i+1];
		}
		Serial.flush();
		flush();
	}
}

int digitalOut() // Params={PIN,Value} Return 0 if suscesfull
{
	pinMode(params[0],OUTPUT);
	if(len!=3)
	{
		flush();
		return -3; //Bad Lengt of Data
	}
	if((params[1]!=0) && (params[1]!=1))
	{
		return -2; // Invalid Range
	}
	if(params[0]<=maxPin && params[0]>1)
	{
		digitalWrite(params[0],params[1]);
		return 0;
	}
	return -1;	// Invalid Pin
}

int digitalIn() // Params={PIN} Return the value of the PIN
{
	if(len!=2)
	{
		flush();
		return -3; //Bad Lengt of Data
	}
	if(params[0]<=maxPin && params[0]>1)
	{
		pinMode(params[0] && params[0]>1,INPUT);
		return digitalRead(params[0]);
	}
	return -1;	// Invalid Pin
}

int voltajeRead() // Params={ADC} Return the value of the Analog Read
{
	if(len!=2)
	{
		Serial.flush();
		return -3; //Bad Lengt of Data
	}
	int ADCs[]={A0,A1,A2,A3,A4,A5};
	if(params[0]<=maxADC)
	{
		return analogRead(ADCs[params[0]]);
	}
	return -1;	// Invalid Pin
}

int serPosition() // Params={PIN,Angle} Return 0 if Suscesfull 
{
	if(len!=3)
	{
		return -3; //Bad Lengt of Data
	}
	if((params[0]<=maxPin && params[0]>1)&&(params[1]<=180))
	{
		myservo.attach(params[0]);
		myservo.write(int(params[1]));
		delay(1000);
		myservo.detach();
		return 0;
	}
	if(params[1]>180)
	{
		return -2; // Invalid Range
	}
	return -1; // Invalid Pin
}

int PWM() // Params={PIN,PWM} Return 0 if Suscesfull 
{
	if(len!=3)
	{
		return -3; //Bad Lengt of Data
	}
	int pwmPins[]={3,5,6,9,10,11};
	for (int i=0;i<6;i++)
	{
		if(params[0]==pwmPins[i] && params[1]<=255)
		{
			analogWrite(params[0],params[1]);
			return 0;
		}
	}
	if(params[1]>255)
	{
		return -2;  //PWM value out of range
	}
	return -1;		//Not available PIN
}

int toneGen() // Params={PIN,frec1,frec2,Duration} Return 0 if Suscesfull  
{
	if(len!=5)
	{
		flush();
		return -3; //Bad Lengt of Data
	}
	if((params[0]<=maxPin && params[0]>1 )&&(params[1]<100)&&(params[2]<100))
	{
		int frec=(params[1]*1000)+(params[2]*10);
		int duration=params[3]*1000;
		tone(params[0],frec,duration);
		return 0;
	}
	if(params[1]>=100 || params[2]>=100)
	{
		return -2; // Invalid Range
	}
	return -1;	//Invalid Pin
}

void response(int val) // Print the confirmation message
{
	int cen=0;
	int dec=0;
	Serial.write(':');
	if(val>=0)
	{
		Serial.write('K');
	}
	else
	{
		Serial.write('E');
	}
	Serial.write(AT);
	if(AT==2)
	{
		
		while(val>=100)
		{
			val-=100;
			cen++;
		}
		dec=val;
		Serial.write(cen+11);
		Serial.write(dec+11);
	}
	else
	{
		Serial.write(val);
	}
	Serial.write(0x0A);
	Serial.flush();
	flush();
}

void flush()
{
	while(Serial.available()>0)
	{
		Serial.read();
	}
	Serial.flush();
}
