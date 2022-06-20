#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>
#include <Wire.h>

#define lightBall 11
#define readSensor 3
#define wirteSensor 9

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

unsigned int numberOfCut = 0;

//cariables to hold distance and currentPosition
int keyFullNumber = 0; 
int presentPosition = 0;

//initialize an instance of class New keypad
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

LiquidCrystal_I2C lcd(0x27, 16, 2); //0x27 or 0x3F - lcd16x2

// Define a stepper and the pins it will use
//pin6 -> (-pul), pin5 -> (-dir)
AccelStepper stepper(1,6,5); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5



void setup() {
  //Serial.begin(9600);

pinMode(3, INPUT);    //sensor & led pin
pinMode(9, OUTPUT);
//pinMode(10, OUTPUT);
pinMode(11, OUTPUT);

  //  AccelStepper speed and acceleration setup
  stepper.setMaxSpeed(15);  // maximum speed of stepper
  stepper.setAcceleration(2);  //  accelleration 
  
  lcd.init();       //initiate lcd
  lcd.backlight();  // turn on back ligh on lcd
  lcd.clear();

  lcd.setCursor(0, 0); 
  lcd.print("vi tri: 0-500mm");

  delay(1000); 

  digitalWrite(lightBall, HIGH); 
}

void loop() {
  char key = myKeypad.getKey();

  if(key != NO_KEY) { //if keypad button pressed check which key it was
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
      
      case '*':
        deleteNumber();;
      break;
      
      case '#':
        calculateDistance();
      break;
      
/*      case 'A':            // number of repeating cut
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("so lan cat:");
        lcd.setCursor(0, 1);
        lcd.print(numberOfCut);

        bool repeatingCut = false;
        if (key == 'B') {
          numberOfCut++;
        }
        if (key == 'A' && 
      break;*/
    }
  }

}

void checkNumber(int x) {
  if (firstNumber == 99) {
    firstNumber = x;
    int displayValue = firstNumber;
    showOnLcd(displayValue);
    
  } else if (secondNumber == 99) {
    secondNumber = x;
    int displayValue = (firstNumber * 10) + secondNumber;
    showOnLcd(displayValue);
    
  } else {
    thirdNumber = x;
    int displayValue = (firstNumber * 100) + (secondNumber * 10) + thirdNumber;
    showOnLcd(displayValue);
  }
}

void deleteNumber() {
  if (thirdNumber != 99) {
    lcd.setCursor(2, 1);
    lcd.print(" ");

    thirdNumber = 99;
  }else if (secondNumber != 99) {
    lcd.setCursor(1, 1);
    lcd.print(" ");

    secondNumber = 99;
  } else if (firstNumber != 99) {
    lcd.setCursor(0, 1);
    lcd.print(" ");

    firstNumber = 99;
  }
}

void calculateDistance() {
  if (thirdNumber == 99 && secondNumber == 99 && firstNumber != 99) {
    keyFullNumber = firstNumber;
    moveStepper(keyFullNumber);
  }
  if (secondNumber != 99 && thirdNumber == 99) {
    keyFullNumber = (firstNumber * 10) + secondNumber;
    moveStepper(keyFullNumber);
  }
  if (thirdNumber != 99) {
    keyFullNumber = (firstNumber * 100) + (secondNumber * 10) + thirdNumber;
    moveStepper(keyFullNumber);
  }
  resetNumbers(); //reset number to get ready for new entry
}

void moveStepper(int z) {
  //Stepper
  int microStep = 2;                              // pull/revolution 
  float angleStep = 1.8;
  float stepPerRound = ((5 *microStep * 360)/angleStep); // 2000 pulse/round
  //Lead screw
  int pitch = 20;                                 // pitch of screw in mm  
  //float minimumCut = 0;
  //lenght = lenght - minimumCut;                 //reCalculate the moving lenght of lead screw
  int calculatedMove = ((z/pitch)*(stepPerRound)); 
  presentPosition = 400-z;
  
  if (z < 400) {      //run stepper if moment in possible range
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("vi tri:");
    lcd.setCursor(0, 1);
    lcd.print(z);
  
    stepper.runToNewPosition(-calculatedMove);

    if(presentPosition < z) {           //checking the remaining distance
      stepper.runToNewPosition(0);
    }
    
  } else if (z == 0) {     //giving signal if stepper are in beginning position
    digitalWrite(lightBall, HIGH);
    
    stepper.runToNewPosition(-calculatedMove);
    
  } else {                              //unavailable position
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("khong kha dung,");
    lcd.setCursor(0, 1);
    lcd.print("moi nhap lai");
  }
  
  
}
void resetNumbers(){
  firstNumber = 99;
  secondNumber = 99;
  thirdNumber = 99;

  keyFullNumber = 0;
  presentPosition = 0;
}
void showOnLcd(int displayValue) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("vi tri: 0-500mm");  
  lcd.setCursor(0, 1);
  lcd.print(displayValue);
    
}
