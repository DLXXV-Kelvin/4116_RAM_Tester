#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
/*
 * above is only needed if you want to use an LCD
*/
#define DI          7     //  Arduino Nano D7 > 4116 pin 2
#define DO          15    //  Arduino Nano A1 > 4116 pin 14
#define CAS         14    //  Arduino Nano A0 > 4116 pin 15
#define RAS         5     //  Arduino Nano D5 > 4116 pin 4
#define WE          6     //  Arduino Nano D6 > 4116 pin 3

#define Add_0         4     //  Arduino Nano D4 > 4116 pin 5
#define Add_1         2     //  Arduino Nano D2 > 4116 pin 7
#define Add_2         3     //  Arduino Nano D3 > 4116 pin 6
#define Add_3         17    //  Arduino Nano A3 > 4116 pin 12
#define Add_4         9     //  Arduino Nano D10 > 4116 pin 9
#define Add_5         8     //  Arduino Nano D11 > 4116 pin 8 
#define Add_6        16    //  Arduino Nano A2 > 4116 pin 13

#define RED_LED      10    // Anode to pin 10 (D10)
#define GR_LED       11    // Anode to pin 11 (D11)


#define BUS_SIZE     7

//follow is only needed if using LCD. From this point, anything with LCD is only 
//needed if using an LCD
LiquidCrystal_PCF8574 lcd(0x27);

volatile int bus_size;

const unsigned int a_bus[BUS_SIZE] = {
  Add_0, Add_1, Add_2, Add_3, Add_4, Add_5, Add_6
};


void setup() {
  int i;
  
  Serial.begin(9600);
//Wire only needed for LCD
  Wire.begin();
  Wire.beginTransmission(0x27);
 
  lcd.begin(16, 2); // initialize the lcd
  lcd.setBacklight(200);
  lcd.setCursor(0, 0);
  lcd.print("-4116 RAM Test-");
  lcd.setCursor(0, 1);
  Serial.println();
  Serial.print("c= [4116 RAM Tester] c= \n \n");
  bus_size = BUS_SIZE;
 for (i = 0; i < BUS_SIZE; i++)
    pinMode(a_bus[i], OUTPUT);

  pinMode(CAS, OUTPUT);
  pinMode(RAS, OUTPUT);
  pinMode(WE, OUTPUT);
  pinMode(DI, OUTPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(GR_LED, OUTPUT);

  pinMode(DO, INPUT);

  digitalWrite(WE, HIGH);
  digitalWrite(RAS, HIGH);
  digitalWrite(CAS, HIGH);

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GR_LED, HIGH);

  Serial.flush();

  digitalWrite(RED_LED, LOW);
  digitalWrite(GR_LED, LOW);

  noInterrupts();
  
  for (i = 0; i < (1 << BUS_SIZE); i++) {
    digitalWrite(RAS, LOW);
    digitalWrite(RAS, HIGH);
  }
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GR_LED, HIGH);

}



void setBus(unsigned int a) {
  int i;
  for (i = 0; i < BUS_SIZE; i++) {
    digitalWrite(a_bus[i], a & 1);
    a /= 2;
  }
}

void writeAddress(unsigned int r, unsigned int c, int v) {
  /* address row */
  setBus(r);
  digitalWrite(RAS, LOW);

  /* rw */
  digitalWrite(WE, LOW);

  /* value */
  digitalWrite(DI, (v & 1)? HIGH : LOW);

  /* address column */
  setBus(c);
  digitalWrite(CAS, LOW);
  digitalWrite(WE, HIGH);
  digitalWrite(CAS, HIGH);
  digitalWrite(RAS, HIGH);
}

int readAddress(unsigned int r, unsigned int c) {
  int ret = 0;

  /* address row */
  setBus(r);
  digitalWrite(RAS, LOW);

  /* address column */
  setBus(c);
  digitalWrite(CAS, LOW);

  /* get current value */
  ret = digitalRead(DO);

  digitalWrite(CAS, HIGH);
  digitalWrite(RAS, HIGH);

  return ret;
}

void error(int r, int c)
{
  unsigned long a = ((unsigned long)c << bus_size) + r;
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GR_LED, LOW);
  interrupts();
  Serial.print(" Test Failed at address: $");
  Serial.println(a, HEX);
  Serial.flush();
  lcd.setCursor(0,0);
  lcd.print("                 ");
  lcd.setCursor(0,0);
  lcd.print("Test Failed!!");
  lcd.setCursor(0,1);
  lcd.print("                 ");
  lcd.setCursor(0,1);
  lcd.print("at address: $" + String(a,HEX));
  turn_off_leds();
  while (1);
}

void ok(void)
{
  digitalWrite(RED_LED, LOW);
  digitalWrite(GR_LED, HIGH);
  interrupts();
  lcd.setCursor(0,0);
  lcd.print("                 ");
  lcd.setCursor(2,0);
  lcd.print("RAM is good!");
  lcd.setCursor(0,1);
  lcd.print("                 ");
  lcd.setCursor(0,1);
  lcd.print("-wait a second-");
  Serial.println("\n this 4116 RAM module is good ");
  Serial.flush();
  turn_off_leds();
  
  while (1);
}

void wait_for_reset(void)
{
  while(1);
}

void turn_off_leds(void)
{
  delay(6000);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GR_LED, LOW);
  delay(500);
  blink();
  
    //digitalWrite(RED_LED, LOW);
    //digitalWrite(GR_LED, LOW);

while(1);
}

void blink(void)
{
   digitalWrite(RED_LED, LOW);
   digitalWrite(GR_LED, LOW);
   delay(200);
   digitalWrite(RED_LED, HIGH);
   digitalWrite(GR_LED, HIGH);
   delay(200);
   digitalWrite(RED_LED, LOW);
   digitalWrite(GR_LED, LOW);
   delay(200);
   digitalWrite(RED_LED, HIGH);
   digitalWrite(GR_LED, HIGH);
   delay(200);
   digitalWrite(RED_LED, LOW);
   digitalWrite(GR_LED, LOW);
   delay(200);
   digitalWrite(RED_LED, HIGH);
   digitalWrite(GR_LED, HIGH);
   delay(200);
   Serial.print("\n ready for another 4116 module");
   lcd.setCursor(0,0);
  lcd.print("                 ");
  lcd.setCursor(0,0);
  lcd.print("-4116 RAM Test-");
  lcd.setCursor(0,1);
  lcd.print("                 ");
  lcd.setCursor(0,1);
  lcd.print("ready 4 another");
}

void green(int v) {
  digitalWrite(RED_LED, v);
}

void fill(int v) {
  int r, c, g = 0;
  v &= 1;
  for (c = 0; c < (1 << bus_size); c++) {
    green(g? HIGH : LOW);
    for (r = 0; r < (1 << bus_size); r++) {
      writeAddress(r, c, v);
      if (v != readAddress(r, c))
        error(r, c);
    }
    g ^= 1;
  }
}

void fillx(int v) {
  int r, c, g = 0;
  v &= 1;
  for (c = 0; c < (1 << bus_size); c++) {
    green(g? HIGH : LOW);
    for (r = 0; r < (1 << bus_size); r++) {
      writeAddress(r, c, v);
      if (v != readAddress(r, c))
        error(r, c);
      v ^= 1;
    }
    g ^= 1;
  }
}


void loop()
{
 interrupts(); lcd.setCursor(0,1); lcd.print("                "); lcd.setCursor(0, 1);lcd.print("checking block 1"); Serial.print("writing/reading...\n"); Serial.flush(); noInterrupts(); fillx(0);
  interrupts(); lcd.setCursor(0,1); lcd.print("                ");lcd.setCursor(0, 1);lcd.print("checking block 2"); Serial.print("writing/reading...\n"); Serial.flush(); noInterrupts(); fillx(1);
  interrupts(); lcd.setCursor(0, 1); lcd.print("                ");lcd.setCursor(0, 1);lcd.print("checking block 3"); Serial.print("writing/reading...\n"); Serial.flush(); noInterrupts(); fill(0);
  interrupts(); lcd.setCursor(0, 1); lcd.print("                ");lcd.setCursor(0, 1);lcd.print("checking block 4"); Serial.print("writing/reading...\n"); Serial.flush(); noInterrupts(); fill(1);
  ok();
}
