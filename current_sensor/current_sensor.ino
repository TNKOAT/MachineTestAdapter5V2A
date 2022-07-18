/*
 Machine Test Adapter 5V 2A By Oatt
 วิธีการใช้งาน 1.ปรับค่าให้กระแส = 0 ก่อน (ซ้าย = ค่าเพิ่ม,  ขวา = ค่าลด)
            2.เสียบสาย Adapter เพื่อทดสอบได้เลย
            *ไฟแดง = NG 
            *ไฟเขียว = OK 
            **หมายเหตุ OK ที่กระแส 1.8 A ขึ้นไป แต่ไม่เกิน 2 A
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

float average_current;
int LED_NG = 2;
int LED_OK = 3;

float R = 2.5; //ค่า R Load
int sensitive = 185; // ค่าสำหรับ Current sensor 5A
int offset; // ค่าเริ่มต้น 2500 ปรับค่าตรงนี้เพื่อให้ค่ายังไม่มีโหลดเป็น 0.00

void setup() {
  lcd.begin();
  Serial.begin(9600);
  pinMode(LED_NG, OUTPUT);
  pinMode(LED_OK, OUTPUT);
}

void loop() {
  Read_Current();
}

void Read_Current() {
  int VR = analogRead(A1);
  //Serial.println(VR);
  offset = 2500 + ((VR * 100) / 1000); // ปรับค่า offset ด้วย R ปรับค่าได้
  double I = getCA();
  Serial.print(I); 
  Serial.print(" ");
  Serial.println(offset);

  if (I <= 0.02 && I >= -0.02) { 
    I = 0;
  } else if(I >= 2){ // กระแสที่แสดงจะไม่เกิน 2 เพราะ adapter 5V 2A
      I = 2;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current");
  lcd.setCursor(0, 1);
  lcd.print(I);
  lcd.print(" A");
  float Volt = I * R;
  lcd.setCursor(9, 0);
  lcd.print("Voltage");
  lcd.setCursor(10, 1);
  lcd.print(Volt);
  lcd.print(" V");

  if (I < 1.75) { // ถ้ากระแสน้อยกว่า 1.75 A ไฟแดง NG จะติด
    digitalWrite(LED_NG, HIGH);
    digitalWrite(LED_OK, LOW);
  } else if (I > 1.75) { // ถ้ากระแสมากกว่า 1.75 ไฟเขียว OK จะติด
    digitalWrite(LED_NG, LOW);
    digitalWrite(LED_OK, HIGH);
  }

  delay(1000);
}

// หาค่ากระแสเฉลี่ย
double getCA() {
  int count = 100;
  double sum = 0;
  for (int i = 0; i < count; i++) {
    sum += getC();
  }
  double val = sum / count;
  return val;
}
// อ่านค่ากระแส
double getC() {
  int a = analogRead(A0);
  //Serial.println(a);
  double v = (a / 1024.0) * 5000;
  double c = (v - offset) / sensitive;
  return c;
}
