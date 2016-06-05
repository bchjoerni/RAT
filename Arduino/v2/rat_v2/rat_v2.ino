
/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2016 bchjoerni
*/

////////////////////////////////////////////////////////////
// DEBUGGING - only set true for testing!
////////////////////////////////////////////////////////////

boolean DEBUG = false;

////////////////////////////////////////////////////////////
// variables:
////////////////////////////////////////////////////////////

const long baudRate = 38400;

// pins leds
bool useLEDs = true;
int pinLED1 = 10;
int pinLED2 = 11;

// pins to sensors:
int pinRead  = 8;
int pinPos   = 7;
int pinComp1 = 3;
int pinComp2 = 6;
int pinVent1 = 5;
int pinVent2 = 4;

// sensor states:
const int delayReadMicros = 300;
int pos = -1; // -1 to read values first time
int comp1 = 0;
int comp2 = 0;
int vent1 = 0;
int vent2 = 0;

// bluetooth transmitting
boolean running = false;
boolean printReadable = false;

// time variables
unsigned long msLastTransmit = 0;
unsigned long transmitIntervalMs = 100;
unsigned long msLastConSignal = 0;
unsigned long checkConIntervalMs = 2000;

////////////////////////////////////////////////////////////
// helper functions::
////////////////////////////////////////////////////////////

String getMillisConvStr()
{
    unsigned long time = millis();
    char t1 = (time/95/95/95)%95 + 32;
    char t2 = (time/95/95)%95 + 32;
    char t3 = (time/95)%95 + 32;
    char t4 = time%95 + 32;
    
    return String( t1 ) + String( t2 ) + String( t3 ) + String( t4 ); 
}

void checkSerialMessage()
{
    if(Serial.available() > 0)
    {
        String msg = Serial.readString();
        msg.trim();
        
        while(msg.indexOf("\n") != -1)
        {
            String firstMsg = msg.substring(0, msg.indexOf("\n"));
            firstMsg.trim();
            interpretMessage(firstMsg);
            msg = msg.substring(msg.indexOf("\n"));
            msg.trim();
        }
        interpretMessage(msg);
    }
}

void interpretMessage( String cmd )
{
    if( cmd == "startDef" )
    {
        printReadable = false;
        msLastConSignal = millis();
        running = true;

        transmitIntervalMs = 100;
        checkConIntervalMs = 2000;
        
        if( useLEDs )
        {
            digitalWrite( pinLED2, HIGH );
        }
        Serial.println( "r" + cmd + ":" + getMillisConvStr() );
    }
    else if( cmd == "startDbg" )
    {
        DEBUG = true;
        printReadable = true;        
        transmitIntervalMs = 500;
        checkConIntervalMs = 2000;
        
        msLastConSignal = millis();
        running = true;
        
        if( useLEDs )
        {
            digitalWrite( pinLED2, HIGH );
        }
        Serial.println( "r" + cmd + ":" + getMillisConvStr() );
    }
    else if( cmd == "start" )
    {
        running = true;
        msLastConSignal = millis();
        
        if( useLEDs )
        {
            digitalWrite( pinLED2, HIGH );
        }
        Serial.println( "r" + cmd + ":" + getMillisConvStr() );
    }
    else if( cmd == "stop" )
    {
        running = false;
        
        if( useLEDs )
        {
            digitalWrite( pinLED2, LOW );
        }
        Serial.println( "r" + cmd );
    }
    else if( cmd == "on" )
    {
        msLastConSignal = millis();
    }
    else if( cmd.substring( 0, 4 ) == "set:" )
    {
        if( cmd.substring( 4, 8 ) == "txi=" )
        {
            transmitIntervalMs = cmd.substring( 8 ).toInt();
            Serial.println( "rtxi=" + String( transmitIntervalMs ) );
        }
        else if( cmd.substring( 4, 8 ) == "cci=" )
        {
            checkConIntervalMs = cmd.substring( 8 ).toInt();
            Serial.println( "rcci=" + String( checkConIntervalMs ) );
        }
        else if( cmd.substring( 4, 9 ) == "prr=1" )
        {
            printReadable = true;
            pos = -1; // to change the message string
            Serial.println( "rprr=" + String( printReadable ) );
        }
        else if( cmd.substring( 4, 9 ) == "prr=0" )
        {
            printReadable = false;
            pos = -1; // to change the message string
            Serial.println( "rprr=" + String( printReadable ) );
        }
        else if( cmd.substring( 4, 9 ) ==  "led=0" )
        {
            useLEDs = false;
            digitalWrite( pinLED1, LOW );
            digitalWrite( pinLED2, LOW );
            Serial.println( "rset:led=" + String(useLEDs) );
        }
        else if( cmd.substring( 4, 9 ) == "led=1" )
        {
            useLEDs = true;
            digitalWrite( pinLED1, HIGH );
            digitalWrite( pinLED2, HIGH );
            Serial.println( "rset:led=" + String(useLEDs) );
        }
        else
        {
            Serial.println( "u" + cmd );
        }
    }
    else
    {
        Serial.println( "u" + cmd );
    }
}

boolean pinsChanged()
{
    int posNew   = digitalRead( pinPos );
    int comp1New = readRASensor( pinComp1 );
    int comp2New = readRASensor( pinComp2 );
    int vent1New = readRASensor( pinVent1 );
    int vent2New = readRASensor( pinVent2 );
    
    if( posNew   != pos
        ||  comp1New != comp1
        ||  comp2New != comp2
        ||  vent1New != vent1
        ||  vent2New != vent2 )
    {
        pos   = posNew;
        comp1 = comp1New;
        comp2 = comp2New;
        vent1 = vent1New;
        vent2 = vent2New;
        
        return true;
    }
    return false;
}

int readRASensor( int pin )
{
    digitalWrite( pin, LOW );
    delayMicroseconds( delayReadMicros );
    int sensorState = digitalRead( pinRead );
    digitalWrite( pin, HIGH );
    return sensorState;
}

void sendStatus()
{
    char posVal  = 0;
    char compVal = 0;
    char ventVal = 0;
    
    if( pos == 0 )
    {
        posVal = 1;
    }
    
    if( comp1 == 1 && comp2 == 1 )
    {
        compVal = 0;
    }
    else if( comp1 == 0 && comp2 == 1 )
    {
        compVal = 25;
    }
    else if( comp1 == 0 && comp2 == 0 )
    {
        compVal = 45;
    }
    else if( comp1 == 1 && comp2 == 0 )
    {
        compVal = 55;
    }
    
    if( vent1 == 1 && vent2 == 1 )
    {
        ventVal = 0;
    }
    else if( vent1 == 0 && vent2 == 1 )
    {
        ventVal = 30;
    }
    else if( vent1 == 0 && vent2 == 0 )
    {
        ventVal = 60;
    }
    else if( vent1 == 1 && vent2 == 0 )
    {
        ventVal = 90;
    }

    if( printReadable )
    {
        Serial.println( 
            "<dbg:"
            " c1: " + String( comp1 ) +
            " c2: " + String( comp2 ) +
            " v1: " + String( vent1 ) +
            " v2: " + String( vent2 ) +
            " val:"
            " c:" + String( int( compVal ) ) +            
            " v:" + String( int( ventVal ) ) +
            " p:" + String( int( posVal ) ) +
            " t:"  + String( millis() ) + 
            ">");
    }
    else
    {
        compVal += 32;
        ventVal += 32;
        posVal += 32;      
      
        unsigned long time = millis();
        char t1 = (time/95/95/95)%95 + 32;
        char t2 = (time/95/95)%95 + 32;
        char t3 = (time/95)%95 + 32;
        char t4 = time%95 + 32;
                
        Serial.write( 'd' );
        Serial.write( compVal );
        Serial.write( ventVal );
        Serial.write( posVal );
        Serial.write( t1 );
        Serial.write( t2 );
        Serial.write( t3 );
        Serial.write( t4 );
        Serial.write( "\r\n" );
    }
}

////////////////////////////////////////////////////////////
// setup and loop:
////////////////////////////////////////////////////////////

void setup()
{
    pinMode( pinRead,  INPUT  );
    
    pinMode( pinPos,   OUTPUT );
    pinMode( pinComp1, OUTPUT );
    pinMode( pinComp2, OUTPUT );
    pinMode( pinVent1, OUTPUT );
    pinMode( pinVent2, OUTPUT );

    pinMode( pinLED1, OUTPUT );
    pinMode( pinLED2, OUTPUT );
    
    digitalWrite( pinLED1, HIGH );
    digitalWrite( pinLED2, LOW );
    
    Serial.begin( baudRate );
    
    if( DEBUG )
    {
        printReadable = true;        
        transmitIntervalMs = 500;
        checkConIntervalMs = 2000;
        
        msLastConSignal = millis();
        running = true;
    }
}

void loop()
{    
    checkSerialMessage();
    
    if( running )
    {
        if( pinsChanged() )
        {
            sendStatus();         
            msLastTransmit = millis();
        }
        else
        {
            if( millis() - msLastTransmit > transmitIntervalMs )
            {
                sendStatus();
                msLastTransmit = millis();
            }
        }
        /* does not work properly!!!
        if( millis() - msLastConSignal > checkConIntervalMs )
        {
            if( !DEBUG )
            {
                running = false;
            }
        }
        */
    }
    else
    {
        digitalWrite( pinLED2, LOW );
    }
}
