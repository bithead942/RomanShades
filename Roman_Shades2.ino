/*
  Roman Shade Control
  by Bithead942

  Works with the Arduino MKR1000 board.
  
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
  Up       3
  Down     4

  Shade 2 (South)
  Up       7
  Down     8

  Other
  LED      6

*/
#include <SPI.h>
#include <WiFi101.h>

const int pShade1Up = 3;  //white
const int pShade1Down = 4;  //red
const int pShade2Up = 7;  //white
const int pShade2Down = 8;  //red
const int pLED = 6;

String inString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int BlindNo;
int BlindUD;
int i;
char InChar;

char ssid[] = "<name>";          //  your network SSID (name)
char pass[] = "<password>";   // your network password
int status = WL_IDLE_STATUS;
int oldstatus = WL_IDLE_STATUS;
IPAddress ip(192, 168, 1, ???);
IPAddress dns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(8888);

void setup() {
  pinMode(pShade1Up, OUTPUT);
  pinMode(pShade1Down, OUTPUT);
  pinMode(pShade2Up, OUTPUT);
  pinMode(pShade2Down, OUTPUT);
  pinMode(pLED, OUTPUT);

  // initialize serial:
  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}

  inString.reserve(3);
  inString = "";
}


void loop() {
  WiFiConnect();

  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    while (client.connected()) {
      if (client.available()) {
        InChar = client.read();
        inString += InChar;
        // if the incoming character is a newline, set a flag
        // so the main loop can do something about it:
        if (InChar == 13 or InChar == 10)
        {
          stringComplete = true;
        }
      }

      if (stringComplete)     //wait for 1st signal to start the blind
      {
        Serial.println("Command Received");
        BlindNo = int(inString.charAt(0)) - 48;  
        client.print(BlindNo);    //, DEC);
        BlindUD = int(inString.charAt(1)) - 48;
        client.println(BlindUD);  //, DEC);
        if ((BlindNo == 1 or BlindNo == 2) and (BlindUD == 0 or BlindUD == 1))
        {
          moveBlind(BlindNo, BlindUD);
          client.println("0");
        }
        else
        {
          Serial.println("Error");
          client.println("1");  //Error, invalid input
        }
        inString = "";
        stringComplete = false;
        client.flush();
        //client.stop();
      }
    }
  }
  delay(1000);
}

void WiFiConnect() {
  // attempt to connect to Wifi network:
  status = WiFi.status();
  oldstatus = status;
  //Serial.println(status);
  while ( status != WL_CONNECTED) {
    WiFi.disconnect();
    // initialize WiFi:
    WiFi.config(ip, dns, gateway, subnet);
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  if (oldstatus != status and status == WL_CONNECTED) {
    Serial.print("Connected to ");
    IPAddress ReadIP = WiFi.localIP();
    Serial.println(ReadIP);
    server.begin();
    Blink(3);
  }
}


void moveBlind(int BlindNo, int BlindUD)
{

  if (BlindNo == 1)  //North
  {
    if (BlindUD == 0)  //Need to go down
    {
      digitalWrite(pShade1Down, HIGH);
      delay(300);
      digitalWrite(pShade1Down, LOW);
      Serial.println("North shade down");
    }
    if (BlindUD == 1)  //Need to go up
    {
      digitalWrite(pShade1Up, HIGH);
      delay(300);
      digitalWrite(pShade1Up, LOW);
      Serial.println("North shade up");
    }
    Blink(2);
  }

  if (BlindNo == 2)  //South
  {
    if (BlindUD == 0)  //Need to go down
    {
      digitalWrite(pShade2Down, HIGH);
      delay(300);
      digitalWrite(pShade2Down, LOW);
      Serial.println("South shade down");
    }
    if (BlindUD == 1)  //Need to go up
    {
      digitalWrite(pShade2Up, HIGH);
      delay(300);
      digitalWrite(pShade2Up, LOW);
      Serial.println("South shade up");
    }
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
