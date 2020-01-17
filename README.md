# IoT-enabled-coffee-maker
IoT enabled coffee maker is made by programing TM4C using energia

# IMPLEMENTATION:
Coffee makers basically uses NTC temperature sensor, heating coil, water pump, 12 V dc motor, 2x 5V DC motors, motor driver IC L293D, WIFI module( ESP 8266).
These components are integrated as explained in the finite state machine.

WIFI module connects the controller to an app which sends signal to make coffee.

2 5V DC motors control the movement of coffee and sugar containers.

Where as heating coil warms up the water and temperature monitors the temperature and pumps water to coffee cup when desired temperature is attained.

A conveyer belt arrangement moves the cup the position of each step.

HERE ARE FUNCTION NAMES MENTIONED FOR PERFORMING ALL THESE STEPS:

Void servo(int c, int a)      2. void DC_MOTOR(int dctime)
Void water_pump()          4. float Thermistor(int Vo)
5.    Void esp_enable()
