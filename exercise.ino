/*
* Name & Email: Valerie Wong
* Discussion Section: 021
* Assignment: Lab 7 Exercise 1
* Exercise Description: For this laboratory assignment, you will implement a mock-indoor plant growing system based on the following specification. 
*                       This assignment will introduce the DHT11 temperature and humidity sensor and the LCD screen. 

* I acknowledge all content contained herein, excluding template 
  or example code, is my own original work.

* Demo Link: https://youtu.be/42MZr6ZqBzI
*/

#include <LiquidCrystal.h>
#include <DHT.h>
#define DHTPIN A0     
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);

} task;

const unsigned short tasksNum = 6;
task tasks[tasksNum];

// Pins
int JS_X = A2;
int JS_Y = A1;
int JS_BTN = 12;
int humidifierPin = 8;
int dehumidifierPin = 9;
int coolerPin = 10;
int heaterPin = 11;
int j_x;
int j_y;
int j_btn;
// Periods
const unsigned long systemPeriod = 300;
const unsigned long setTempPeriod = 300;
const unsigned long setHumidityPeriod = 300;
const unsigned long dhtSensorPeriod = 500;
const unsigned long comparePeriod = 300;
const unsigned long generatePulsePeriodTemp = 5;
const unsigned long generatePulsePeriodHum = 20;
// Bools
bool isOFF;
bool isDefault;
// Variables
unsigned char temp;
unsigned char humidity;
unsigned char s_temp;
unsigned char s_humidity;
unsigned char sensor_temp = 0;                         // 1 - heater, 2 - cooler
unsigned char sensor_hum = 0;                          // 3 - humidifier, 4 - dehumidifier
float h;
float t;
unsigned char i_temp = 0;
unsigned char i_hum = 0;

void readJoystick() {
  j_x = analogRead(JS_X);
  j_y = analogRead(JS_Y);
  j_btn = digitalRead(JS_BTN);
}

void setTempOrHumidity() {
  if (j_y < 200) {
    isDefault = false;
  }
  else if (j_y > 700) {
    isDefault = true;
  }
}

void readDHTSensor() {
  h = dht.readHumidity();
  t = dht.readTemperature(true);
  temp = static_cast<int>(t);
  humidity = static_cast<int>(h);
}

enum System_States { SYSTEM_START, OFF, ON, WAIT };
int System_Tick(int state1) {
  switch(state1) {
    case SYSTEM_START:
      isOFF = true;
      state1 = OFF;
      break;
  
    case OFF:
      readJoystick();
      
      if (j_btn) {
        state1 = OFF;
      }
      else if (!j_btn) {
        state1 = WAIT; 
      }
      break;
  
    case WAIT:
      readJoystick();
      
      if (!j_btn) {
        state1 = WAIT;
      }
      else if (j_btn && isOFF) {
        isOFF = false;
        state1 = ON;
      }
      else if (j_btn && !isOFF) {
        isOFF = true;
        state1 = OFF;
      }
      break;
  
    case ON:
      readJoystick();
      
      if (j_btn) {
        state1 = ON;
      }
      else {
        state1 = WAIT;
      }
      break;
  }
  
  switch(state1) {
    case SYSTEM_START:
      break;

    case OFF:
      //Serial.println("OFF");
      lcd.noDisplay();
      break;

    case WAIT:
      //Serial.println("WAIT");
      break;

    case ON:
      //Serial.println("ON");
      lcd.display();
      break;
  }

  return state1;
}

enum Set_Temp_States { START2, OFF2, ON2, INCREASE_TEMP, DECREASE_TEMP, WAIT2 };
int Set_Temp(int state2) {
  switch(state2) {
    case START2:
      state2 = OFF2;
      s_temp = 75;
      break;

    case OFF2:
      if (isOFF) {
        state2 = OFF2;
      }
      else {
        state2 = ON2;
        isDefault = true;
      }
      break;

    case ON2:
      readJoystick();
      setTempOrHumidity();
      
      if (!isOFF && (j_x > 200 && j_x < 700)) {
        state2 = ON2; 
      }
      else if (!isOFF && (j_x > 700) && isDefault) {
        state2 = INCREASE_TEMP;
      }
      else if (!isOFF && (j_x < 200) && isDefault) {
        state2 = DECREASE_TEMP;
      }
      else if (!isOFF && (j_x > 700) && !isDefault) {
        state2 = ON2;
      }
      else if (!isOFF && (j_x < 200) && !isDefault) {
        state2 = ON2;
      }
      else if (isOFF) {
        state2 = OFF2;
      }
      
      break;

    case INCREASE_TEMP:
      if (!isOFF && (j_x > 700)) {
        state2 = INCREASE_TEMP;
      }
      else if (!isOFF && (j_x > 200 && j_x < 700)) {
        state2 = ON2;
      }
      
      break;

    case DECREASE_TEMP:
      if (!isOFF && (j_x < 200)) {
        state2 = DECREASE_TEMP;
      }
      else if (!isOFF && (j_x > 200 && j_x < 700)) {
        state2 = ON2;
      }
      
      break;
  }

  switch(state2) {
    case START2:
      break;

    case OFF2:
      break;

    case ON2:
      lcd.setCursor(8,0);
      lcd.print("STemp:"); lcd.print(s_temp);
      break;

    case INCREASE_TEMP:
      ++s_temp;
      lcd.setCursor(8,0);
      lcd.print("STemp:"); lcd.print(s_temp);
      break;

    case DECREASE_TEMP:
      --s_temp;
      lcd.setCursor(8,0);
      lcd.print("STemp:"); lcd.print(s_temp);
      break;
  }

  return state2;
}

enum Set_Humidity_States { START3, OFF3, ON3, INCREASE_HUMIDITY, DECREASE_HUMIDITY, WAIT3 };
int Set_Humidity(int state3) {
  switch(state3) {
    case START3:
      state3 = OFF3;
      s_humidity = 30;
      break;

    case OFF3:
      if (isOFF) {
        state3 = OFF3;
      }
      else {
        state3 = ON3;
      }
      break;
  
    case ON3:
      readJoystick();
      setTempOrHumidity();
      
      if (!isOFF && (j_x > 200 && j_x < 700)) {
        state3 = ON3; 
      }
      else if (!isOFF && !isDefault && (j_x > 700)) {
        state3 = INCREASE_HUMIDITY;
      }
      else if (!isOFF && !isDefault && (j_x < 200)) {
        state3 = DECREASE_HUMIDITY;
      }
      else if (!isOFF && isDefault && (j_x > 700)) {
        state3 = ON3;
      }
      else if (!isOFF && isDefault && (j_x < 200)) {
        state3 = ON3;
      }
      else if (isOFF) {
        state3 = OFF3;
      }
      
      break;
  
    case INCREASE_HUMIDITY:
      if (!isOFF && (j_x > 700)) {
        state3 = INCREASE_HUMIDITY;
      }
      else if (!isOFF && (j_x > 200 && j_x < 700)) {
        state3 = ON3;
      }
      break;
  
    case DECREASE_HUMIDITY:
      if (!isOFF && (j_x < 200)) {
        state3 = DECREASE_HUMIDITY;
      }
      else if (!isOFF && (j_x > 200 && j_x < 700)) {
        state3 = ON3;
      }
      break;
  }

  switch(state3) {
    case START3:
      break;

    case OFF3:
      break;

    case ON3:
      lcd.setCursor(7,1);
      lcd.print("SHum:"); lcd.print(s_humidity);
      break;
  
    case INCREASE_HUMIDITY:
      ++s_humidity;
      lcd.setCursor(7,1);
      lcd.print("SHum:"); lcd.print(s_humidity);
      break;
  
    case DECREASE_HUMIDITY:
      --s_humidity;
      lcd.setCursor(7,1);
      lcd.print("SHum:"); lcd.print(s_humidity);
      break;
  }
  
  return state3;
}

enum Sample_States { START4, OFF4, SAMPLE };
int Sample_DHT(int state4) {
  switch(state4) {
    case START4:
      state4 = OFF4;
      break;

    case OFF4:
      if (isOFF) {
        state4 = OFF4;
      }
      else {
        state4 = SAMPLE;
      }
      break;

    case SAMPLE:
      if (!isOFF) {
        state4 = SAMPLE;
      }
      else {
        state4 = OFF4;
      }
      break;
  }

  switch (state4) {
    case START4:
      break;

    case OFF:
      break;

    case SAMPLE:
      readDHTSensor();
      lcd.setCursor(0,0);
      lcd.print("Temp:"); lcd.print(temp);
      lcd.setCursor(0,1);
      lcd.print("Hum:"); lcd.print(humidity);

      if (s_temp == temp) {
        sensor_temp = 0;
      }
      else if (s_temp > temp) {
        sensor_temp = 1;
      }
      else if (s_temp < temp) {
        sensor_temp = 2;
      }

      if (s_humidity == humidity) {
        sensor_hum = 0;
      }
      else if (s_humidity > humidity) {
        sensor_hum = 3;
      }
      else if (s_humidity < humidity) {
        sensor_hum = 4;
      }
      break;
  }

  return state4;
}

enum Generate_PWM_Temp_States { START6, OFF6, PWM_TEMP, RED_HIGH, RED_LOW, WHITE_HIGH, WHITE_LOW };
int Generate_PWM_Temp(int state6) {
  switch(state6) {
    case START6:
      state6 = OFF6;
      i_temp = 0;
      break;

    case OFF6:
      if (isOFF) {
        state6 = OFF6;
      }
      else {
        state6 = PWM_TEMP;
      }
      break;

    case PWM_TEMP:
      if (!isOFF && sensor_temp == 0) {
        state6 = PWM_TEMP;
      }
      else if (!isOFF && sensor_temp == 1) {
        state6 = RED_HIGH;
      }
      else if (!isOFF && sensor_temp == 2) {
        state6 = WHITE_HIGH;
      }
      else if (isOFF) {
        state6 = isOFF;
      }
      break;

    case RED_HIGH:
      if (isOFF) {
        state6 = OFF6;
      }
      else if (sensor_temp == 1) {
        if (i_temp <= 19) {
          state6 = RED_HIGH;
        }
        else if (i_temp > 19) {
          i_temp = 0;
          state6 = RED_LOW;
        }
      }
      else if (sensor_temp == 0) {
        state6 = PWM_TEMP;
      }
      else if (sensor_temp == 2) {
        state6 = WHITE_HIGH;
      }
      break;

    case RED_LOW:
      if (isOFF) {
        state6 = OFF6;
      }
      else if (sensor_temp == 1) {
        if (i_temp <= 1) {
          state6 = RED_LOW;
        }
        else if (i_temp > 1) {
          i_temp = 0;
          state6 = RED_HIGH;
        }
      }
      else if (sensor_temp == 0) {
        state6 = PWM_TEMP;
      }
      else if (sensor_temp == 2) {
        state6 = WHITE_HIGH;
      }
      break;

    case WHITE_HIGH:
      if (isOFF) {
        state6 = OFF6;
      }
      else if (sensor_temp == 2) {
        if (i_temp <= 5) {
          state6 = WHITE_HIGH;
        }
        else if (i_temp > 5) {
          i_temp = 0;
          state6 = WHITE_LOW;
        }
      }
      else if (sensor_temp == 0) {
        state6 = PWM_TEMP;
      }
      else if (sensor_temp == 1) {
        state6 = RED_HIGH;
      }
      break;

    case WHITE_LOW:
      if (isOFF) {
        state6 = OFF6;
      }
      else if (sensor_temp == 2) {
        if (i_temp <= 15) {
          state6 = WHITE_LOW;
        }
        else if (i_temp > 15) {
          i_temp = 0;
          state6 = WHITE_HIGH;
        }
      }
      else if (sensor_temp == 0) {
        state6 = PWM_TEMP;
      }
      else if (sensor_temp == 1) {
        state6 = RED_HIGH;
      }
      break;
  }

  switch(state6) {
    case START6:
      break;

    case OFF6:
      digitalWrite(heaterPin, LOW);
      digitalWrite(coolerPin, LOW);
      break;

    case PWM_TEMP:
      i_temp = 0;
      digitalWrite(heaterPin, LOW);
      digitalWrite(coolerPin, LOW);
      break;

    case RED_HIGH:
      ++i_temp;
      digitalWrite(heaterPin, HIGH);
      digitalWrite(coolerPin, LOW);
      Serial.println("RED HIGH");
      Serial.print("i_temp: "); Serial.println(i_temp);
      break;

    case RED_LOW:
      ++i_temp;
      digitalWrite(heaterPin, LOW);
      digitalWrite(coolerPin, LOW);
      Serial.println("RED LOW");
      Serial.print("i_temp: "); Serial.println(i_temp);
      break;

    case WHITE_HIGH:
      ++i_temp;
      digitalWrite(heaterPin, LOW);
      digitalWrite(coolerPin, HIGH);
      Serial.println("WHITE HIGH");
      Serial.print("i_temp: "); Serial.println(i_temp);
      break;

    case WHITE_LOW:
      ++i_temp;
      digitalWrite(heaterPin, LOW);
      digitalWrite(coolerPin, LOW);
      Serial.println("WHITE LOW");
      Serial.print("i_temp: "); Serial.println(i_temp);
      break;
  }

  return state6;
}

enum Generate_PWM_Hum_States { START7, OFF7, PWM_HUM, BLUE_HIGH, BLUE_LOW, YELLOW_HIGH, YELLOW_LOW };
int Generate_PWM_Hum(int state7) {
  switch(state7) {
    case START7:
      state7 = OFF7;
      i_hum = 0;
      break;

    case OFF7:
      if (isOFF) {
        state7 = isOFF;
      }
      else {
        state7 = PWM_HUM;
      }
      break;

    case PWM_HUM:
      if (isOFF) {
        state7 = isOFF;
      }
      
      if (!isOFF && sensor_hum == 0) {
        state7 = PWM_TEMP;
      }
      else if (!isOFF && sensor_hum == 3) {
        state7 = BLUE_HIGH;
      }
      else if (!isOFF && sensor_hum == 4) {
        state7 = YELLOW_HIGH;
      }
      break;

    case BLUE_HIGH:
      if (isOFF) {
        state7 = isOFF;
      }
      else if (sensor_hum == 3) {
        if (i_hum <= 2) {
          state7 = BLUE_HIGH;
        }
        else if (i_hum > 2) {
          i_hum = 0;
          state7 = BLUE_LOW;
        }
      }
      else if (sensor_hum == 0) {
        state7 = PWM_HUM;
      }
      else if (sensor_hum == 4) {
        state7 = YELLOW_HIGH;
      }
      break;

    case BLUE_LOW:
      if (isOFF) {
        state7 = isOFF;
      }
      else if (sensor_hum == 3) {
        if (i_hum <= 8) {
          state7 = BLUE_LOW;
        }
        else if (i_hum > 8) {
          i_hum = 0;
          state7 = BLUE_HIGH;
        }
      }
      else if (sensor_hum == 0) {
        state7 = PWM_HUM;
      }
      else if (sensor_hum == 4) {
        state7 = YELLOW_HIGH;
      }
      break;

    case YELLOW_HIGH:
      if (isOFF) {
        state7 = isOFF;
      }
      else if (sensor_hum == 4) {
        if (i_hum <= 5) {
          state7 = YELLOW_HIGH;
        }
        else if (i_hum > 5) {
          i_hum = 0;
          state7 = YELLOW_LOW;
        }
      }
      else if (sensor_hum == 0) {
        state7 = PWM_HUM;
      }
      else if (sensor_hum == 3) {
        state7 = BLUE_HIGH;
      }
      break;

    case YELLOW_LOW:
      if (isOFF) {
        state7 = isOFF;
      }
      else if (sensor_hum == 4) {
        if (i_hum <= 15) {
          state7 = YELLOW_LOW;
        }
        else if (i_hum > 15) {
          i_hum = 0;
          state7 = YELLOW_HIGH;
        }
      }
      else if (sensor_hum == 0) {
        state7 = PWM_HUM;
      }
      else if (sensor_hum == 3) {
        state7 = BLUE_HIGH;
      }
      break;
  }

  switch(state7) {
    case START7:
      break;

    case OFF7:
      digitalWrite(humidifierPin, LOW);
      digitalWrite(dehumidifierPin, LOW);
      break;

    case PWM_HUM:
      i_hum = 0;
      digitalWrite(humidifierPin, LOW);
      digitalWrite(dehumidifierPin, LOW);
      break;

    case BLUE_HIGH:
      ++i_hum;
      digitalWrite(humidifierPin, HIGH);
      digitalWrite(dehumidifierPin, LOW);
      Serial.println("BLUE HIGH");
      Serial.print("i_hum: "); Serial.println(i_hum);
      break;

    case BLUE_LOW:
      ++i_hum;
      digitalWrite(humidifierPin, LOW);
      digitalWrite(dehumidifierPin, LOW);
      Serial.println("BLUE LOW");
      Serial.print("i_hum: "); Serial.println(i_hum);
      break;

    case YELLOW_HIGH:
      ++i_hum;
      digitalWrite(humidifierPin, LOW);
      digitalWrite(dehumidifierPin, HIGH);
      Serial.println("YELLOW HIGH");
      Serial.print("i_hum: "); Serial.println(i_hum);
      break;

    case YELLOW_LOW:
      ++i_hum;
      digitalWrite(humidifierPin, LOW);
      digitalWrite(dehumidifierPin, LOW);
      Serial.println("YELLOW LOW");
      Serial.print("i_hum: "); Serial.println(i_hum);
      break;
  }

  return state7;
}

void setup() {
  pinMode(heaterPin, OUTPUT);
  pinMode(coolerPin, OUTPUT);
  pinMode(dehumidifierPin, OUTPUT);
  pinMode(humidifierPin, OUTPUT);
  pinMode(JS_BTN, INPUT_PULLUP);

  unsigned char i = 0;
  tasks[i].state = SYSTEM_START;
  tasks[i].period = systemPeriod;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &System_Tick;
  i++;
  tasks[i].state = START2;
  tasks[i].period = setTempPeriod;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Set_Temp;
  ++i;
  tasks[i].state = START3;
  tasks[i].period = setHumidityPeriod;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Set_Humidity;
  ++i;
  tasks[i].state = START4;
  tasks[i].period = dhtSensorPeriod;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Sample_DHT;
  ++i;
  tasks[i].state = START6;
  tasks[i].period = generatePulsePeriodTemp;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Generate_PWM_Temp;
  ++i;
  tasks[i].state = START7;
  tasks[i].period = generatePulsePeriodHum;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Generate_PWM_Hum;

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);

  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
    if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis(); // Last time this task was ran
    }
  }
}
