/////////////////////////////////////////////////////////////////////////////////////
//Intruction:
//  A: defaul mode
//  B: moving backward 
//  C: Auto mode (input: moving distance - number of repeating time)
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////

#include <AccelStepper.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

const byte ROWS = 4;// number of rows on the keypad
const byte COLS = 4;// number of columns on the keypad

byte rowPins[ROWS] = {8, 7, 4, 2}; //Arduino pins connected to row pins of the keypad
byte colPins[COLS] = {A3, A2, A1, A0}; // Arduino pins connected to column pins of the keypad
char keys [ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//Cariables to hold entered number on Keypad
volatile int firstNumber = 99;
volatile int secondNumber = 99;
volatile int thirdNumber = 99;

//cariables to hold distance and currentPosition
int keyFullNumber = 0;
int presentPosition = 0;

//stepper mode
bool movingDirection = true;
bool AutoStepper = false;
bool AutoMark = false;

//automode number
unsigned int AutoNum1 = 0;
unsigned int AutoNum2 = 0;

//system maximum travel distance
int maxDistance = 450;

//initialize an instance of class New keypad
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

LiquidCrystal_I2C lcd(0x27, 16, 2); //0x27 or 0x3F - lcd16x2

// Define a stepper and the pins it will use
//pin6 -> (-pul), pin5 -> (-dir)
AccelStepper stepper(1, 6, 5); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

void setup() {
  //Serial.begin(9600);

  //  AccelStepper speed and acceleration setup
  stepper.setMaxSpeed(200);  // Not to fast or you will have missed steps
  stepper.setAcceleration(2);  //  Same here

  lcd.init();       //initiate lcd
  lcd.backlight();  // turn on back ligh on lcd
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("input(0-" + String(maxDistance) + "mm):");
  lcd.setCursor(9, 1);
  lcd.print("<<-");
  lcd.setCursor(0, 1);
  
  delay(1000);
}

void loop() {
  char key = myKeypad.getKey();

  if (key != NO_KEY) { //if keypad button pressed check which key it was
    switch (key) {

      case '0':
        checkNumber(0);
        break;

      case '1':
        checkNumber(1);
        break;

      case '2':
        checkNumber(2);
        break;

      case '3':
        checkNumber(3);
        break;

      case '4':
        checkNumber(4);
        break;

      case '5':
        checkNumber(5);
        break;

      case '6':
        checkNumber(6);
        break;

      case '7':
        checkNumber(7);
        break;

      case '8':
        checkNumber(8);
        break;

      case '9':
        checkNumber(9);
        break;

      case 'A':      
        resetNumbers();
        movingDirection = true;
        AutoStepper = false;
        
        lcdShow("input(0-" + String(maxDistance) + "mm):", 0);
        break;

      case 'B': //moving backward      
        resetNumbers();
        movingDirection = false;
        AutoStepper = false;
        
        lcdShow("move BACKWARD:", 0);
        break;

      case 'C':         
        resetNumbers();
        AutoStepper = true;

        if (movingDirection == true) {  
          lcdShow("input(0-" + String(maxDistance) + "mm):", 0);
        }else {
          lcdShow("move BACKWARD:", 0);
        }
        break;

      case 'D':
        break;

      case '*':
        deleteNumber();;
        break;

      case '#':
      
        if ((finalNumber() > maxDistance) && (finalNumber() != 99)) {               //enter invalid value of distance
          lcd.setCursor(0, 1);
          lcd.print("invalid!");
          resetNumbers();
          
        } else if (AutoStepper == false) {                                           //non-Auto
            if ((finalNumber() < maxDistance) && (movingDirection == true)) {       //moving forward
              stepper.runToNewPosition((DistanceToStep((finalNumber()))));
             } 
            else if ((finalNumber() < maxDistance) && (movingDirection == false)) { //moving backward
              stepper.runToNewPosition(-(DistanceToStep((finalNumber()))));
             }
           
        } else if (AutoStepper == true) {                                            //Auto mode: num1 - distance travel || num2 - how many time repeating the travel distance
            if (AutoMark == false) {                                                 //flag to record num1
              AutoNum1 = finalNumber();
              resetNumbers();
              
              lcd.setCursor(String(AutoNum1).length(), 1);
              lcd.print("Redo?");
              lcd.blink();              
              
              AutoMark = true;                                            //trigger Auto flag
            } else if (AutoMark == true) {                                           //flag to record num2
              AutoNum2 = finalNumber();
              resetNumbers();
              
              if (movingDirection == true) {  
                lcdShow("input(0-" + String(maxDistance) + "mm):", 0);
              }else {
                lcdShow("move BACKWARD:", 0);
              }
              lcd.noBlink();
              
              AutoMark = false;
              
              if ((AutoStepper == true) && (AutoNum2 > 0)) {
                AutoStepperMode(AutoNum1, AutoNum2);
              }              
            }
        }
        break;
      }
    }
}
void AutoStepperMode(int AutoMovingDistance, unsigned int repeatingCount){
  int remainingLength = maxDistance;
  int AutoAdvanceDistance = 0;

  lcd.setCursor(0,1);
  lcd.print(String(AutoNum1) + "|");
  
  while (repeatingCount > 0) {
    
    lcd.setCursor(String(AutoNum1).length() + 1, 1);
    //lcd.print(repeatingCount - 1);
    lcd.print(repeatingCount - 1);
    
    AutoAdvanceDistance += AutoMovingDistance;    //increasing position
    if (remainingLength < AutoMovingDistance) { 
      stepper.runToNewPosition(0);
      
    }else if ((movingDirection == true)) {
      stepper.runToNewPosition((DistanceToStep(AutoAdvanceDistance)));
      
    }else if ((movingDirection == false)) {
      stepper.runToNewPosition(-(DistanceToStep(AutoAdvanceDistance)));        
    }
    delay(500);
    remainingLength -= AutoMovingDistance; 
    repeatingCount -= 1;    
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcdShow("Auto Ended", 0);
  
  stepper.runToNewPosition(0);
  
  AutoNum1 = 0;
  AutoNum2 = 0;

  if (movingDirection == true) {  
    lcdShow("input(0-" + String(maxDistance) + "mm):", 0);
  }else {
    lcdShow("move BACKWARD:", 0);
  }
}

void checkNumber(int x) {
  if (firstNumber == 99) {
    firstNumber = x;
    int displayValue = firstNumber;
    if (movingDirection == true) {  
      lcdShow("input(0-" + String(maxDistance) + "mm):", displayValue);
    }else {
      lcdShow("move BACKWARD:", displayValue);
    }

  } else if (secondNumber == 99) {
    secondNumber = x;
    int displayValue = (firstNumber * 10) + secondNumber;
    if (movingDirection == true) {  
      lcdShow("input(0-" + String(maxDistance) + "mm):", displayValue);
    }else {
      lcdShow("move BACKWARD:", displayValue);
    }

  } else {
    thirdNumber = x;
    int displayValue = (firstNumber * 100) + (secondNumber * 10) + thirdNumber;
    if (movingDirection == true) {  
      lcdShow("input(0-" + String(maxDistance) + "mm):", displayValue);
    }else {
      lcdShow("move BACKWARD:", displayValue);
    }
  }
}

void deleteNumber() {
  if (thirdNumber != 99) {
    lcd.setCursor(2, 1);
    lcd.print(" ");

    thirdNumber = 99;
  } else if (secondNumber != 99) {
    lcd.setCursor(1, 1);
    lcd.print(" ");

    secondNumber = 99;
  } else if (firstNumber != 99) {
    lcd.setCursor(0, 1);
    lcd.print(" ");

    firstNumber = 99;
  }
}

int finalNumber() {
  if (thirdNumber == 99 && secondNumber == 99 && firstNumber != 99) {
    keyFullNumber = firstNumber;
    return keyFullNumber;
  }
  if (secondNumber != 99 && thirdNumber == 99) {
    keyFullNumber = (firstNumber * 10) + secondNumber;
    return keyFullNumber;
  }
  if (thirdNumber != 99) {
    keyFullNumber = (firstNumber * 100) + (secondNumber * 10) + thirdNumber;
    return keyFullNumber;
  }
  if (thirdNumber == 99) {
    keyFullNumber = 0;
    return keyFullNumber;
  }
  resetNumbers(); //reset number to get ready for new entry
}

int DistanceToStep(int z) {
  //Stepper
  int microStep = 2;                              // pull/revolution
  float angleStep = 1.8;
  float stepPerRound = 2 * microStep * 360 / angleStep; // 200 pulse/round X geared box ratio: 1:2
  //Lead screw
  int pitch = 20;                                 // pitch of screw in mm
  int calculatedMove = ((z / pitch) * (stepPerRound));
  presentPosition = z;

  //lcdShow("currently in:", z);

  return calculatedMove;
  //stepper.runToNewPosition(calculatedMove);
}
void resetNumbers() {
  firstNumber = 99;
  secondNumber = 99;
  thirdNumber = 99;

  keyFullNumber = 0;
  //presentPosition = 0;
}
void lcdShow(String PrintedOnLcd, int displayValue) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(PrintedOnLcd);
  lcd.setCursor(0, 1);
  lcd.print(displayValue);  
  lcd.setCursor(9, 1);
  if (movingDirection == true) {  
    lcd.print("<<-");
  }else {
    lcd.print("->>");
  }
  lcd.setCursor(12, 1);
  if (AutoStepper == true) {
    lcd.print("Auto");
  }
}
