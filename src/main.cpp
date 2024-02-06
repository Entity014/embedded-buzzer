#include <Arduino.h>
#include <LCD_I2C.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CDevice.h>

#define BUZZER 18
#define BUTTON 0

hw_timer_t *buzzer_timer = NULL;
hw_timer_t *state_timer = NULL;

// C C# D Eb E F F# G G# A Bb B
// 0 1  2 3  4 5 6  7 8  9 10 11
int key_note[12] = {1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976};
int note[] = {3, 10, 9, 9, 0, 2, 3, -1, 3, -1, 3, -1, 2, 3, 10, 2, 10,
              3, 3, 7, 5, 7, 10, 2, 3, 3, 2, 3, 10, 10, 9, 10,
              10, 3, 3, 7, 5, 7, 9, 10, 3, 3, 2, 3, 5, 7, 3, 3,
              9, 9, 9, 10, 7, 7, 3, 7, 9, 9, 9, 10, 7, 7, 3, 7,
              5, 5, 5, 5, 5, 10, 2, 10, 2, 3, 3, 3, 3, 7, 5, 7, 9, 10, 3, 3, 2, 3, 5, 7, 3, 3,
              9, 9, 9, 10, 7, 7, 3, 7, 9, 9, 9, 10, 7, 7, 3, 7,
              5, 5, 5, 5, 5, 10, 2, 10, 2, 3, 3, 3, 3};
int state = 0;
int delay_note_time = 250000;
int note_time = 1e6;
int speed = 0;

LCD_I2C lcd(0x27, 16, 2);

void IRAM_ATTR onBuzzerTimer()
{
  digitalWrite(BUZZER, !digitalRead(BUZZER));
}

void IRAM_ATTR onStateTimer()
{

  // digitalWrite(BUZZER, LOW);
  if (speed == 0)
  {
    delay_note_time = 250000;
  }
  else if (speed == 1)
  {
    delay_note_time = 250000 * 0.5;
  }
  else if (speed == 2)
  {
    delay_note_time = 250000 * 0.25;
  }
  else if (speed == 3)
  {
    delay_note_time = 250000 * 2.0;
  }
  else if (speed == 4)
  {
    delay_note_time = 250000 * 1.5;
  }
  note_time = (note[state] == -1) ? 10 : int((1.0 / (key_note[note[state]] * 1.0)) * 1e6);
  timerAlarmWrite(buzzer_timer, note_time, true);
  timerAlarmWrite(state_timer, delay_note_time, true);
  state++;
}

void IRAM_ATTR IO_INT_ISR()
{
  if (!digitalRead(BUTTON) == 1)
  {
    speed = (speed > 3) ? 0 : speed + 1;
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(BUTTON, IO_INT_ISR, FALLING);
  buzzer_timer = timerBegin(0, 80, true);                   // use tiemr 0 and set prescale to 80 so 1 tick is 1 uSec
  timerAttachInterrupt(buzzer_timer, &onBuzzerTimer, true); // point to the ISR
  timerAlarmWrite(buzzer_timer, note_time, true);           // set alarm every 1 sec
  timerAlarmEnable(buzzer_timer);                           // enable the alarm

  state_timer = timerBegin(1, 80, true);
  timerAttachInterrupt(state_timer, &onStateTimer, true);
  // timerAlarmWrite(state_timer, 225000, true);
  timerAlarmWrite(state_timer, 250000, true);
  timerAlarmEnable(state_timer);
  lcd.begin();
  lcd.backlight();
  lcd.cursor();
  lcd.blink();
}

void loop()
{
  if (speed == 0)
  {
    lcd.print("Normal");
  }
  else if (speed == 1)
  {
    lcd.print("Fast");
  }
  else if (speed == 2)
  {
    lcd.print("Fast Mak");
  }
  else if (speed == 3)
  {
    lcd.print("Slow Mak");
  }
  else if (speed == 4)
  {
    lcd.print("Slow");
  }
  delay(500);
  lcd.clear();
}