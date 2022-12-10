# CS120B-LAB7

## Wiring Diagram
![image](https://user-images.githubusercontent.com/74090811/206876326-f74eb41e-bbd0-43cc-b411-3b667ce57a04.png)

## Exercise:
There system has 4 outputs:
* A humidifier (blue LED pin 8)
  * uses a PWM signal with a duty cycle of 20% and a period of 200 ms
* A de-humidifier (yellow LED pin 9)
  * uses a PWM signal with a duty cycle of 25% and a period of 400 ms
* A cooler (white LED pin 10)
  * uses a PWM signal with a duty cycle of 60% and a period of 300 ms
* A heater (red LED pin 11).
  * uses a PWM signal with a duty cycle of 95% and a period of 100 ms

The system will also display four values on the LCD screen:
* The current humidity
* The current temperature
* The set humidity
* The set temperature.

The user controls the system using the joystick:
* To turn the system on press the joystick 
* To turn the system off press the joystick again
* By default the joystick controls the set temperature 
  * Moving the joystick right increases the set temperature
  * Moving the joystick left decreases the set temperature
* Moving the joystick down sets the joystick to control the set humidity
  * Moving the joystick right increases the set humidity
  * Moving the joystick left decreases the set humidity
  * Moving the joystick up sets the joystick to control the set temperature again
* The system samples the input from the joystick every 300ms
* The system samples the temperature and humidity from the DHT11 every 500ms

The system operation is as follows:
* When the system is on, the default settings for the set temperature and set humidity are 75 and 30
* The LCD displays the set temperature and set humidity and the current temperature and current humidity
* If the set temperature and the set humidity are the same as the current temperature and current humidity then the system does nothing 
* If the set temperature is above the current temperature, the heater turns on until the set temperature reaches the current temperature
* If the set temperature is below the current temperature, the cooler turns on until the set temperature reaches the current temperature
* If the set humidity is above the current humidity, the humidifier turns on until the set humidity reaches the current humidity
* If the set humidity is below the current humidity, the de-humidifier turns on until the set humidity reaches the current humidity

Demo Link: https://youtu.be/42MZr6ZqBzI
