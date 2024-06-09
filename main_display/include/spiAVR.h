

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

// DRAW CHARACTER FUNCTION
void draw_char(unsigned int x, unsigned int y, char c, unsigned int color) {
    if (c < ' ' || c > '~') return; // Check for valid ASCII range
    int index = c - ' ';
    set_column(x, x + 9); // Adjusted for larger font
    set_row(y, y + 19); // Adjusted for larger font
    command(0x2C); // RAMWR
    for (int i = 0; i < 10; i++) {
        uint16_t line = font[index][i];
        for (int j = 0; j < 16; j++, line >>= 1) {
            for (int k = 0; k < 2; k++) {
                for (int l = 0; l < 2; l++) {
                    set_column(x + (j * 2) + k, x + (j * 2) + k + 1);
                    set_row(y + (i * 2) + l, y + (i * 2) + l + 1);
                    if (line & 1) {
                        write_pixel(color);
                    } else {
                        write_pixel(0x0000); // Background color (black)
                    }
                }
            }
        }
    }
}

// DRAW STRING FUNCTION
void draw_string(unsigned int x, unsigned int y, const char* str, unsigned int color) {
    while (*str) {
        draw_char(x, y, *str, color);
        y += 20; // Move to the next character position downwards (20 for scaled font)
        if (y + 10 >= 160) { // If we reach the end of the column
            y = 0; // Reset y to the start of the next column
            x += 20; // Move right by the width of the character (20 for scaled font)
        }
        str++;
    }
}

// ST7735 INITIALIZATION FUNCTION
// 

// POPULATE SCREEN FUNCTION
void populate_screen(unsigned int color) {
    set_column(0, 127);
    set_row(0, 159);
    command(0x2C); // RAMWR
    for (uint32_t i = 0; i < 128 * 160; ++i) {
        write_pixel(color);
    }
}




// #ifndef ST77DISPLAY_H
// #define ST77DISPLAY_H

// #include "helper.h"
// #include "spiAVR.h"
// #include "serialATmega.h"
// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>

// #define RES_PIN PB4
// #define AO_PIN PB1
// #define CS_PIN PB2

// #define AO_OFF          PORTB &= ~(1 << AO_PIN)
// #define AO_ON           PORTB |= (1 << AO_PIN)
// #define CS_OFF          PORTB &= ~(1 << CS_PIN)
// #define CS_ON           PORTB |= (1 << CS_PIN)
// #define RESET_OFF       PORTB &= ~(1 << RES_PIN)
// #define RESET_ON        PORTB |= (1 << RES_PIN)

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

// int size, size2;

// void send_command(uint8_t cmd) {
//     AO_OFF;
//     CS_OFF;
//     SPI_SEND(cmd);
//     CS_ON;
// }

// void pass_data(uint8_t data) {
//     AO_ON;
//     CS_OFF;
//     SPI_SEND(data);
//     CS_ON;
// }

// void set_column(uint16_t start, uint16_t end) {
//     send_command(0x2A);
//     pass_data(start >> 8);
//     pass_data(start & 0xFF);
//     pass_data(end >> 8);
//     pass_data(end & 0xFF);
// }

// void set_row(uint16_t start, uint16_t end) {
//     send_command(0x2B);
//     pass_data(start >> 8);
//     pass_data(start & 0xFF);
//     pass_data(end >> 8);
//     pass_data(end & 0xFF);
// }

// void write_pixel(uint16_t color) {
//     pass_data((color >> 8) & 0xFF);
//     pass_data(color & 0xFF);
// }

// void populate_screen(uint16_t color)
// {
//     set_column(0, 129);
//     set_row(0, 200);
//     send_command(0x2C);

//     for (uint32_t i = 0; i < 129 * 200; ++i){
//          write_pixel(color);
//     }
// }

// void write_to_screen(int startRow, int endRow, int startColumn, int endColumn, int color) {
//     set_column(startColumn, endColumn);
//     set_row(startRow, endRow);
//     send_command(0x2C);

//     for (int i = startRow; i < (endRow - startRow + 1) * (endColumn - startColumn + 1); ++i) {
//         write_pixel(color);
//     }
// }

// void SPI_SEND(uint8_t data) {
//     SPDR = data;
//     while(!(SPSR & (1 << SPIF))); 
// }

// void reset_whole() {
//   RESET_OFF;
//   _delay_ms(200);
//   RESET_ON;
//   _delay_ms(200);
// }

// void ST7735_Init() {
//     hardwareRst();
//     SPI_SEND(0x01); // Software reset
//     _delay_ms(150);
//     SPI_SEND(0x11); // Exit sleep mode
//     _delay_ms(200);
//     SPI_SEND(0x36); // Set memory access mode
//     SPI_SEND(0x06);
//     _delay_ms(10);
//     SPI_SEND(0x29); // Display on
//     _delay_ms(200);
// }

// void ST7735_Init() {
//     hardwareRst();
//     PORTB = SetBit(PORTB, 1, 0);
//     PORTB = SetBit(PORTB, 2, 0);
//     _delay_ms(200);
//     command(0x11);
//     _delay_ms(200);
//     command(0x36);
//     data(0x00); 
//     command(0x3A); 
//     data(0x05); 
//     command(0x29); 
//     _delay_ms(200);
//     set_column(0, 127);
//     set_row(0, 159);
//     data(0x2C); 
//     for (uint32_t i = 0; i < 128 * 160; i++) {
//         write_pixel(0x001F); 
//     }
// }


void draw_large_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, float scale) {
    if (c < ' ' || c > '~') return; // Check for valid ASCII range

    // int index = c - ' ';
    int index = c - 48;

    // uint16_t scaled_width = (uint16_t)(16 * scale);
    // uint16_t scaled_height = (uint16_t)(10 * scale);
    
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
                    } else {
                        write_pixel(bg);
                    }
                }
            }
        }
    }
}

void draw_large_string(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg, float scale, uint16_t maxY, uint16_t spacing) {
    uint16_t pixel_size = (uint16_t)scale; // Calculate the size of each pixel block based on the scaling factor
    while (*str) {
        draw_large_char(x, y, *str, color, bg, scale);
        y += (16 * pixel_size / 2) + spacing; // Move to the next character position downwards with spacing
        if (y + (16 * pixel_size / 2) >= maxY) { // If we reach the end of the column
            y = 0;    // Reset y to the start of the next column
            x += (10 * pixel_size / 2) + spacing;  // Move right by the width of the character with spacing
        }
        str++;
    }
}


// void draw_title(int state) {
//     switch(state) {
//         case 0:
//             populate_screen(BLACK);
//             draw_large_string(20, 7, "BOP", PINK, BLACK, 4.0, 200, 5); // White text on black background
//             draw_large_string(60, 12, "  *", YELLOW, BLACK, 3.0, 200, 5);
//             draw_large_string(60, 12, "IT", WHITE, BLACK, 3.0, 200, 5);
//             draw_large_string(63, 98, "S", GRAY, BLACK, 1.25, 200, 5);
//             draw_large_string(74, 98, "T", GRAY, BLACK, 1.29, 200, 5);
//             draw_large_string(85, 98, "A", GRAY, BLACK, 1.25, 200, 5);
//             draw_large_string(96, 98, "R", GRAY, BLACK, 1.25, 200, 5);
//             draw_large_string(107, 98, "T", GRAY, BLACK, 1.29, 200, 5);
//             break;
//         case 1:
//             draw_large_string(63, 98, "S", BLACK, BLACK, 1.25, 200, 5);
//             draw_large_string(74, 98, "T", BLACK, BLACK, 1.29, 200, 5);
//             draw_large_string(85, 98, "A", BLACK, BLACK, 1.25, 200, 5);
//             draw_large_string(96, 98, "R", BLACK, BLACK, 1.25, 200, 5);
//             draw_large_string(107, 98, "T", BLACK, BLACK, 1.29, 200, 5);
//             break;
//     }
// }

// void draw_damage(int state) {
//     switch(state) {
//         case 0:
//             populate_screen(BLACK);
//             draw_large_string(97, 88, "X", RED, BLACK, 3, 200, 5);
//             break;
//         case 1:
//             draw_large_string(97, 88, "X", WHITE, BLACK, 3, 200, 5);
//             break;
//     }
// }

// void draw_correct(int state) {
//     switch(state) {
//         case 0:
//             populate_screen(BLACK);
//             draw_large_string(10, 88, "N", GREEN, BLACK, 2.5, 200, 5);
//             draw_large_string(32, 88, "I", GREEN, BLACK, 2.5, 200, 5);
//             draw_large_string(54, 88, "C", GREEN, BLACK, 2.5, 200, 5);
//             draw_large_string(76, 88, "E", GREEN, BLACK, 2.5, 200, 5);
//             draw_large_string(98, 88, "!", GREEN, BLACK, 2.5, 200, 5);
//             break;
//         case 1:
//             draw_large_string(10, 88, "!", BLACK, BLACK, 2.5, 200, 5);
//             draw_large_string(32, 88, "E", BLACK, BLACK, 2.5, 200, 5);
//             draw_large_string(54, 88, "C", BLACK, BLACK, 2.5, 200, 5);
//             draw_large_string(76, 88, "I", BLACK, BLACK, 2.5, 200, 5);
//             draw_large_string(98, 88, "N", BLACK, BLACK, 2.5, 200, 5);
//             break;
//     }
// }

// void draw_bop_task(int state) {
//     size = 2.75;
//     size2 = 4.5;
//     switch(state) {
//         case 0:
//             populate_screen(BLACK);
//             draw_large_string(12, 25, "B", BLUE, BLACK, size, 200, 5);
//             draw_large_string(34, 25, "O", BLUE, BLACK, size, 200, 5);
//             draw_large_string(56, 25, "P", BLUE, BLACK, size, 200, 5);
//             draw_large_string(10, 50, "I", YELLOW, BLACK, size2, 200, 5);
//             draw_large_string(45, 50, "T", YELLOW, BLACK, size2, 200, 5);
//             draw_large_string(75, 50, "!", YELLOW, BLACK, size2, 200, 5);
//             break;
//         case 1:
//             draw_large_string(12, 25, "B", BLACK, BLACK, size, 200, 5);
//             draw_large_string(34, 25, "O", BLACK, BLACK, size, 200, 5);
//             draw_large_string(56, 25, "P", BLACK, BLACK, size, 200, 5);
//             draw_large_string(10, 50, "I", BLACK, BLACK, size2, 200, 5);
//             draw_large_string(45, 50, "T", BLACK, BLACK, size2, 200, 5);
//             draw_large_string(75, 50, "!", BLACK, BLACK, size2, 200, 5);
//             break;
//     }
// }

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
    int xVal = 30;
    int yVal = 30;
    float size = 0.5; 
    for (unsigned int i = 0; i < 12; i++) {
        current_line = stickFigureUp[i];
        for (unsigned int j = 0; j < 8; ++j ){
            if (((current_line << j) &0x80) > 1) {
                draw_large_char(xVal, yVal, '@', WHITE, BLACK, size);
            }
            xVal+=10;
        }
        xVal = 30;
        yVal += 5;
    }
    return; 
}

void displayUpManOff() {
    unsigned int current_line = 0;
    int xVal = 30;
    int yVal = 30;
    float size = 0.5; 
    for (unsigned int i = 0; i < 12; i++) {
        current_line = stickFigureUp[i];
        for (unsigned int j = 0; j < 8; ++j ){
            if (((current_line << j) &0x80) > 1) {
                draw_large_char(xVal, yVal, '@', BLACK, BLACK, size);
            }
            xVal+=10;
        }
        xVal = 30;
        yVal += 5;
    }
    return; 
}

void displayDownMan() { 
    unsigned int current_line = 0;
    int xVal = 30;
    int yVal = 30;
    float size = 0.5; 
    for (unsigned int i = 0; i < 12; i++) {
        current_line = stickFigureDown[i];
        for (unsigned int j = 0; j < 8; ++j ){
            if (((current_line << j) &0x80) > 1) {
                draw_large_char(xVal, yVal, '@', WHITE, BLACK, size);
            }
            xVal+=10;
        }
        xVal = 30;
        yVal += 5;
    }
    return; 
}
void displaySideMan() { return; }
void displayEarly(){ return; }
void displayNice() { return; }
void displayLate() { return; }

#endif /* SPIAVR_H */









void draw_test(int num) {
    float size = 0.5;
    int size2 = 4.5;
    switch(num) {
        case 0:
            populate_screen(BLACK);
            draw_large_string(12, 25, "@", RED, BLACK, size, 200, 5);
            draw_large_string(32, 25, "O", RED, BLACK, size, 200, 5);
            draw_large_string(52, 25, "R", RED, BLACK, size, 200, 5);
            draw_large_string(72, 25, "N", RED, BLACK, size, 200, 5);
            draw_large_string(92, 25, "K", RED, BLACK, size, 200, 5);
            draw_large_string(10, 50, "I", YELLOW, BLACK, size2, 200, 5);
            draw_large_string(45, 50, "T", YELLOW, BLACK, size2, 200, 5);
            draw_large_string(75, 50, "!", YELLOW, BLACK, size2, 200, 5);
            break;
        case 1:
            draw_large_string(12, 25, "T", BLACK, BLACK, size, 200, 5);
            draw_large_string(34, 25, "W", BLACK, BLACK, size, 200, 5);
            draw_large_string(56, 25, "I", BLACK, BLACK, size, 200, 5);
            draw_large_string(78, 25, "S", BLACK, BLACK, size, 200, 5);
            draw_large_string(100, 25, "T", BLACK, BLACK, size, 200, 5);
            draw_large_string(10, 50, "I", BLACK, BLACK, size2, 200, 5);
            draw_large_string(45, 50, "T", BLACK, BLACK, size2, 200, 5);
            draw_large_string(75, 50, "!", BLACK, BLACK, size2, 200, 5);
            break;
        case 2:
            unsigned int current_line = 0;
            int xVal = 30;
            int yVal = 30;
            // if ((stick_figure[0] >> 8) % 2 == 0) {
            //     draw_large_string(xVal, yVal, "@", WHITE, BLACK, size, 200, 5);
            // }
            // else {
            //     draw_large_string(xVal, yVal, "@", BLACK, BLACK, size, 200, 5);
            // }
            // draw_large_string(xVal, yVal, "@", WHITE, YELLOW, size, 200, 5);
            for (unsigned int i = 0; i < 12; i++) {
                current_line = stickFigureSide[i];
                for (unsigned int j = 0; j < 8; ++j ){
                    if (((current_line << j) &0x80) > 1) {
                        draw_large_string(xVal, yVal, "@", WHITE, BLACK, size, 200, 5);
                    }
                    xVal+=10;
                }
                xVal = 30;
                yVal += 5;
            }
            // char wordX = 64;
            // char wordY = 100;
            // char word_size = 2; 
            
            // _delay_ms(1000);
            // draw_large_string(wordX, wordY, "3", BLACK, BLACK, word_size, 200, 5);
            // draw_large_string(wordX, wordY, "2", WHITE, BLACK, word_size, 200, 5);
            // _delay_ms(1000);
            // draw_large_string(xVal, yVal, "1", WHITE, BLACK, word_size, 200, 5);
            // _delay_ms(1000);
            // draw_large_string(xVal, yVal, "GO!", WHITE, BLACK, word_size, 200, 5);
            


            // for (unsigned int i = 0; i < 8; ++i ){
            //     if (((0b00111100 << i) &0x80) > 1) {
            //         draw_large_string(xVal, yVal, "@", WHITE, BLACK, size, 200, 5);
            //     }
            //     xVal+=10;
            // }
            // yVal+=5;
            // for (unsigned int i = 0; i < 8; ++i ){
            //     if (((0b01000010 << i) &0x80) > 1) {
            //         draw_large_string(xVal, yVal, "@", WHITE, BLACK, size, 200, 5);
            //     }
            //     xVal+=15;
            // }
            // for (unsigned int i = 0; i < 15; ++i ) {
            //     current_line = stick_figure[i];
            //     for (unsigned int j = 0; j < 15; ++j) {
            //         if ((current_line >> j) % 2 == 1) {
            //             draw_large_string(xVal, yVal, "@", WHITE, BLACK, size, 200, 5);
            //         }
            //         else {
            //             // draw_large_string(xVal, yVal, "@", BLACK, BLACK, size, 200, 5);
            //         }
            //         xVal += 22;
            //     }
            //     yVal += 25;
            // }
            // draw_large_string(12, 25, "@", BLACK, BLACK, size, 200, 5);
        

    }
    
}