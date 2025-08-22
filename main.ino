#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  

const int buzz = 7;
const int irSensor = 12;
const int warnLed = 4;

Servo sv;
int svPin = 9;

unsigned long closedAt = 0;
bool prev_closed = false;
bool vibrating = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(buzz, OUTPUT);
  pinMode(irSensor, INPUT);
  pinMode(warnLed, OUTPUT);

  sv.attach(svPin);

  Serial.println("System Ready - Driver Awake");
  lcd.setCursor(0, 0);
  lcd.print("Driver Awake");
}

void loop() {
  int irValue = digitalRead(irSensor);

  if (irValue == LOW) {  // Eyes closed
    if (!prev_closed) {
      closedAt = millis();
      prev_closed = true;
    } else {
      unsigned long blinkTime = (millis() - closedAt) / 1000;

      Serial.print("Eyes closed for ");
      Serial.print(blinkTime);
      Serial.println(" sec");

      lcd.setCursor(0, 0);
      lcd.print("Closed: ");
      lcd.print(blinkTime);
      lcd.print("s   ");
      lcd.setCursor(0, 1);
      lcd.print("Status: Watching");

      if (blinkTime >= 3) {
        if (!vibrating) {
          Serial.println("ALERT: Driver Drowsy!");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Drowsy Detected");
          lcd.setCursor(0, 1);
          lcd.print("Vibrating Seat");

          digitalWrite(buzz, HIGH);
          digitalWrite(warnLed, HIGH);
          vibrating = true;
        }
        vibrateNow();  // make the seat vibrate
      }
    }
  } else {
    if (prev_closed) {
      prev_closed = false;

      Serial.println("Driver Awake");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Driver Awake");
      lcd.setCursor(0, 1);
      lcd.print("Restart Manual");

      digitalWrite(buzz, LOW);
      digitalWrite(warnLed, LOW);
      vibrating = false;
    }
  }

  delay(100);
}

void vibrateNow() {
  sv.write(0);
  delay(150);
  sv.write(60);
  delay(150);
  sv.write(120);
  delay(150);
  sv.write(90);
}
