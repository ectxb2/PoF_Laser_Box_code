#include <Adafruit_MAX31865.h>
#include <TaskScheduler.h>
#include <Adafruit_INA260.h>
#include <Adafruit_DS3502.h>


//adafruit sensor setups
Adafruit_MAX31865 laser_thermo = Adafruit_MAX31865(10, 11, 12, 13);
Adafruit_MAX31865 opc_thermo = Adafruit_MAX31865(9, 11, 12, 13);
Adafruit_INA260 power = Adafruit_INA260();
Adafruit_INA260 laser_power = Adafruit_INA260();
Adafruit_DS3502 potentiometer = Adafruit_DS3502();
//these values are used for calculating temperature
#define RREF      430.0
#define RNOMINAL  100.0

//laser control pins, current read
const int LASER_CONTROL = 8;

//current read out and value of sensor for 
//calculating the current from voltage
const int CURRENT_READOUT = A7;
const int POT_VOLTAGE = A6;
double laserCurrent;
double laserVoltage;
double potVoltage;
double laser_vcc;
double time = 0;


/****************************************************************************** 
###############################################################################
Here is where we make adjusments to the code to operate our 
laser. 

The print_timer variable determines how frequently our data is saved.
Currently it is set to every second

The pulse variable determines whether the laser is being pulsed. 
0 means no pulsing, and 1 means the laser will be pulsed

The pulse_time variable determines the cycle time in which the laser 
is pulsed. Currently it is set to every 50 milliseconds, which means
the laser will be on for 25 milliseconds and off for 25 milliseconds

The control_voltage variable determines the desired control voltage 
for the laser driver. Csurrently it is set to 0.5 volts. The appropriate 
range is 0 to 5volts, and correlates to 0 to 3 amps of current to the laser

Since the digital potentiometer has 128 steps of resistance, getting the exact 
voltage desired isn't possible, instead the closest possible voltage will be selected.
5V / 128 steps will results in a step resolution of  0.039 volts approximately

###############################################################################
 *****************************************************************************/

//counters and timers for sending data in seconds
double print_timer = 1;

//pulse or no pulse? 0 is no pulsing, 1 is pulsing laser
int pulse = 0;

//the pulse cycle time in milliseconds
int pulse_time = 50;

//desired control voltage for laser driver, range is 0 to 5V
double control_voltage = 0.5;

/**************************************************************************** 
#############################################################################
 ****************************************************************************/

double timer = print_timer;
int print_counter = 0;


//function to get the sensor data and print to serial comms
void print_data(){
    laserVoltage = analogRead(CURRENT_READOUT);
    laserVoltage = laserVoltage * (5.0/1023);
    laserCurrent = laserVoltage / 0.15;
    potVoltage = analogRead(POT_VOLTAGE);
    potVoltage = potVoltage * (5.0/1023);

    //Serial.println("Laser Temp - C, OPC Temp - C, Laser Current in - A, Laser Current out - A, OPC Voltage - V, OPC Current - A");
    Serial.print(millis()/1000.0, 3), Serial.print(",");
    Serial.print(laser_thermo.temperature(RNOMINAL, RREF)), Serial.print(",");
    Serial.print(opc_thermo.temperature(RNOMINAL, RREF)), Serial.print(",");
    Serial.print(potVoltage), Serial.print(",");
    Serial.print(laserCurrent), Serial.print(",");
    Serial.print(laser_power.readCurrent()/1000), Serial.print(",");
    Serial.print(laser_power.readBusVoltage()/1000), Serial.print(",");
    Serial.print(laser_power.readPower()/1000), Serial.print(",");
    Serial.print(power.readCurrent()/1000), Serial.print(",");
    Serial.print(power.readBusVoltage()/1000), Serial.print(",");
    Serial.println(power.readPower()/1000);
    time += timer;
    print_counter = 0;
}

void setup() {

  //setup serial comms and print confirmation
  Serial.begin(115200);
  // Wait until serial port is opened
  while (!Serial) { delay(10); }

  if (!power.begin()) {
    Serial.println("Couldn't find INA260 chip");
    while (1);
  }

  if (!laser_power.begin((uint8_t)65)) {
    Serial.println("Couldn't find INA260 chip B");
    while (1);
}

  if (!potentiometer.begin()) {
    Serial.println("Couldn't find DS3502 chip");
    while (1);
}

  Serial.println("Date and Time, Running Time - S, Laser Temp - C, OPC Temp - C, Control Voltage - V, Driver Current - A, Laser Voltage - V, Laser Current - A, Laser Power - W, OPC Voltage - V, OPC Current - A, OPC Power - W");
  //start the temp sensors
  laser_thermo.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
  opc_thermo.begin(MAX31865_3WIRE);
  potentiometer.setWiper(control_voltage/0.0390625);
  //calculate the frequency to print data to 
  //serial terminal
  pinMode(LASER_CONTROL, OUTPUT);
  pulse_time = pulse_time/2;
  print_timer = ((1000/pulse_time) * print_timer)/2; 
  print_data();
}

void loop() {

  if (pulse == 1){
    //turn laser on
    digitalWrite(LASER_CONTROL, HIGH);
    delay(pulse_time);

    // if its time to print the sensor data
    //then print the data
    if (print_counter > print_timer){
      print_data();
      print_counter = 0;
      delay(pulse_time);
    }

    //turn laser off
    digitalWrite(LASER_CONTROL, LOW);
    delay(pulse_time);
    print_counter += 1;
    }

  else{
    digitalWrite(LASER_CONTROL, HIGH);
    print_data();
    delay(timer*1000);
  }
}
