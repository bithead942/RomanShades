/*
Roman Shade Control
by Bithead942

 Works with the Arduino FIO board with Xbee S6B Wifi module
 
 Remote control of the Roman Shades hanging on the French Doors on the East side of the house.
 
 This module interfaces with the Hunter Douglas Connection Interface for the PowerRise 2.0 with Platinum Technology system.
 http://www.automatedshadeinc.com/files/controls-hunterdouglas/connection%20interface_5110540109_0510.pdf
 
 There are 2 Roman shades, one on each door.  They can be controlled independently.
 The north-most shade is Shade 1
 The south-most shade is Shade 2
 
 To control, a command in the following format must be sent:
 
 Request: 2 integer values
 First value:  The number of the shade to control (either 1 or 2)
 Second value:  Up/down (1 or 0)
 Termination value:  Chr(10) or Chr(13)
 
 Response: 1 integer value
 Echo back request (followed by a Chr(13))
 Prints a 0 when complete.  Prints a non-zero when an error occurs
 Error 1:  Invalid input
 
 The control application will need to calibrate the timing to stop the shade mid-way.
 
 PINS USED:
 Xbee
 Tx       0
 Rx       1
 
 Shade 1 (North)
 Up       5
 Down     6
 
 Shade 2 (South)
 Up       10
 Down     11
 
 Other
 Sleep    3
 LED      13 
 
 */

const int pShade1Up = 5;    
const int pShade1Down = 6;  
const int pShade2Up = 10;
const int pShade2Down = 11;
const int pLED = 13;
const int pSleep = 3;

String inString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int BlindNo;
int BlindUD;
int i;
  
void setup() {
  pinMode(pShade1Up, OUTPUT);
  pinMode(pShade1Down, OUTPUT);
  pinMode(pShade2Up, OUTPUT);
  pinMode(pShade2Down, OUTPUT); 
  pinMode(pSleep, OUTPUT);
  
    // switch off ADC
  ADCSRA = ADCSRA & 0x7F;  // clear ADEN
 
  // switch off analog comparator
  ACSR = ACSR & 0x7F;
  
  // stay awake
  digitalWrite(pSleep, LOW);
  
  // initialize serial:
  Serial.begin(9600);
  inString.reserve(3);
  inString = "";

  Blink(3);
}

void loop() {
  if (stringComplete)     //wait for 1st signal to start the blind
  {    
    BlindNo = int(inString.charAt(0)) - 48;
    Serial.print(BlindNo, DEC);
    BlindUD = int(inString.charAt(1)) - 48;
    Serial.println(BlindUD, DEC);
    if ((BlindNo == 1 or BlindNo == 2) and (BlindUD == 0 or BlindUD == 1))
       {
       moveBlind();
       }
    else
       {
       Serial.println("1");  //Error, invalid input
       }

    // clear the string:
    inString = "";
    stringComplete = false;
    
    delayedRead();  //wait for 2nd signal to stop blinds
    if (stringComplete)
    {
       BlindNo = int(inString.charAt(0)) - 48;
       Serial.print(BlindNo, DEC);
       BlindUD = int(inString.charAt(1)) - 48;
       Serial.println(BlindUD, DEC);
       if ((BlindNo == 1 or BlindNo == 2) and (BlindUD == 0 or BlindUD == 1))
          {
          moveBlind();
          }
       else
          {
          Serial.println("1");  //Error, invalid input
          }

       // clear the string:
       inString = "";
      stringComplete = false;
    }
    Serial.setTimeout(1000);
  }
  digitalWrite(pSleep, HIGH);    //Sleep
  delay(5000);
  digitalWrite(pSleep, LOW);     //Wake
  delay(200);  
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {

  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    //Serial.write(inChar);
    // add it to the inputString:
    inString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == 13 or inChar == 10) 
    {
      stringComplete = true;
    } 
  }
}

void delayedRead() {

  Serial.setTimeout(10000);
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    //Serial.write(inChar);
    // add it to the inputString:
    inString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == 13 or inChar == 10) 
    {
      stringComplete = true;
    } 
  }
}


void moveBlind()
{

  if (BlindNo == 1)  //North
  {
    if (BlindUD == 0)  //Need to go down
    {
      digitalWrite(pShade1Down, HIGH);
      delay(300);
      digitalWrite(pShade1Down, LOW);
    }
    if (BlindUD == 1)  //Need to go up
    {
      digitalWrite(pShade1Up, HIGH);
      delay(300);
      digitalWrite(pShade1Up, LOW);
    }
    Serial.println("0");
    Blink(2);
  }
  
  if (BlindNo == 2)  //South
  {
    if (BlindUD == 0)  //Need to go down
    {
      digitalWrite(pShade2Down, HIGH);
      delay(300);
      digitalWrite(pShade2Down, LOW);
    }
    if (BlindUD == 1)  //Need to go up
    {
      digitalWrite(pShade2Up, HIGH);
      delay(300);
      digitalWrite(pShade2Up, LOW);
    }
    Serial.println("0");
    Blink(2);
  }
}

void Blink(int iTimes)
{
  for (int i = 1; i <= iTimes; i++)
    {
       digitalWrite(pLED, HIGH);
       delay(100);
       digitalWrite(pLED, LOW);
       delay(100);
    }
}


