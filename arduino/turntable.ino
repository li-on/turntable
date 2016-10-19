#include <LiquidCrystal.h>

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

//DFR0009
#define lcdRS 6
#define lcdEN 7
#define lcdD4 2
#define lcdD5 3
#define lcdD6 4
#define lcdD7 5

#define lcdKeyPin 4

//step motor constants
int motorPin1=14;    // Blue   - 28BYJ48 pin 1
int motorPin2=15;    // Pink   - 28BYJ48 pin 2
int motorPin3=16;   // Yellow - 28BYJ48 pin 3
int motorPin4=17;  // Orange - 28BYJ48 pin 4
#define motorMax 3000
#define motorConf 2000
//#define motorSpeed 1
#define motorSpeed 1200
#define motorInert 2

// select the pins used on the LCD panel
LiquidCrystal lcd(lcdRS, lcdEN, lcdD4, lcdD5, lcdD6, lcdD7);

// define some values used by the panel and buttons
int lcd_key      = 0;
int last_lcd_key = 0;
int adc_key_in   = 0;
int last_read = 0;

//turntable position
int stepPos = 0;
int stepTarget = 0;
int pos = 1;

int menuLevel = 0;
int menuFunct = 0;

int lastDir=0;
int action=0;

//ULN2003 + 28BYJ-48
int lookup[9] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001, B00000};

void setup()
{
 pinMode(motorPin1, OUTPUT);
 pinMode(motorPin2, OUTPUT);
 pinMode(motorPin3, OUTPUT);
 pinMode(motorPin4, OUTPUT);
 Serial.begin(9600);

 lcd.begin(16, 2);              // start the library
 lcd.setCursor(0,0);

 lcd.setCursor(0,1); 
 lcd.print("auto            ");

}
 
void loop()
{

  action=0;
 lcd_key = read_LCD_buttons();  // read the buttons
 if(last_lcd_key==lcd_key ){//&& (last_read+5000) < millis()
    lcd_key = btnNONE;
 }else{
    last_lcd_key = lcd_key;
    last_read = millis();
 }
 
 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
    Serial.print("R");
    Serial.print(motorPin1);
	 if(menuLevel==0){
		menuFunct++;
		if(menuFunct>21){
			menuFunct=0;
		}
	 }else{
	    if(menuFunct==0){
		    //auto
	    }else{
              stepTarget+=10;
             
			//manual
		}
	 }
     break;
   case btnLEFT:
     if(menuLevel==0){
		menuFunct--;
		if(menuFunct<0){
			menuFunct=21;
		}
	 }else{
	    if(menuFunct==0){
		    //auto
	    }else{
		stepTarget-=10;
		}
	 }
     break;
   case btnUP:
		if(menuLevel==1 && menuFunct==1){
			//manual fast
                      stepTarget+=500; 
		}
     break;
   case btnDOWN:
     //lcd.print("DOWN  ");
	 if(menuLevel==1){
		if(menuFunct==1){
			//manual fast
                      stepTarget-=500;  

		}
		if(menuFunct>1){
			//save position
		}
	 }
     break;
   case btnSELECT:
	 if(menuLevel==0){
		menuLevel=1;
	 }else{
		menuLevel=0;
	 }
     break;
   case btnNONE:
     //lcd.print("NONE  ");
     break;
 } 


if(stepTarget>stepPos){
   action=1;
   //myStepper.step(10);
   if(lastDir==-1){
     for(int i=0;i<motorInert;i++){
       clockwise();       
     }
   }
   clockwise();
   lastDir=1;
   stepPos+=1;   
   if(stepPos>motorMax){
     stepPos=0;
     stepTarget-=motorMax;
   }
   if(stepTarget==stepPos){
     off();
   }
 }
 
if(stepTarget<stepPos){
   action=1;
   if(lastDir==1){
     for(int i=0;i<motorInert;i++){
       anticlockwise();       
     }     
   }
   anticlockwise();
   lastDir=-1;
   stepPos-=1;   
   if(stepPos<0){
     stepPos=motorMax;
     stepTarget+=motorMax;
   }
   if(stepTarget==stepPos){
     off();
   }
}
 
  //lcd 16x2
// lcd.clear();
 if(lcd_key!=btnNONE || action){
   lcd.setCursor(0,0);
   lcd.print(stepPos);
   lcd.setCursor(8,0);
   lcd.print(pos);
   
 }
 
 if(lcd_key!=btnNONE){
   lcd.setCursor(0,1);
    if(menuLevel==1){
     if(menuFunct==0){
    	lcd.print("AUTO            ");
     }
     if(menuFunct==1){
    	lcd.print("MANUAL          ");
     }	
     if(menuFunct>1){
    	lcd.print("SET ");
    	lcd.print(menuFunct-1);
    	lcd.print(" V=SAVE");
     }
    }else{ 
     if(menuFunct==0){
    	lcd.print("auto            ");
     }
     if(menuFunct==1){
    	lcd.print("manual          ");
     }	
     if(menuFunct>1){
    	lcd.print("set ");
    	lcd.print(menuFunct-1);
     }
    }
 }

} 
 
// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(lcdKeyPin);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 /*if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  */

 // For V1.0 comment the other threshold and use the one below:

 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   

 return btnNONE;  // when all others fail, return this...
}
 
void anticlockwise()
{
  setOutput(0);
  delayMicroseconds(motorSpeed);
  setOutput(1);
  delayMicroseconds(motorSpeed);
  setOutput(2);
  delayMicroseconds(motorSpeed);
  setOutput(3);
  delayMicroseconds(motorSpeed);
  setOutput(4);
  delayMicroseconds(motorSpeed);
  setOutput(5);
  delayMicroseconds(motorSpeed);
  setOutput(6);
  delayMicroseconds(motorSpeed);
  setOutput(7);
  delayMicroseconds(motorSpeed);
}

void clockwise()
{
  setOutput(7);
  delayMicroseconds(motorSpeed);
  setOutput(6);
  delayMicroseconds(motorSpeed);
  setOutput(5);
  delayMicroseconds(motorSpeed);
  setOutput(4);
  delayMicroseconds(motorSpeed);
  setOutput(3);
  delayMicroseconds(motorSpeed);
  setOutput(2);
  delayMicroseconds(motorSpeed);
  setOutput(1);
  delayMicroseconds(motorSpeed);
  setOutput(0);
  delayMicroseconds(motorSpeed);
}

void off()
{
  setOutput(8);  
}
void setOutput(int out)
{
  digitalWrite(motorPin1, bitRead(lookup[out], 0));
  digitalWrite(motorPin2, bitRead(lookup[out], 1));
  digitalWrite(motorPin3, bitRead(lookup[out], 2));
  digitalWrite(motorPin4, bitRead(lookup[out], 3));
}
