
// // //////// CODE FOR THE REMOTE ///////

// // #include "TimerISR.h"
// // #include "periph.h"
// // #include "helper.h"   
// // #include "usart_ATmega328p.h" 
// // #include "gyro.h"
// // #include "serialATmega.h"
// // #define NUM_TASKS 0// DEFINE STATEMNETS 

// // // DEFINE TASK
// // typedef struct _task{
// //   signed   char state;    //Task's current state
// //   unsigned long period;     //Task period
// //   unsigned long elapsedTime;  //Time elapsed since last task tick
// //   int (*TickFct)(int);    //Task tick function
// // } task;

// // task tasks[NUM_TASKS]; // DECLARE TASK ARRAY

// // // GLOBAL VARIABLES

// // // variables to store the values of gyro
// // int x_ang = 0;
// // int y_ang = 0;
// // int z_ang = 0;

// // int x_acl = 0;
// // int y_acl = 0;
// // int z_acl = 0;

// // // STATES
// // enum gyroStates { GYRO_INIT, READ_GYRO };
// // enum BTStates   { BT_INIT, BT_TRANSMIT };

// // // TICK FUNCTIONS
// // int ReadGyro(int state) {
// //   switch (state) { // state transitions
// //     i2c_init();
// //   case GYRO_INIT:
// //     state = READ_GYRO;
// //     break;
// //   case READ_GYRO:
// //     state = READ_GYRO;
// //     break;
// //   default:
// //     break;
// //   }; // end state transitions

// //   switch (state) { // state actions
// //   case GYRO_INIT:
// //     break;
// //   case READ_GYRO:
// //     z_acl = i2c_read_ack();
// //     serial_println(z_acl);
// //     break;
// //   default:
// //     break;
// //   }; // end state actions
// //   return state;
// // }

// // int TransmitBT (int state) {

// // }

// // // TASK PERIODS
// // const unsigned long READ_GYRO_PERIOD = 1;
// // const unsigned long GCD_PERIOD = 1;

// // void TimerISR() {
// //   for ( unsigned int i = 0; i < NUM_TASKS; i++ ) {                   // Iterate through each task in the task array
// //     if ( tasks[i].elapsedTime == tasks[i].period ) {           // Check if the task is ready to tick
// //       tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
// //       tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
// //     }
// //     tasks[i].elapsedTime += GCD_PERIOD;                        // Increment the elapsed time by GCD_PERIOD
// //   }
// // }



// // int main(void) {
// //   // INITIALIZE PORT AND DDR REGISTERS
// //   DDRD = 0xFE; PORTD = ~DDRD; //1111 1110
// //   DDRC = 0xFF; PORTC = 0x00;

// //   // TASK INIT
  
// //   // INITIALIZATIONS
// //   // TimerSet(GCD_PERIOD);
// //   // TimerOn();
// //   i2c_init();
// //   initUSART();
// //   serial_init(9600);
// //   int i = 0;
// //   tasks[i].period = READ_GYRO_PERIOD;
// //   tasks[i].state = GYRO_INIT;
// //   tasks[i].elapsedTime = tasks[i].period;
// //   tasks[i].TickFct = &ReadGyro;

// //   while (1) {
// //     // serial_println('0');
// //     I2C_Start(0x50);
// //     // i2c_write(0x48);
// //     I2C_Repeated_Start(0x47);
// //     // i2c_write(0x01);
// //     // Wire.read()<<8|Wire.read()
// //     long z = i2c_read_ack();
// //     serial_println(z);
// //     // serial_println('1');
// //     i2c_stop();
// //     _delay_ms(100);
// //     // ReadGyro();
// //     // USART_Send(0);
// //     // _delay_ms(500);
// //     // USART_Send(1);
// //     // _delay_ms(500);

// //     // PORTD = SetBit(PORTD, 2, 1);  
// //     // USART_Send(1);  
// //     // PORTD = SetBit(PORTD, 2, 1); 
// //     // _delay_ms(500);
// //     // USART_Send(0);  
// //     // PORTD = SetBit(PORTD, 2, 0); 
// //     // _delay_ms(500);
// //   }
// //   return 0;
// // }


// #include <avr/io.h>
// #include <util/twi.h>
// #include "serialATmega.h"

// #define F_CPU 16000000UL   // CPU clock speed
// #define F_SCL 100000UL     // SCL clock speed (100kHz)
// #define TWBR_VALUE ((F_CPU / F_SCL - 16) / 2)

// void i2c_init(void) {
//     // Set TWI bit rate
//     TWBR = (uint8_t)TWBR_VALUE;
//     // Enable TWI
//     TWCR = (1 << TWEN);
// }

// void i2c_start(void) {
//     // Send START condition
//     TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
//     // Wait for TWINT Flag set. This indicates that the START condition has been transmitted.
//     while (!(TWCR & (1 << TWINT)));
// }

// void i2c_stop(void) {
//     // Send STOP condition
//     TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
//     // Wait for STOP condition to be executed
//     while (TWCR & (1 << TWSTO));
// }

// void i2c_write(uint8_t data) {
//     // Load data into TWDR register
//     TWDR = data;
//     // Start transmission of data
//     TWCR = (1 << TWINT) | (1 << TWEN);
//     // Wait for TWINT Flag set. This indicates that the data has been transmitted, and ACK/NACK has been received.
//     while (!(TWCR & (1 << TWINT)));
// }

// uint8_t i2c_read_ack(void) {
//     // Enable TWI, generation of ACK after reception
//     TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
//     while (!(TWCR & (1 << TWINT)));
//     return TWDR;
// }

// uint8_t i2c_read_nack(void) {
//     // Enable TWI, no generation of ACK after reception
//     TWCR = (1 << TWINT) | (1 << TWEN);
//     while (!(TWCR & (1 << TWINT)));
//     return TWDR;
// }

// void i2c_beginTransmission(uint8_t address) {
//     // Send start condition
//     i2c_start();
//     // Send the device address with the write bit
//     i2c_write((address << 1) | TW_WRITE);
// }

// void i2c_endTransmission(void) {
//     // Send stop condition
//     i2c_stop();
// }

// void i2c_requestFrom(uint8_t address, uint8_t quantity) {
//     // Send start condition
//     i2c_start();
//     // Send the device address with the read bit
//     i2c_write((address << 1) | TW_READ);
// }

// int main(void) {
//     uint8_t gyroData[6];
//     int16_t gyroX, gyroY, gyroZ;

//     // Initialize I2C
//     i2c_init();

//     // Begin transmission to device with address 0b1101000 (0x68)
    

//     // Stop the transmission
//     i2c_stop();

//       serial_init(9600);
//     // Combine the bytes into gyroX, gyroY, and gyroZ
    

//     // Your code to use gyroX, gyroY, and gyroZ

//     while (1) {
//       serial_println(7);
//       i2c_beginTransmission(0b1101000);
    
//     // Write starting register for Gyro Readings (0x43)
//       i2c_write(0x43);
      
//       // End the transmission
//       i2c_endTransmission();

//       // Request 6 bytes from device with address 0b1101000 (0x68)
//       i2c_requestFrom(0b1101000, 6);

//       // Read the 6 bytes of data
//       for (uint8_t i = 0; i < 6; i++) {
//           if (i < 5) {
//               gyroData[i] = i2c_read_ack();
//           } else {
//               gyroData[i] = i2c_read_nack();
//           }
//       }
//       gyroX = (gyroData[0] << 8) | gyroData[1];
//       gyroY = (gyroData[2] << 8) | gyroData[3];
//       gyroZ = (gyroData[4] << 8) | gyroData[5];
//       serial_println(gyroZ);
//         // Main loop
//     }
// }





















#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "serialATmega.h" 
#include <stdio.h>
#include "usart_ATmega328p.h" 
#include "gyro.h"

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

int main(void) {
    serial_init(9600);
    i2c_init();
    setup_mpu6050();
    DDRD = 0xFE; PORTD = ~DDRD; //1111 1110
    DDRC = 0xFF; PORTC = 0x00;

    while (1) {
        read_acl();
        scale_data_acl();
        read_gyro();
        scale_data_gyro();
        USART_Send(gForceZ);
    }

    return 0;
}