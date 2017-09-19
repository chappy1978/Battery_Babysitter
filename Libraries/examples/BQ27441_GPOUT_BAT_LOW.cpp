/******************************************************************************
BQ27441_GPOUT_BAT_LOW
BQ27441 Library GPOUT Example - Battery low Mode
Revision by: Jeff Chap @ MakerHQ
Sept 18 2017

Based on:
Jim Lindblom @ SparkFun Electronics
https://github.com/sparkfun/SparkFun_BQ27441_Arduino_Library

Demonstrates how to use the BQ27441's BAT_LOW function on GPOUT. In this mode
GPOUT will become active whenever the battery goes below a set threshold.

In addition to I2C, connect GPOUT to pin 4 of your RaspberryPi.

After uploading, They should only print when the percentage goes up or down
by 1%.

Hardware Resources:
- Raspberry Pi
- SparkFun Battery Babysitter

Development environment specifics:
SparkFun Battery Babysitter v1.0
Raspberry PI
GCC
WiringPi Library
/*****************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <BQ27441.h>

// I2C_ADAPTER "/dev/i2c-1"
const char* DeviceName = "/dev/i2c-1";

// Set BATTERY_CAPACITY to the design capacity of your battery.
const unsigned int BATTERY_CAPACITY = 8600; // e.g. 850mAh battery

const int SOCI_SET = 15; // Interrupt set threshold at 20%
const int SOCI_CLR = 20; // Interrupt clear threshold at 25%
const int SOCF_SET = 5; // Final threshold set at 5%
const int SOCF_CLR = 10; // Final threshold clear at 10%

// Arduino pin connected to BQ27441's GPOUT pin
const int GPOUT_PIN = 4;


void setupBQ27441(void)
{
  // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
  // connected and communicating.
  if (!lipo.begin(DeviceName)) // begin() will return true if communication is successful
  {
	// If communication fails, print an error message and loop forever.
    printf("Error: Unable to communicate with BQ27441. \n");
    printf("  Check wiring and try again. \n");
    printf("  (Battery must be plugged into Battery Babysitter!) \n");
    while (1) ;
  }

  printf("Connected to BQ27441! \n");
  printf("%s\n", DeviceName);
  unsigned int dev = lipo.deviceType();
  printf("Device Type is: %#x \n", dev);
  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity %#x
  // of your battery.

  /*if (!lipo.setCapacity(BATTERY_CAPACITY)) // begin() will return true if communication is successful
  {
	// If communication fails, print an error message and loop forever.
    printf("Error: Unable to set BQ27441 capacity. \n");
    while (1) ;
  }*/
  
  
  // In this example, we'll manually enter and exit config mode. By controlling
  // config mode manually, you can set the chip up faster -- completing all of
  // the set up in a single config mode sweep.
  lipo.enterConfig(); // To configure the values below, you must be in config mode
  lipo.setCapacity(BATTERY_CAPACITY); // Set the battery capacity
  lipo.setGPOUTPolarity(LOW); // Set GPOUT to active-high
  lipo.setGPOUTFunction(BAT_LOW); // Set GPOUT to BAT_LOW mode
  lipo.setSOC1Thresholds(SOCI_SET, SOCI_CLR); // Set SOCI set and clear thresholds
  lipo.setSOCFThresholds(SOCF_SET, SOCF_CLR); // Set SOCF set and clear thresholds
  lipo.exitConfig();

  // Use lipo.GPOUTPolarity to read from the chip and confirm the changes
  if (lipo.GPOUTPolarity())
    printf("GPOUT set to active-HIGH");
  else
    printf("GPOUT set to active-LOW");

  // Use lipo.GPOUTFunction to confirm the functionality of GPOUT
  if (lipo.GPOUTFunction())
    printf("GPOUT function set to BAT_LOW");
  else
    printf("GPOUT function set to SOC_INT");

  // Read the set and clear thresholds to make sure they were set correctly
  printf("SOC1 Set Threshold: %d\n", lipo.SOC1SetThreshold());
  printf("SOC1 Clear Threshold: %d\n", lipo.SOC1ClearThreshold());
  printf("SOCF Set Threshold: %d\n", lipo.SOCFSetThreshold());
  printf("SOCF Clear Threshold: %d\n",lipo.SOCFClearThreshold());


}

void printBatteryStats()
{
  // Read battery stats from the BQ27441-G1A
  unsigned int soc = lipo.soc();  // Read state-of-charge (%)
  float volts = lipo.voltage(); // Read battery voltage (mV)
  volts = volts/1000;
  int current = lipo.current(AVG); // Read average current (mA)
  unsigned int capacity = lipo.capacity(REMAIN); // Read remaining capacity (mAh)
  unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
  int power = lipo.power(); // Read average power draw (mW)
  int health = lipo.soh(PERCENT); // Read state-of-health (%)*/
  float temp = 1.8 * lipo.temperature(INTERNAL_TEMP)/10 - 459.67;

  printf("SOC: %d%%\n", soc);
  printf("Voltage: %4.3fV\n", volts);
  printf("Current: %dmA\n", current);
  printf("Average power draw: %dmW\n", power);
  printf("Remaining capacity: %dmAh\n",capacity);
  printf("Fuel gauge capacity set to, %d \n", fullCapacity);
  printf("State of Health: %d%%\n", health);
  printf("Temperature: %4.2fF\n\n", temp);
}

int main (int argc, char **argv)
{
  wiringPiSetup ();
  setupBQ27441();

        while(1)
        {
          // If the GPOUT interrupt is active (low)
		  if (!digitalRead(GPOUT_PIN))
		  {
			    // Check which of the flags triggered the interrupt
			    if (lipo.socfFlag()) 
				      printf("<!-- WARNING: Battery Dangerously low -->");
			    else if (lipo.socFlag())
				      printf("<!-- WARNING: Battery Low -->");
          }
          printBatteryStats();
          delay(1000);
          

         }
  return (0);
}
