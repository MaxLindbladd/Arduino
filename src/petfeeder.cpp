
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


Servo myservo;  // create servo object to control a servo
Servo motor;
#define SHORT_PRESS_DURATION 999 // Lyhyen painalluksen aikaraja millisekunteina
#define LONG_PRESS_DURATION 1000 // Pitkän painalluksen aikaraja millisekunteina

unsigned long buttonPressTime = 0;
int potpin = 0;  // analog pin used to connect the potentiometer
int valservo;    // variable to read the value from the analog pin
int valmotor;
int interruptmenuPin = 2; //button
int interruptrunningPin = 3;
int buttonPin = 7;
int redLed = 12;
int greenLed = 11;
int blueLed = 10;
int ammount = 5000;
int feedingTime1 = 9;
int feedingTime2 = 21;

int runningState = false;
int menuState = false;
int standby = true;
int buttonShortPress = false;
int buttonLongPress = false; 
int feededState = false;


unsigned long previousMillis = 0;  
const long interval = 60000;       
int hours = 0;                     
int minutes = 0; 




void changeRunningState() {
  runningState = true;
  menuState = false;
 }

void changeMenuState() {
  menuState = true;
  runningState = false;
}

void setup() {
  motor.attach(5);
  myservo.attach(6);
  Serial.begin(9600);
  pinMode(interruptmenuPin, INPUT_PULLUP);
  pinMode(interruptrunningPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptmenuPin), changeMenuState, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptrunningPin), changeRunningState, RISING);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  setTime(0, 0);

}

void runFeeding(){
  feededState = true;
  valservo = 170;
  valmotor = 1580;
  myservo.write(valservo);  
  delay(1000);
  motor.writeMicroseconds(valmotor);
  delay(3000);
  valservo = 30;
  myservo.write(valservo);  
  delay(2000);
  valmotor = 0;
  motor.write(valmotor); 
}

void handleLed(int red, int green, int blue){
  analogWrite(redLed, red);
  analogWrite(greenLed, green);
  analogWrite(blueLed, blue);
}



void setTime(int _hours, int _minutes) {
  hours = _hours;
  minutes = _minutes;
}

void handleButton(){
    if(digitalRead(buttonPin) == LOW) {
    delay(50); 
    
    if(digitalRead(buttonPin) == LOW) { // Napin tila on edelleen LOW, joten se on todennäköisesti painettuna
      buttonPressTime = millis(); // Tallennetaan nykyinen aika
      while(digitalRead(buttonPin) == LOW) {} // Odota, kunnes nappi vapautetaan
    }
    
    // Tarkistetaan kuinka pitkään nappia painettiin
    unsigned long pressDuration = millis() - buttonPressTime;

    // Lyhyt painallus
    if (pressDuration < SHORT_PRESS_DURATION) {
      // Tee jotain lyhyen painalluksen tapahtuessa
      buttonShortPress = !buttonShortPress; // Vaihda napin tilaa
    }
    
    // Pitkä painallus
    else if (pressDuration >= LONG_PRESS_DURATION) {
      buttonLongPress = true;
    }
  }
}


void options(){
  while(menuState == true){
      handleButton();
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Options:");
      
      if(buttonShortPress == false) {
        display.println("-> Set Time");
        display.println("   Set Feeding Time");
        if(buttonLongPress == true){
          buttonLongPress = false;
          setClock();
        }
      } else if(buttonShortPress == true) {
        display.println("   Set Time");
        display.println("-> Set Feeding Time");
        if(buttonLongPress == true){
          buttonLongPress = false;
          setFeedingTimes();
        }
      }
      display.print("time: ");
      display.print(hours);
      display.print(":");
      display.println(minutes);
      display.print("feeding times: ");
      display.print(feedingTime1);
      display.print(", ");
      display.println(feedingTime2);

      display.display();
      buttonLongPress = false;
    }
}

void setFeedingTimes(){
  int counter = 1;
  while(true){
      handleButton();
      if(buttonShortPress == true){
      counter ++;
      buttonShortPress = false;
      if(counter == 4){
      counter = 1;}
      }
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("set feeding times");
      display.print("feeding time 1: ");
      if(counter == 1){
      display.print(feedingTime1);
      display.println(" <-");
      if(buttonLongPress == true){
          buttonLongPress = false;
          setFeedingTime1();
        }
      }else{
      display.println(feedingTime1);
      }
      display.print("feeding time 2: ");
      if(counter == 2){
      display.print(feedingTime2);
      display.println("<-");
      if(buttonLongPress == true){
          buttonLongPress = false;
          setFeedingTime2();
        }
      }else{display.println(feedingTime2);}
      if(counter == 3){
      display.println("");
      display.println("");
      display.println("");
      display.println("<-");
      if(buttonLongPress == true){
          buttonLongPress = false;
          break;
          options();
    
          delay(15);
        }
      }
      display.display();
  }
}

void setFeedingTime1(){
  while(true){
      handleButton();
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("set feeding times");
      display.print("feeding time 1: ");
      display.println(feedingTime1);
      display.print("feeding time 2: ");
      display.println(feedingTime2);
      if(buttonShortPress == true) {
        feedingTime1 ++;
        buttonShortPress = false;
        if(feedingTime1 == 24){
        feedingTime1 = 0;
        }
      }
      if(buttonLongPress == true){
      buttonLongPress = false;
      break;
      setFeedingTimes();
   
      delay(15);
      } 
      display.display();
    }
}

void setFeedingTime2(){
  while(true){
      handleButton();
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("set feeding times");
      display.print("feeding time 1: ");
      display.println(feedingTime1);
      display.print("feeding time 2: ");
      display.println(feedingTime2);
      if(buttonShortPress == true) {
        feedingTime2 ++;
        buttonShortPress = false;
        if(feedingTime2 == 24){
        feedingTime2 = 0;
        }
      }
      if(buttonLongPress == true){
      buttonLongPress = false;
      break;
      setFeedingTimes();
    
      delay(15);
      } 
      display.display();
    }
}

void setClock(){
  int counter = 1;
    while(true){
      handleButton();
      if(buttonShortPress == true){
      counter ++;
      buttonShortPress = false;
      if(counter == 4){
      counter = 1;}
      }
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("set time: ");
      display.print(hours);
      display.print(":");
      display.println(minutes);
      if(counter == 1){
      display.println("          ^ ");
      display.println("          | ");
      if(buttonLongPress == true){
          buttonLongPress = false;
          setHours();
        }
      } else if(counter ==2) {
      display.println("             ^ ");
      display.println("             | ");
      if(buttonLongPress == true){
          buttonLongPress = false;
          setMinutes();
        }
      }
      else if(counter == 3){
      display.println("");
      display.println("");
      display.println("");
      display.println("");
      display.println("<-");
      if(buttonLongPress == true){
          buttonLongPress = false;
          break;
          options();
         
          delay(15);
        }
      }
      display.display();
      } 
}

void setHours(){
  while(true){
    handleButton();
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("set time: ");
      display.print(hours);
      display.print(":");
      display.println(minutes);
      if(buttonShortPress == true) {
        hours ++;
        buttonShortPress = false;
        if(hours == 24){
        hours = 0;
        }
      } 
      display.display();
      if(buttonLongPress == true){
      buttonLongPress = false;
      break;
      setClock();
      
      delay(15);
      }
  }
}

void setMinutes(){
while(true){
    handleButton();
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("set time: ");
      display.print(hours);
      display.print(":");
      display.println(minutes);
      if(buttonShortPress == true) {
        minutes ++;
        buttonShortPress = false;
        if(minutes == 60){
        minutes = 0;
        }
      } 
      display.display();
      if(buttonLongPress == true){
      buttonLongPress = false;
      break;
      setClock();
      
      delay(15);
      }
  }
}


void loop() {
Serial.println(runningState);
Serial.print("väli");
Serial.println(menuState);
unsigned long currentMillis = millis();  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    minutes++;
    if (minutes == 60) {
      minutes = 0;
      hours++;
      feededState = false;
      if (hours == 24) {
        hours = 0;  // Palaa nollaan kun tunti on 24
      }
    }
  }


if(digitalRead(buttonPin) == LOW){
  handleButton();
}

if(runningState == true){
  handleLed(0, 255, 0);
  if(hours == 9 && minutes == 1 && feededState == false || hours == 21 && minutes == 1 && feededState == false){
    runFeeding();
  }
  
}

if(menuState == true){
  handleLed(0, 0, 255);
  options();
}

if(standby == true){
  handleLed(255, 0, 0);
}

}

