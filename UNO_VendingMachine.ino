#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <pitches.h>
#include <TM1637Display.h>
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h>

#define DIO 2 // TM1637
#define CLK 4 // TM1637
#define RST_PIN 9 // rfid
#define SS_PIN 10 // rfid
#define BUZZER_PIN 8
#define LED_PIN 7

const int sw0 = 14; //  A0
const int sw1 = 15; //  A1
const int sw2 = 16; //  A2
const int sw3 = 17; //  A3
const int sw_restock = 5; // pin 5
const byte rxPin = 0; // pin 0
const byte txPin = 1; // pin 1

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance
TM1637Display display = TM1637Display(CLK, DIO);
Adafruit_PWMServoDriver myServo = Adafruit_PWMServoDriver(0x40);
SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);

const int Contrast = 100;
int buttonPressed = -1;
bool isCardDetected = false;
bool cardEnabled = false;
bool timeout = false;
int count, index;
int pulselen = 150;
float totalIncome = 0.0;
float productIncome[4] = {0.0};

const char* snacks[] = {
  "Snack A", "Snack B", "Snack C", "Snack D",
};

int prices[] = {
  10, 20, 30, 40,
};

int product_amount[] = {
  4, 4, 4, 4,
};

//  Twinkle, Twinkle, Little Star
int melody[] = {
  NOTE_C5, NOTE_C5, NOTE_G5, NOTE_G5,
  NOTE_A5, NOTE_A5, NOTE_G5,
  NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5,
  NOTE_D5, NOTE_D5, NOTE_C5,
};

int durations[] = {
  8, 8, 8, 8,
  8, 8, 4,
  8, 8, 8, 8,
  8, 8, 2,
};

byte heart[] = {
  0x00,  
  0x00,  
  0x0A,  
  0x15,  
  0x11,  
  0x0A,  
  0x04,  
  0x00
};

void setup() {
  initialSetup();
}

void initialSetup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(sw_restock, INPUT);
  pinMode(LED_PIN, OUTPUT); // Set the LED pin as an output
  pinMode(BUZZER_PIN, OUTPUT); // Set the BUZZER pin as an output
  analogWrite(6,Contrast);  //  set contrast at pin6
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, heart);
  lcd.clear();
  lcd.print("   Welcome to   ");
  lcd.setCursor(0, 1);
  lcd.print("Vending Machine");
  lcd.write((byte)0);
	SPI.begin();			// Init SPI bus
	rfid.PCD_Init();		// Init MFRC522
  display.clear();
  display.setBrightness(7);
  isCardDetected = false; // Reset card detection status
  cardEnabled = false; // Disable the card
  timeout = false;
  myServo.begin();
  myServo.setPWMFreq(50);
  TCCR1A = 0x00;
  TCCR1B = (1<<CS12);
  TCNT1 = 3036;
  TIMSK1 = (1<<TOIE1);
  sei();
}

void loop() {
  int button = checkButtons();
  switch (button) {
    case sw0:
      displaySnackAndPrice(sw0);
      delay(500);
      break;
    case sw1:
      displaySnackAndPrice(sw1);
      delay(500);
      break;
    case sw2:
      displaySnackAndPrice(sw2);
      delay(500);
      break;
    case sw3:
      displaySnackAndPrice(sw3);
      delay(500);
      break;
  }
  display.showNumberDecEx(00, 0b11100000, true, 2, 0);
  display.showNumberDecEx(count, 0, true, 2, 2);

  if (digitalRead(sw_restock) == LOW) {
    restockProduct();
  }

  if(!timeout){
    readCard();
    if (isCardDetected && cardEnabled) {
      count = 0;
      display.showNumberDecEx(count, 0, true, 2, 2);
      processTransaction();
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   Time out !   ");
    lcd.setCursor(0, 1);
    lcd.print("Please try again");
    delay(1000);
    initialSetup();
  }
}

int checkButtons() {
  for (int i = 0; i < 4; i++) {
    if (digitalRead(sw0 + i) == LOW) {
      if (product_amount[i] > 0) {
        enableCard(); // Enable the card
        buttonPressed = sw0 + i;
        count = 15;
        return buttonPressed;
      } else {
        count = 0;
        display.showNumberDecEx(0000, 0b11100000, true, 4, 0);
        displayOutOfStock(i);
        delay(1000);
        initialSetup();
      }
    }
  }
  return -1; // No button pressed or all products are out of stock
}

void displaySnackAndPrice(int index) {
  index -= 14;
  if (product_amount[index] > 0){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(snacks[index]);
    lcd.print(" (");
    lcd.print(product_amount[index]);
    lcd.print(" left)");
    lcd.setCursor(0, 1);
    lcd.print("Price: ");
    lcd.print(prices[index]);
    lcd.print(" Baht");
  } else {
    displayOutOfStock(index);
  }
}

void displayOutOfStock(int index) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     ");
  lcd.print(snacks[index]);
  lcd.setCursor(0, 1);
  lcd.print("  OUT OF STOCK  ");
}

void readCard() {
  if (!cardEnabled) {
    return; // If card is disabled, exit the function
  }
  // Reset the loop if no new card present on the sensor/reader.
  if (!rfid.PICC_IsNewCardPresent()) {
    isCardDetected = false; // Reset the card detection status
    digitalWrite(LED_PIN, LOW); // Turn off the LED
    return;
  }

  // Verify if the NUID has been read
  if (!rfid.PICC_ReadCardSerial())
    return;

  isCardDetected = true; // Set the card detection status to true
  rfid.PICC_HaltA();  // Halt PICC
  rfid.PCD_StopCrypto1(); // Stop encryption on PCD
  delay(1000);
}

void enableCard() {
  cardEnabled = true;
}

void playSong()
{
  int size = sizeof(durations) / sizeof(int);

  for (int note = 0; note < size; note++) {
    int duration = 1000 / durations[note];
    tone(BUZZER_PIN, melody[note], duration);
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZER_PIN);
  }
}

void processTransaction() {
  digitalWrite(LED_PIN, HIGH); // Turn on the LED
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Processing...");
  playSong();
  digitalWrite(LED_PIN, LOW); // Turn off the LED
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     Enjoy     ");
  lcd.setCursor(0, 1);
  lcd.print(" your snack!!! ");
  delay(1000);
  checkServo(buttonPressed);
  updateProduct(buttonPressed);
  calculateTotalIncome(buttonPressed);
  delay(1000);
  initialSetup();
}

void checkServo(int buttonPressed) {
  switch (buttonPressed) {
    case sw0:
      rotateServo(0);
      delay(800);
      stopServo(0);
      break;
    case sw1:
      rotateServo(1);
      delay(950);
      stopServo(1);
      break;
    case sw2:
      rotateServo(2);
      delay(750);
      stopServo(2);
      break;
    case sw3:
      rotateServo(3);
      delay(800);
      stopServo(3);
      break;
  }
}

void rotateServo(uint8_t servoNum) {
  myServo.setPWM(servoNum, 0, pulselen);
}

void stopServo(uint8_t servoNum){
  myServo.setPWM(servoNum, 0, 0);
}

void updateProduct(int buttonPressed) {
  index = buttonPressed - 14;
  if (index >= 0 && index < 4 && product_amount[index] > 0) {
    product_amount[index] -= 1;
    if (product_amount[index] == 0){
      Serial.print(snacks[index]);
      Serial.println(" is out of stock!");
    } else {
      Serial.print(snacks[index]);
      Serial.print(" remaining: ");
      Serial.println(product_amount[index]);
    }
  }
}

void restockProduct(){
  for (int i = 0; i < 4; i++) {
    product_amount[i] = 4;
  }
  Serial.println("Restock complete!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Restocking...  ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Restock complete");
  delay(2000);
  initialSetup();
}

float calculateTotalIncome(int buttonPressed) {
  index = buttonPressed - 14;
  int price = prices[index];
  productIncome[index] += prices[index];
  totalIncome += price;

  Serial.print("A income: ");
  Serial.println(productIncome[0]);
  Serial.print("B income: ");
  Serial.println(productIncome[1]);
  Serial.print("C income: ");
  Serial.println(productIncome[2]);
  Serial.print("D income: ");
  Serial.println(productIncome[3]);

  Serial.print("Total income: ");
  Serial.println(totalIncome);
  return totalIncome;
}

ISR(TIMER1_OVF_vect) {
  if (count > 0) {
    count--;
    if (count == 0) {
      // Transaction timeout
      timeout = true;
    }
  }
}