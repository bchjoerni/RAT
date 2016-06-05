
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

    Copyright 2015 bchjoerni
*/

////////////////////////////////////////////////////////////
// variables:
////////////////////////////////////////////////////////////

// pins leds
bool ledsOn = true;
int pinLED1 = 11;
int pinLED2 = 12;

// pins to sensors:
int pinPos   = 3;
int pinComp1 = 4;
int pinComp2 = 7;
int pinVent1 = 6;
int pinVent2 = 5;

// sensor states:
int pos = -1; // to read values first time
int comp1 = 0;
int comp2 = 0;
int vent1 = 0;
int vent2 = 0;

// calculated values
int comp = 0;
int vent = 0;

// bluetooth transmitting
boolean active = false;
boolean raw = false;

// time variables
unsigned long time = 0;
unsigned long timeout = 1000;

// message
String msg = "";

////////////////////////////////////////////////////////////
// helper functions::
////////////////////////////////////////////////////////////

void messageReceived( String cmd )
{
    if( cmd == "talkrats\n" )
    {
        active = true;
        timeout = 100;
        digitalWrite( pinLED1, HIGH );
        digitalWrite( pinLED2, HIGH );
        Serial.println( "aye(talkrats)" );
    }
    else if( cmd == "silence\n" )
    {
        active = false;
        digitalWrite( pinLED2, LOW );
        Serial.println( "aye(silence)" );
    }
    else if( cmd == "talk2me\n" )
    {
        active = true;
        if( ledsOn )
        {
            digitalWrite( pinLED2, HIGH );
        }
        Serial.println( "aye(talk2me)" );
    }
    else
    {
        if( cmd.substring( 0, 4 ) == "set:" )
        {
            if( cmd.substring( 4, 8 ) == "mto=" )
            {
                timeout = cmd.substring( 8 ).toInt();
                Serial.println( "aye(mto=" + String( timeout ) + ")" );
            }
            else if( cmd.substring( 4, 9 ) == "raw=1" )
            {
                raw = true;
                pos = -1; // to change the message string
                Serial.println( "aye(raw=1)" );
            }
            else if( cmd.substring( 4, 9 ) == "raw=0" )
            {
                raw = false;
                pos = -1; // to change the message string
                Serial.println( "aye(raw=0)" );
            }
            else if( cmd.substring( 4, 9 ) ==  "led=0" )
            {
                ledsOn = false;
                digitalWrite( pinLED1, LOW );
                digitalWrite( pinLED2, LOW );
                Serial.println( "aye(led=0)" );
            }
            else if( cmd.substring( 4, 9 ) == "led=1" )
            {
                ledsOn = true;
                digitalWrite( pinLED1, HIGH );
                digitalWrite( pinLED2, HIGH );
                Serial.println( "aye(led=1)" );
            }
            else
            {
                Serial.println( "err(unknown set)" );
            }
        }
        else
        {
            Serial.println( "err(unknown command)" );
        }
    }
}

bool pinsChanged()
{
    return ( digitalRead( pinPos )   != pos
         ||  digitalRead( pinComp1 ) != comp1
         ||  digitalRead( pinComp2 ) != comp2
         ||  digitalRead( pinVent1 ) != vent1
         ||  digitalRead( pinVent2 ) != vent2 );
}

void updateMsg()
{
    comp = 0;
    if( !(comp1 == 1 && comp2 == 1) )
    {
        comp = 2 + comp1 - comp2;
    }

    vent = 0;
    if( !(vent1 == 1 && vent2 == 1) )
    {
        vent = 2 + vent1 - vent2;
    }

    msg = "<" + String( 1-pos ) 
        + String( comp ) + String( vent ) + ">";

    if( raw )
    {
        msg = String( pos ) +
            String( comp1 ) + String( comp2 ) +
            String( vent1 ) + String( vent2 ) + " " + msg;
    }
}

////////////////////////////////////////////////////////////
// setup and loop:
////////////////////////////////////////////////////////////

void setup()
{
    pinMode( pinPos,   INPUT );
    pinMode( pinComp1, INPUT );
    pinMode( pinComp2, INPUT );
    pinMode( pinVent1, INPUT );
    pinMode( pinVent2, INPUT );

    pinMode( pinLED1, OUTPUT );
    pinMode( pinLED2, OUTPUT );
    
    digitalWrite( pinLED1, HIGH );
    digitalWrite( pinLED2, LOW );
    
    Serial.begin( 9600 );    
    time = millis();
}

void loop()
{    
    if( Serial.available() > 0 )
    {
        String message = Serial.readString();
        messageReceived( message );
    }
    
    if( active )
    {
        if( pinsChanged() )
        {
            pos   = digitalRead( pinPos );
            comp1 = digitalRead( pinComp1 );
            comp2 = digitalRead( pinComp2 );
            vent1 = digitalRead( pinVent1 );
            vent2 = digitalRead( pinVent2 );
            
            updateMsg();
                
            Serial.println( msg );           
            time = millis();
        }
        else
        {
            if( millis() - time > timeout )
            {
                Serial.println( msg );
                time = millis();
            }
        }
    }
}
