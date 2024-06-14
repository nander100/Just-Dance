

#ifndef SPIAVR_H
#define SPIAVR_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "helper.h"

// PIN DEFINE STATEMENTS
#define PIN_SCK PORTB5 // SPI Clock (SCK) - should always be B5 on the Arduino
#define PIN_MOSI PORTB3 // Master Out Slave In (MOSI) - should always be B3 on the Arduino
#define PIN_SS PORTB2 // Slave Select (SS) - can be any digital pin on the Arduino

#define SKA 5 // PB5
#define SDA 3 // PB3
#define AO 1 // PB1
#define RST 4 // PB0
#define CS 2 // PB2

#define BLACK           0x0000
#define WHITE           0xFFFF
#define BLUE            0xF800
#define GREEN           0x07E0
#define RED             0x001F
#define YELLOW          0x07FF
#define PINK            0xF81F
#define CYAN            0xFFE0
#define MAGENTA         0xFC18
#define GRAY            0x8410

#define RINIT 1
#define GINIT 1


const unsigned int font[42][10] = {
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0xC300, 0xC300, 0xC300, 0xC300, 0xFF00, 0x7E00}, // '0'
    {0x0000, 0x1800, 0x3800, 0x7800, 0x1800, 0x1800, 0x1800, 0x1800, 0xFF00, 0xFF00}, // '1'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0x0600, 0x1C00, 0x3000, 0x6000, 0xFF00, 0xFF00}, // '2'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0x0600, 0x1E00, 0x0600, 0xC300, 0xFF00, 0x7E00}, // '3'
    {0x0000, 0x0600, 0x1E00, 0x3600, 0x6600, 0xFF00, 0xFF00, 0x0600, 0x0600, 0x0600}, // '4'
    {0x0000, 0xFF00, 0xFF00, 0xC000, 0xFE00, 0xFF00, 0x0300, 0xC300, 0xFF00, 0x7E00}, // '5'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0xFE00, 0xFF00, 0xC300, 0xC300, 0xFF00, 0x7E00}, // '6'
    {0x0000, 0xFF00, 0xFF00, 0x0300, 0x0600, 0x0C00, 0x1800, 0x3000, 0x3000, 0x3000}, // '7'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0x7E00, 0x7E00, 0xC300, 0xC300, 0xFF00, 0x7E00}, // '8'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0xFF00, 0x7F00, 0x0300, 0xC300, 0xFF00, 0x7E00}, // '9'
    {0x0000, 0x0000, 0x7C00, 0x7C00, 0x0000, 0x0000, 0x7C00, 0x7C00, 0x0000, 0x0000}, // ':'
    {0x0000, 0x0000, 0x7C00, 0x7C00, 0x0000, 0x0000, 0x7C00, 0x7C00, 0x0C00, 0x1800}, // ';'
    {0x0000, 0x0600, 0x1C00, 0x3000, 0xC000, 0xC000, 0x3000, 0x1C00, 0x0600, 0x0000}, // '<'
    {0x0000, 0x0000, 0xFF00, 0xFF00, 0x0000, 0x0000, 0xFF00, 0xFF00, 0x0000, 0x0000}, // '='
    {0x0000, 0x6000, 0x3800, 0x0C00, 0x0300, 0x0300, 0x0C00, 0x3800, 0x6000, 0x0000}, // '>'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0x0600, 0x1C00, 0x1800, 0x0000, 0x1800, 0x0000}, // '?'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0xDB00, 0xDB00, 0xDF00, 0xC000, 0xFF00, 0x7E00}, // '@'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0xC300, 0xFF00, 0xFF00, 0xC300, 0xC300, 0xC300}, // 'A'
    {0x0000, 0xFC00, 0xFE00, 0xC300, 0xFE00, 0xFE00, 0xC300, 0xC300, 0xFE00, 0xFC00}, // 'B'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0xC000, 0xC000, 0xC300, 0xFF00, 0x7E00, 0x0000}, // 'C'
    {0x0000, 0xFC00, 0xFE00, 0xC300, 0xC300, 0xC300, 0xC300, 0xFE00, 0xFC00, 0x0000}, // 'D'
    {0x0000, 0xFF00, 0xFF00, 0xC000, 0xFE00, 0xFE00, 0xC000, 0xFF00, 0xFF00, 0x0000}, // 'E'
    {0x0000, 0xFF00, 0xFF00, 0xC000, 0xFE00, 0xFE00, 0xC000, 0xC000, 0xC000, 0x0000}, // 'F'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0xC000, 0xCF00, 0xC300, 0xFF00, 0x7F00, 0x0000}, // 'G'
    {0x0000, 0xC300, 0xC300, 0xC300, 0xFF00, 0xFF00, 0xC300, 0xC300, 0xC300, 0x0000}, // 'H'
    {0x0000, 0x7E00, 0x7E00, 0x1800, 0x1800, 0x1800, 0x1800, 0x7E00, 0x7E00, 0x0000}, // 'I'
    {0x0000, 0xFF00, 0xFF00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0xCC00, 0xFC00, 0x7800}, // 'J'
    {0x0000, 0xC300, 0xC300, 0xC600, 0xCC00, 0xF800, 0xF800, 0xCC00, 0xC600, 0xC300}, // 'K'
    {0x0000, 0xC000, 0xC000, 0xC000, 0xC000, 0xC000, 0xC000, 0xFF00, 0xFF00, 0x0000}, // 'L'
    {0x0000, 0xC300, 0xE700, 0xFF00, 0xDB00, 0xC300, 0xC300, 0xC300, 0xC300, 0x0000}, // 'M'
    {0x0000, 0xC300, 0xE300, 0xF300, 0xDB00, 0xCF00, 0xC700, 0xC300, 0xC300, 0x0000}, // 'N'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0xC300, 0xC300, 0xC300, 0xFF00, 0x7E00, 0x0000}, // 'O'
    {0x0000, 0xFC00, 0xFE00, 0xC300, 0xC300, 0xFE00, 0xFC00, 0xC000, 0xC000, 0x0000}, // 'P'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0xC300, 0xCF00, 0xCC00, 0xFF00, 0x7E00, 0x0300}, // 'Q'
    {0x0000, 0xFC00, 0xFE00, 0xC300, 0xC300, 0xFE00, 0xFC00, 0xCC00, 0xC600, 0xC300}, // 'R'
    {0x0000, 0x7E00, 0xFF00, 0xC300, 0x6000, 0x1C00, 0x0300, 0xC300, 0xFF00, 0x7E00}, // 'S'
    {0x0000, 0xFF00, 0xFF00, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x0000}, // 'T'
    {0x0000, 0xC300, 0xC300, 0xC300, 0xC300, 0xC300, 0xC300, 0xC300, 0xFF00, 0x7E00}, // 'U'
    {0x0000, 0xC300, 0xC300, 0xC300, 0xC300, 0x6600, 0x6600, 0x3C00, 0x1800, 0x0000}, // 'V'
    {0x0000, 0xC300, 0xC300, 0xC300, 0xDB00, 0xFF00, 0x7E00, 0x6600, 0x6600, 0x0000}, // 'W'
    {0x0000, 0xC300, 0x6600, 0x3C00, 0x1800, 0x3C00, 0x6600, 0xC300, 0xC300, 0x0000}, // 'X'
    {0x0000, 0xC300, 0xC300, 0x6600, 0x3C00, 0x1800, 0x1800, 0x1800, 0x1800, 0x0000}, // 'Y'
};


// SPI SEND FUNCTION
void SPI_SEND(char data) {
    SPDR = data; // Set data to transmit
    while (!(SPSR & (1 << SPIF))); // Wait until done transmitting
}

// SPI INIT FUNCTION
void SPI_INIT() {
    DDRB |= (1 << PIN_SCK) | (1 << PIN_MOSI) | (1 << PIN_SS); // Initialize your pins
    SPCR |= (1 << SPE) | (1 << MSTR); // Initialize SPI communication
}

// HARDWARE RESET FUNCTION
void hardwareRst() {
    PORTB = SetBit(PORTB, RST, 0); // Set RST pin to low
    _delay_ms(200);
    PORTB = SetBit(PORTB, RST, 1); // Set RST pin to high
    _delay_ms(200);
}

// SEND COMMAND FUNCTION
void command(char command) {
    PORTB = SetBit(PORTB, AO, 0); // AO = off
    PORTB = SetBit(PORTB, CS, 0); // CS = off
    SPI_SEND(command); // Send the command
    PORTB = SetBit(PORTB, CS, 1); // CS = on
}

// SEND DATA FUNCTION
void data(char dat) {
    PORTB = SetBit(PORTB, AO, 1); // AO = on
    PORTB = SetBit(PORTB, CS, 0); // CS = off
    SPI_SEND(dat); // Send the data
    PORTB = SetBit(PORTB, CS, 1); // CS = on
}

// SET COLUMN FUNCTION
void set_column(unsigned int start, unsigned int end) {
    command(0x2A); // CASET
    data(start >> 8);
    data(start & 0xFF);
    data(end >> 8);
    data(end & 0xFF);
}

// SET ROW FUNCTION
void set_row(unsigned int start, unsigned int end) {
    command(0x2B); // RASET
    data(start >> 8);
    data(start & 0xFF);
    data(end >> 8);
    data(end & 0xFF);
}

// WRITE PIXEL FUNCTION
void write_pixel(unsigned int color) {
    data((color >> 8) & 0xFF);
    data(color & 0xFF);
}

// POPULATE SCREEN FUNCTION
void populate_screen(unsigned int color) {
    set_column(0, 127);
    set_row(0, 159);
    command(0x2C); // RAMWR
    for (uint32_t i = 0; i < 128 * 160; ++i) {
        write_pixel(color);
    }
}

void draw_large_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, float scale) {
    // int index = c - ' ';
    int index = c - 48;
    for (int i = 0; i < 10; i++) {
        uint16_t line = font[index][i];
        for (int j = 0; j < 16; j++) {
            uint16_t bit = (line >> (15 - j)) & 1;
            for (int k = 0; k < scale; k++) {
                for (int l = 0; l < scale; l++) {
                    set_column(x + (j * scale) + k, x + (j * scale) + k + 1);
                    set_row(y + (i * scale) + l, y + (i * scale) + l + 1);
                    command(0x2C); // RAMWR
                    if (bit) {
                        write_pixel(color);
                    } 
                    else {
                        write_pixel(bg);
                    }
                }
            }
        }
    }
}


void draw_line(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    for (uint16_t i = 0; i < width; i++) {
        for (uint16_t j = 0; j < height; j++) {
            set_column(x + i, x + i + 1);
            set_row(y + j, y + j + 1);
            command(0x2C); // RAMWR
            write_pixel(color);
        }
    }
}

unsigned char stickFigureSide[12] = {
    0b00111100,  //   ****
    0b01000010,  //  *    *
    0b01000010,  //  *    *
    0b01000010,  //  *    *
    0b00111100,  //   ****
    0b00011000,  //    **
    0b00011000,  //    **
    0b11111111,  // ********
    0b00011000,  //    **
    0b00100100,  //   *  *
    0b01000010,  //  *    *
    0b10000001   // *      *
};

unsigned char stickFigureUp[12] = {
    0b00111100,  //   ****
    0b01000010,  //  *    *
    0b01000010,  //  *    *
    0b01000010,  //  *    *
    0b00111100,  //   ****
    0b00011001,  //    **  *
    0b00011010,  //    ** *
    0b11111100,  // ****** 
    0b00011000,  //    **
    0b00100100,  //   *  *
    0b01000010,  //  *    *
    0b10000001   // *      *
};

unsigned char stickFigureDown[12] = {
    0b00111100,  //   ****
    0b01000010,  //  *    *
    0b01000010,  //  *    *
    0b01000010,  //  *    *
    0b00111100,  //   ****
    0b00011000,  //    **  
    0b00011000,  //    ** 
    0b11111100,  // ****** 
    0b00011010,  //    ** *
    0b00100101,  //   *  * *
    0b01000010,  //  *    *
    0b10000001   // *      *
};

void displayUpMan()   { 
    unsigned int current_line = 0;
    int xVal = 40;
    int yVal = 30; 
    for (unsigned int i = 0; i < 12; i++) {
        current_line = stickFigureUp[i];
        for (unsigned int j = 0; j < 8; ++j ){
            if (((current_line << j) &0x80) > 1) {
                // draw_large_char(xVal, yVal, '@', WHITE, BLACK, size);
                draw_line(xVal, yVal, 5, 5, WHITE); // draws a 5x5 box to the display
            }
            xVal+=5;
        }
        xVal = 40;
        yVal += 5;
    }
    return; 
}

void displayUpManOff() {
    unsigned int current_line = 0;
    int xVal = 40;
    int yVal = 30;
    for (unsigned int i = 0; i < 12; i++) {
        current_line = stickFigureUp[i];
        for (unsigned int j = 0; j < 8; ++j ){
            if (((current_line << j) &0x80) > 1) {
                draw_line(xVal, yVal, 5, 5, BLACK); // draws a 5x5 box to the display
            }
            xVal+=5;
        }
        xVal = 40;
        yVal += 5;
    }
    return; 
}


void displayDownMan() { 
    unsigned int current_line = 0;
    int xVal = 40;
    int yVal = 30;
    for (unsigned int i = 0; i < 12; i++) {
        current_line = stickFigureDown[i];
        for (unsigned int j = 0; j < 8; ++j ){
            if (((current_line << j) & 0x80) > 1) {
                draw_line(xVal, yVal, 5, 5, WHITE); // draws a 5x5 box to the display
            }
            xVal+=5;
        }
        xVal = 40;
        yVal += 5;
    }
    return;   
}

void displayDownManOff() {
    unsigned int current_line = 0;
    int xVal = 40;
    int yVal = 30;
    for (unsigned int i = 0; i < 12; i++) {
        current_line = stickFigureDown[i];
        for (unsigned int j = 0; j < 8; ++j ){
            if (((current_line << j) & 0x80) > 1) {
                draw_line(xVal, yVal, 5, 5, BLACK); // draws a 5x5 box to the display
            }
            xVal+=5;
        }
        xVal = 40;
        yVal += 5;
    }
    return; 
}


void displaySideMan() { 
    unsigned int current_line = 0;
    int xVal = 40;
    int yVal = 30; 
    for (unsigned int i = 0; i < 12; i++) {
        current_line = stickFigureSide[i];
        for (unsigned int j = 0; j < 8; ++j ){
            if (((current_line << j) &0x80) > 1) {
                // draw_large_char(xVal, yVal, '@', WHITE, BLACK, size);
                draw_line(xVal, yVal, 5, 5, WHITE); // draws a 5x5 box to the display
            }
            xVal+=5;
        }
        xVal = 40;
        yVal += 5;
    } 
    return; 
}

void displaySideManOff() {
    unsigned int current_line = 0;
    int xVal = 40;
    int yVal = 30; 
    for (unsigned int i = 0; i < 12; i++) {
        current_line = stickFigureSide[i];
        for (unsigned int j = 0; j < 8; ++j ){
            if (((current_line << j) &0x80) > 1) {
                // draw_large_char(xVal, yVal, '@', WHITE, BLACK, size);
                draw_line(xVal, yVal, 5, 5, BLACK); // draws a 5x5 box to the display
            }
            xVal+=5;
        }
        xVal = 40;
        yVal += 5;
    } 
    return;
}

unsigned int redYlocation = 110;
unsigned int redXlocation = 1;
void displayRed() {
    if(redXlocation < 128) {
        draw_line(redXlocation, redYlocation, 10, 20, RED);
        redXlocation+=10;
    }
}

unsigned int greenYlocation = 1;
unsigned int greenXlocation = 1;
void displayGreen() {
    if(greenXlocation < 128) {
        draw_line(greenXlocation, greenYlocation, 10, 20, GREEN);
        greenXlocation+=10;
    }
}


#endif /* SPIAVR_H */
