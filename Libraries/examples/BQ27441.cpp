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
// Set BATTERY_CAPACITY to the design capacity of your battery.
const unsigned int BATTERY_CAPACITY = 8600; // e.g. 850mAh battery


void setupBQ27441(void)
{
  // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
  // connected and communicating.
  if (!lipo.begin()) // begin() will return true if communication is successful
  {
	// If communication fails, print an error message and loop forever.
    printf("Error: Unable to communicate with BQ27441. \n");
    printf("  Check wiring and try again. \n");
    printf("  (Battery must be plugged into Battery Babysitter!) \n");
    while (1) ;
  }

  printf("Connected to BQ27441! \n");
  unsigned int dev = lipo.deviceType();
  printf("Device Type is: %#x \n", dev);
  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity %#x
  // of your battery.
  //lipo.setCapacity(BATTERY_CAPACITY);
  if (!lipo.setCapacity(BATTERY_CAPACITY)) // begin() will return true if communication is successful
  {
	// If communication fails, print an error message and loop forever.
    printf("Error: Unable to set BQ27441 capacity. \n");
    while (1) ;
  }

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
          printBatteryStats();
          delay(1000);
        }
        return (0);
        }
