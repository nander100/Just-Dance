
#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "serialATmega.h" 
#include <stdio.h>
#include "usart_ATmega328p.h" 
#include "gyro.h"
#include "timerISR.h"

#define F_CPU 16000000UL   // CPU clock speed
#define F_SCL 100000UL     // SCL clock speed (100kHz)
#define TWBR_VALUE ((F_CPU / F_SCL - 16) / 2)

#define MPU6050_ADDR 0b1101000

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

void mpu6050_write_register(uint8_t reg, uint8_t value) {
    i2c_start();
    i2c_write(MPU6050_ADDR << 1);
    i2c_write(reg);
    i2c_write(value);
    i2c_stop();
}

void mpu6050_read_registers(uint8_t reg, uint8_t* buffer, uint8_t length) {
  i2c_start();
  i2c_write(MPU6050_ADDR << 1);
  i2c_write(reg);
  i2c_start();
  i2c_write((MPU6050_ADDR << 1) | 1);
  for (uint8_t i = 0; i < length; i++) {
    if (i < length - 1) {
      buffer[i] = i2c_read_ack();
    } 
    else {
      buffer[i] = i2c_read_nack();
    }
  }
  i2c_stop();
}

void setup_mpu6050(void) {
  mpu6050_write_register(0x6B, 0x00); // Wake up MPU6050
  mpu6050_write_register(0x1B, 0x00); // Put the gyro in +/- 250 degrees/s mode 
  mpu6050_write_register(0x1C, 0x00); // Set accelerometer full scale range to +/- 2g
}

void read_acl(void) {
    uint8_t buffer[6];
    mpu6050_read_registers(0x3B, buffer, 6);
    accelX = (buffer[0] << 8) | buffer[1];
    accelY = (buffer[2] << 8) | buffer[3];
    accelZ = (buffer[4] << 8) | buffer[5];
}

void scale_data_acl(void) {
    gForceX = accelX / 16384.0;
    gForceY = accelY / 16384.0;
    gForceZ = accelZ / 16384.0;
}

void read_gyro(void) {
    uint8_t buffer[6];
    mpu6050_read_registers(0x43, buffer, 6);
    gyroX = (buffer[0] << 8) | buffer[1];
    gyroY = (buffer[2] << 8) | buffer[3];
    gyroZ = (buffer[4] << 8) | buffer[5];
}

void scale_data_gyro(void) {
    rotX = gyroX / 131.0;
    rotY = gyroY / 131.0;
    rotZ = gyroZ / 131.0;
}
// enum BTSendStates { BT_SEND_INIT, BT_WAIT_SEND_READY, BT_SEND_X, BT_WAIT_X, BT_SEND_Y, BT_WAIT_Y, BT_SEND_Z, BT_WAIT_Z, BT_WAIT_RECEIVED };
enum BTSendStates { BT_SEND_INIT, BT_SEND };
unsigned char sendGyroVals = 0x00;
int SendBluetoothTick(int state) {
  switch(state) {
    case BT_SEND_INIT:
      state = BT_SEND;
      break;
    case BT_SEND:
      // USART_Send(0xFC); // flag for start
      if(gForceZ > 0.8) {
        sendGyroVals += 1;
        sendGyroVals = sendGyroVals << 1;
      }
      else {
        sendGyroVals = sendGyroVals << 1;
      }
      if(gForceY > 0.8) {
        sendGyroVals += 1;
        sendGyroVals = sendGyroVals << 1;
      }
      else {
        sendGyroVals = sendGyroVals << 1;
      }
      if(gForceX > 0.8) {
        sendGyroVals += 1;
        sendGyroVals = sendGyroVals << 1;
      }
      else {
        sendGyroVals = sendGyroVals << 1;
      }
      USART_Send(sendGyroVals);
      sendGyroVals = 0x00;
      break;
    default:
      break;
  }
  return state;
}

// DEFINE TASK
typedef struct _task{
  signed   char state;    //Task's current state
  unsigned long period;     //Task period
  unsigned long elapsedTime;  //Time elapsed since last task tick
  int (*TickFct)(int);    //Task tick function
} task;



#define NUM_TASKS 1
unsigned int GCD_PERIOD = 10;
unsigned int BT_SEND_PERIOD = 10;
task tasks[NUM_TASKS]; // DECLARE TASK ARRAY

void TimerISR() {
  read_acl();
  scale_data_acl();
  read_gyro();
  scale_data_gyro();
  for ( unsigned int i = 0; i < NUM_TASKS; i++ ) {     
    read_acl();
    scale_data_acl();
    read_gyro();
    scale_data_gyro();              
    // Iterate through each task in the task array
    if ( tasks[i].elapsedTime == tasks[i].period ) {           // Check if the task is ready to tick
      tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
      tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
    }
    tasks[i].elapsedTime += GCD_PERIOD;                        // Increment the elapsed time by GCD_PERIOD
  }
}

int main(void) {
    serial_init(9600);
    // initUSART();
    i2c_init();
    // TimerSet(GCD_PERIOD);
    // TimerOn();
    setup_mpu6050();
    DDRD = 0xFE; PORTD = ~DDRD; //1111 1110
    DDRC = 0xFF; PORTC = 0x00;

    while (1) {
      sendGyroVals = 0x00;
      read_acl();
      scale_data_acl();
      read_gyro();
      scale_data_gyro();
    }

    return 0;
}