// ECE 355, LAB B03
// Demonstration date: 23 November 2023
// Authors: Joshua Stein V00951354

#include <stdio.h>
#include "diag/Trace.h"
#include <string.h>
#include "cmsis/cmsis_device.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int initial_input = 1; // 0/1 = NE555 Timer (PA1) / Function Generator (PA2)
uint32_t Cycles = 1;
uint32_t Res = 1;

void oled_Write(unsigned char);
void oled_Write_Cmd(unsigned char);
void oled_Write_Data(unsigned char);
void oled_config(void);
void refresh_OLED(void);
SPI_HandleTypeDef SPI_Handle;


// LED Display initialization commands
unsigned char oled_init_cmds[] =
{
    0xAE,
    0x20, 0x00,
    0x40,
    0xA0 | 0x01,
    0xA8, 0x40 - 1,
    0xC0 | 0x08,
    0xD3, 0x00,
    0xDA, 0x32,
    0xD5, 0x80,
    0xD9, 0x22,
    0xDB, 0x30,
    0x81, 0xFF,
    0xA4,
    0xA6,
    0xAD, 0x30,
    0x8D, 0x10,
    0xAE | 0x01,
    0xC0,
    0xA0
};


// Character specifications for LED Display (1 row = 8 bytes = 1 ASCII character)
unsigned char Characters[][8] = {
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b01011111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // !
    {0b00000000, 0b00000111, 0b00000000, 0b00000111, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // "
    {0b00010100, 0b01111111, 0b00010100, 0b01111111, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // #
    {0b00100100, 0b00101010, 0b01111111, 0b00101010, 0b00010010,0b00000000, 0b00000000, 0b00000000},  // $
    {0b00100011, 0b00010011, 0b00001000, 0b01100100, 0b01100010,0b00000000, 0b00000000, 0b00000000},  // %
    {0b00110110, 0b01001001, 0b01010101, 0b00100010, 0b01010000,0b00000000, 0b00000000, 0b00000000},  // &
    {0b00000000, 0b00000101, 0b00000011, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // '
    {0b00000000, 0b00011100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // (
    {0b00000000, 0b01000001, 0b00100010, 0b00011100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // )
    {0b00010100, 0b00001000, 0b00111110, 0b00001000, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // *
    {0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // +
    {0b00000000, 0b01010000, 0b00110000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ,
    {0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // -
    {0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // .
    {0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // /
    {0b00111110, 0b01010001, 0b01001001, 0b01000101, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // 0
    {0b00000000, 0b01000010, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // 1
    {0b01000010, 0b01100001, 0b01010001, 0b01001001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // 2
    {0b00100001, 0b01000001, 0b01000101, 0b01001011, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // 3
    {0b00011000, 0b00010100, 0b00010010, 0b01111111, 0b00010000,0b00000000, 0b00000000, 0b00000000},  // 4
    {0b00100111, 0b01000101, 0b01000101, 0b01000101, 0b00111001,0b00000000, 0b00000000, 0b00000000},  // 5
    {0b00111100, 0b01001010, 0b01001001, 0b01001001, 0b00110000,0b00000000, 0b00000000, 0b00000000},  // 6
    {0b00000011, 0b00000001, 0b01110001, 0b00001001, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // 7
    {0b00110110, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // 8
    {0b00000110, 0b01001001, 0b01001001, 0b00101001, 0b00011110,0b00000000, 0b00000000, 0b00000000},  // 9
    {0b00000000, 0b00110110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // :
    {0b00000000, 0b01010110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ;
    {0b00001000, 0b00010100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // <
    {0b00010100, 0b00010100, 0b00010100, 0b00010100, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // =
    {0b00000000, 0b01000001, 0b00100010, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // >
    {0b00000010, 0b00000001, 0b01010001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // ?
    {0b00110010, 0b01001001, 0b01111001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // @
    {0b01111110, 0b00010001, 0b00010001, 0b00010001, 0b01111110,0b00000000, 0b00000000, 0b00000000},  // A
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // B
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00100010,0b00000000, 0b00000000, 0b00000000},  // C
    {0b01111111, 0b01000001, 0b01000001, 0b00100010, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // D
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // E
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // F
    {0b00111110, 0b01000001, 0b01001001, 0b01001001, 0b01111010,0b00000000, 0b00000000, 0b00000000},  // G
    {0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // H
    {0b01000000, 0b01000001, 0b01111111, 0b01000001, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // I
    {0b00100000, 0b01000000, 0b01000001, 0b00111111, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // J
    {0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // K
    {0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // L
    {0b01111111, 0b00000010, 0b00001100, 0b00000010, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // M
    {0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // N
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // O
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // P
    {0b00111110, 0b01000001, 0b01010001, 0b00100001, 0b01011110,0b00000000, 0b00000000, 0b00000000},  // Q
    {0b01111111, 0b00001001, 0b00011001, 0b00101001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // R
    {0b01000110, 0b01001001, 0b01001001, 0b01001001, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // S
    {0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // T
    {0b00111111, 0b01000000, 0b01000000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // U
    {0b00011111, 0b00100000, 0b01000000, 0b00100000, 0b00011111,0b00000000, 0b00000000, 0b00000000},  // V
    {0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // W
    {0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011,0b00000000, 0b00000000, 0b00000000},  // X
    {0b00000111, 0b00001000, 0b01110000, 0b00001000, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // Y
    {0b01100001, 0b01010001, 0b01001001, 0b01000101, 0b01000011,0b00000000, 0b00000000, 0b00000000},  // Z
    {0b01111111, 0b01000001, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // [
    {0b00010101, 0b00010110, 0b01111100, 0b00010110, 0b00010101,0b00000000, 0b00000000, 0b00000000},  // back slash
    {0b00000000, 0b00000000, 0b00000000, 0b01000001, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // ]
    {0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100,0b00000000, 0b00000000, 0b00000000},  // ^
    {0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // _
    {0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // `
    {0b00100000, 0b01010100, 0b01010100, 0b01010100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // a
    {0b01111111, 0b01001000, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // b
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // c
    {0b00111000, 0b01000100, 0b01000100, 0b01001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // d
    {0b00111000, 0b01010100, 0b01010100, 0b01010100, 0b00011000,0b00000000, 0b00000000, 0b00000000},  // e
    {0b00001000, 0b01111110, 0b00001001, 0b00000001, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // f
    {0b00001100, 0b01010010, 0b01010010, 0b01010010, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // g
    {0b01111111, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // h
    {0b00000000, 0b01000100, 0b01111101, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // i
    {0b00100000, 0b01000000, 0b01000100, 0b00111101, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // j
    {0b01111111, 0b00010000, 0b00101000, 0b01000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // k
    {0b00000000, 0b01000001, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // l
    {0b01111100, 0b00000100, 0b00011000, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // m
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // n
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // o
    {0b01111100, 0b00010100, 0b00010100, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // p
    {0b00001000, 0b00010100, 0b00010100, 0b00011000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // q
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // r
    {0b01001000, 0b01010100, 0b01010100, 0b01010100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // s
    {0b00000100, 0b00111111, 0b01000100, 0b01000000, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // t
    {0b00111100, 0b01000000, 0b01000000, 0b00100000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // u
    {0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // v
    {0b00111100, 0b01000000, 0b00111000, 0b01000000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // w
    {0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // x
    {0b00001100, 0b01010000, 0b01010000, 0b01010000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // y
    {0b01000100, 0b01100100, 0b01010100, 0b01001100, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // z
    {0b00000000, 0b00001000, 0b00110110, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // {
    {0b00000000, 0b00000000, 0b01111111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // |
    {0b00000000, 0b01000001, 0b00110110, 0b00001000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // }
    {0b00001000, 0b00001000, 0b00101010, 0b00011100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // ~
    {0b00001000, 0b00011100, 0b00101010, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000}   // <-
};


void SystemClock48MHz( void )
{
    // Disable the PLL
    RCC->CR &= ~(RCC_CR_PLLON);

    // Wait for the PLL to unlock
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
    
    // Configure the PLL for a 48MHz system clock
    RCC->CFGR = 0x00280000;

    // Enable the PLL
    RCC->CR |= RCC_CR_PLLON;

    // Wait for the PLL to lock
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );

    // Switch the processor to the PLL clock source
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;

    // Update the system with the new clock frequency
    SystemCoreClockUpdate();
}


// Used by user button, NE555 timer, function generator, 4N35 optocoupler, potentiometer
void myGPIOA_Init()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // enable clock for GPIOA peripheral

    // USER BUTTON - PA0
	GPIOA->MODER &= ~(GPIO_MODER_MODER0); // input mode (0)
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR0); // low speed (0)
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_0); // push-pull output (0)
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0); // no pull-up/pull-down (0)

    // NE555 TIMER - PA1
	GPIOA->MODER &= ~(GPIO_MODER_MODER1); // input mode (0)
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR1); // low speed (0)
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_1); // push-pull output (0)
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1); // no pull-up/pull-down (0)

    // FUNCTION GENERATOR - PA2
	GPIOA->MODER &= ~(GPIO_MODER_MODER2); // input mode (0)
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR2); // low speed (0)
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_2); // push-pull output (0)
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2); // no pull-up/pull-down (0)

    // 4N35 OPTOCOUPLER - PA4
	GPIOA->MODER |= (GPIO_MODER_MODER4); // analog mode (3)
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR4); // low speed (0)
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_4); // push-pull output (0)
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4); // no pull-up/pull-down (0)

    // POTENTIOMETER - PA5
    GPIOA->MODER |= (GPIO_MODER_MODER5); // analog mode (3)
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR5); // low speed (0)
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5); // push-pull output (0)
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5); // no pull-up/pull-down (0)
}


void myGPIOB_Init()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // enable clock for GPIOA peripheral

    // SERIAL CLOCK - PB3
	GPIOB->MODER &= ~(GPIO_MODER_MODER3_Msk); // clear bits
	GPIOB->MODER |= (GPIO_MODER_MODER3_1); // AF mode (2)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL3_Msk); // AF0 (SPI1_SCK, I2S1_CK)
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR3); // low speed (0)
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_3); // push-pull output (0)
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR3); // no pull-up/pull-down (0)

    // RESET - PB4
	GPIOB->MODER &= ~(GPIO_MODER_MODER4_Msk); // clear bits
	GPIOB->MODER |= (GPIO_MODER_MODER4_0); // General Output (1)
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR4); // low speed (0)
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_4); // push-pull output (0)
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR4); // no pull-up/pull-down (0)

    // SERIAL DATA - PB5
	GPIOB->MODER &= ~(GPIO_MODER_MODER5_Msk); // clear bits
	GPIOB->MODER |= (GPIO_MODER_MODER5_1); // AF mode (2)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL5_Msk); // AF0 (SPI1_MOSI, I2S1_SD)
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR5); // low speed (0)
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_5); // push-pull output (0)
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR5); // no pull-up/pull-down (0)

    // CHIP SELECT - PB6
	GPIOB->MODER &= ~(GPIO_MODER_MODER6_Msk); // clear bits
	GPIOB->MODER |= (GPIO_MODER_MODER6_0); // General Output (1)
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR6); // low speed (0)
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_6); // push-pull output (0)
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6); // no pull-up/pull-down (0)

    // DATA / COMMAND - PB7
	GPIOB->MODER &= ~(GPIO_MODER_MODER7_Msk); // clear bits
	GPIOB->MODER |= (GPIO_MODER_MODER7_0); // General Output (1)
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR7); // low speed (0)
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_7); // push-pull output (0)
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR7); // no pull-up/pull-down (0)
}


void myEXTI_Init()
{
    // USER BUTTON - EXTI0, PA0
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA; // map EXTI0 to Port A
	EXTI->RTSR |= EXTI_RTSR_TR0; // set EXTI0 rising-edge trigger
	EXTI->IMR |= EXTI_IMR_MR0;  // enable EXTI0 interrupts

    // NE555 TIMER - EXTI1, PA1
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA; // map EXTI1 to Port A
	EXTI->RTSR |= EXTI_RTSR_TR1; // set EXTI1 rising-edge trigger

    // FUNCTION GENERATOR - EXTI2, PA2
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA; // map EXTI2 to Port A
	EXTI->RTSR |= EXTI_RTSR_TR2; // set EXTI2 rising-edge trigger

	EXTI->IMR |= (initial_input ? EXTI_RTSR_TR2 : EXTI_RTSR_TR1); // enable EXTI1 xor EXTI2 interrupts

	NVIC_SetPriority(EXTI0_1_IRQn, 0); // deprioritise level for EXTI0_1_IRQn interrupts
	NVIC_SetPriority(EXTI2_3_IRQn, 0); // deprioritise level for EXTI2_3_IRQn interrupts
	NVIC_EnableIRQ(EXTI0_1_IRQn); // enable IRQ line for EXTI0_1_IRQn interrups
	NVIC_EnableIRQ(EXTI2_3_IRQn); // enable IRQ line for EXTI2_3_IRQn interrups
}


void myTIM3_Init()
{
    // Note: Only 16 bits! Need to prescale.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // enable clock for TIM2 peripheral
	TIM3->PSC = ((uint16_t)47999); // 48MHz clock -> prescale to increment clock count every ms
    TIM3->DIER &= ~(TIM_DIER_UIE_Msk); // disable interrupts
    TIM2->SR &= ~(TIM_SR_UIF_Msk); // clear UIF

    // 0 CEN=0 (disable counter)
    // 1 UDIS=0 (enable update event requests)
    // 2 URS=1 (update request only on underflow/overflow)
    // 3 OPM=1 (stop counting on next update event)
    // 4 DIR=0 (count up)
	// 7 ARPE=1 (buffer ARR)
	TIM3->CR1 = ((uint16_t)0x008C);
	TIM3->ARR = ((uint16_t)0xFFFF); // max value for now
	TIM3->EGR = ((uint16_t)0x0001); // push updates into timer hardware registers
}


void wait(uint16_t wait_ms) {

	// We'll use TIM3 to wait
    TIM3->CR1 &= ((uint16_t)0x0001); // stop timer
    TIM3->CNT = 0; // reset counter
    TIM3->ARR = 0xFFFF;
	TIM3->EGR = ((uint16_t)0x0001); // push updates into timer hardware registers
    TIM3->CR1 |= ((uint16_t)0x0001); // start timer

    while (TIM3->CNT < wait_ms); // wait for wait_ms
    TIM3->CR1 &= ((uint16_t)0x0001); // stop timer
}


void myTIM2_Init()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // enable clock for TIM2 peripheral

    // 0 CEN=0 (disable counter)
    // 1 UDIS=0 (enable update event requests)
    // 2 URS=1 (update request only on underflow/overflow)
    // 3 OPM=1 (stop counting on next update event)
    // 4 DIR=0 (count up)
	// 7 ARPE=1 (buffer ARR)
	TIM2->CR1 = ((uint16_t)0x008C);
	TIM2->PSC = ((uint16_t)0); // don't slow count
	TIM2->ARR = ((uint32_t)0xFFFFFFFF); // max count
	TIM2->EGR = ((uint16_t)0x0001); // push updates into timer hardware registers

    NVIC_SetPriority(TIM2_IRQn, 0); // deprioritise level for TIM2_IRQn interrupts
	NVIC_EnableIRQ(TIM2_IRQn); // enable NVIC interrupts on line 15 for TIM2_IRQn interrupts
	TIM2->DIER |= TIM_DIER_UIE; // enable interrupts from TIM2
}


void myADC_Init() {
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN; // enable clock for ADC1 peripheral

    // 12-bit resolution, right-aligned, overwrite on overrun
    ADC1->CFGR1 &= ((uint32_t)0x00003038); // mask bits we care about
    ADC1->CFGR1 |= ((uint32_t)0x00003000); // set bits we care about

    ADC1->SMPR |= ((uint32_t)0x00000007); // 239.5 ADC clock cycles
    ADC1->CHSELR |= (ADC_CHSELR_CHSEL5); // select ADC channel 5 (corresponds to PA5)

    ADC1->CR |= ((uint32_t)0x00000001); // enable ADC
    while (!(ADC1->ISR & 1)); // wait for ADC ready flag
    ADC1->CR |= ((uint32_t)0x00000004); // start ADC
}


void myDAC_Init() {
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;

    // enable channel1, enable tri-state buffer, disable channel1 trigger
    DAC1->CR &= ~((uint32_t)0x00000007); // mask bits we care about
    DAC1->CR |= 0x00000001; // set bits we care about
}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI0_1_IRQHandler()
{
	// EXTI0 interrupt (user button)
	if (EXTI->PR & EXTI_PR_PR0)
	{
	    trace_printf("User button pressed\n");
		EXTI->PR = EXTI_PR_PR0; // remove pending bit 0 by writing 1

        TIM2->CR1 &= ~((uint16_t)0x0001); // disable timer
        EXTI->IMR ^= (EXTI_RTSR_TR1 | EXTI_RTSR_TR2); // switch input sources
        if (EXTI->IMR & EXTI_RTSR_TR1) trace_printf("Listening to NE555 Timer\n");
        if (EXTI->IMR & EXTI_RTSR_TR2) trace_printf("Listening to Function Generator\n");
	}

	// EXTI1 interrupt (NE555 TIMER)
	if (EXTI->PR & EXTI_PR_PR1)
	{
		EXTI->PR = EXTI_PR_PR1; // remove pending bit 0 by writing 1

        // if timer is running
	    if (TIM2->CR1 & 1) {
            TIM2->CR1 &= ~((uint16_t)0x0001); // disable timer
            Cycles = TIM2->CNT;
            // trace_printf("555 timer freq: %u\n", 48e6 / Cycles);
        }

        else if (EXTI->IMR & EXTI_RTSR_TR1) {
            TIM2->CNT = 0; // reset counter register
            TIM2->CR1 |= 1; // start period measurement
        }
	}
}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI2_3_IRQHandler()
{
	// EXTI2 interrupt (FUNCTION GENERATOR)
	if (EXTI->PR & EXTI_PR_PR2)
	{
		EXTI->PR = EXTI_PR_PR2; // remove pending bit 0 by writing 1

        // if timer is running
	    if (TIM2->CR1 & 1) {
            TIM2->CR1 &= ~((uint16_t)0x0001); // disable timer
            Cycles = TIM2->CNT;
            //trace_printf("cycles: %u, function gen freq: %u\n", Cycles, 48000000 / Cycles);
            //trace_printf("cycles: %u, function gen freq: u\n", Cycles);
        }

        else if (EXTI->IMR & EXTI_RTSR_TR2) {
            TIM2->CNT = 0; // reset counter register
            TIM2->CR1 |= 1; // start period measurement
        }
	}
}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler()
{
	// if UIF is set
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");
		TIM2->SR &= ~(TIM_SR_UIF); // clear UIF
		TIM2->CR1 |= TIM_CR1_CEN; // restart timer
	}
}


int
main(int argc, char* argv[])
{
	SystemClock48MHz();

	myTIM3_Init();
	myTIM2_Init();
    myGPIOA_Init();
    myGPIOB_Init();
	myEXTI_Init();
    myADC_Init();
    myDAC_Init();
	oled_config();

    while (1) {
        while (!(ADC1->ISR & 4)); // wait for ADC channel conversion to complete
        uint32_t voltage_drop = ADC1->DR & 0x00000FFF;
        Res = voltage_drop * 5000 / 4095;
        DAC1->DHR12R1 = voltage_drop; // send resistance to optocoupler

		refresh_OLED();
    }
}


//
// LED Display Functions
//


void refresh_OLED(void)
{
    unsigned char Buffer[17]; // Buffer size = at most 16 characters per PAGE + terminating '\0'

    snprintf( Buffer, sizeof( Buffer ), "R: %5u Ohms", Res ); // Buffer contains character ASCII codes for LED Display
    oled_Write_Cmd(0xB2); // select page 2 (LED Display line)
    oled_Write_Cmd(0x03); // select segment 3 (column)
    oled_Write_Cmd(0x10);

    // for each c = ASCII code, send 8 bytes in Characters[c][0-7] to LED Display
    for (int i = 0; Buffer[i]; i++) {
        for (int j = 0; j < 8; j++) {
            oled_Write_Data(Characters[Buffer[i]][j]);
        }
    }

    snprintf( Buffer, sizeof( Buffer ), "F: %5u Hz", 48000000 / Cycles ); // Buffer contains character ASCII codes for LED Display
    oled_Write_Cmd(0xB3); // select page 3 (LED Display line)
    oled_Write_Cmd(0x04); // select segment 4 (column)
    oled_Write_Cmd(0x10);

    // for each c = ASCII code, send 8 bytes in Characters[c][0-7] to LED Display
    for (int i = 0; Buffer[i]; i++) {
        for (int j = 0; j < 8; j++) {
            oled_Write_Data(Characters[Buffer[i]][j]);
        }
    }

    wait(100); // wait for ~100 ms to get ~10 frames/sec refresh rate
}


void oled_Write_Cmd( unsigned char cmd )
{
    GPIOB->BSRR |= (1 << 6); // make PB6 = CS# = 1
    GPIOB->BSRR |= (1 << (7 + 16)); // make PB7 = D/C# = 0
    GPIOB->BSRR |= (1 << (6 + 16)); // make PB6 = CS# = 0
    oled_Write(cmd);
    GPIOB->BSRR |= (1 << 6); // make PB6 = CS# = 1
}

void oled_Write_Data( unsigned char data )
{
    GPIOB->BSRR |= (1 << 6); // make PB6 = CS# = 1
    GPIOB->BSRR |= (1 << 7); // make PB7 = D/C# = 1
    GPIOB->BSRR |= (1 << (6 + 16)); // make PB6 = CS# = 0
    oled_Write(data);
    GPIOB->BSRR |= (1 << 6); // make PB6 = CS# = 1
}


void oled_Write( unsigned char Value )
{
    while (!(SPI1->SR & SPI_SR_TXE)); // wait for room in Tx FIFO (SPI1 ready for writing)

    // Send one 8-bit character (this function also sets BIDIOE = 1 in SPI1_CR1)
    HAL_SPI_Transmit( &SPI_Handle, &Value, 1, HAL_MAX_DELAY );

    while (!(SPI1->SR & SPI_SR_TXE)); // wait for room in Tx FIFO (SPI1 transmission is complete)
}


void oled_config( void )
{
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI_Handle.Instance = SPI1;

    SPI_Handle.Init.Direction = SPI_DIRECTION_1LINE;
    SPI_Handle.Init.Mode = SPI_MODE_MASTER;
    SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SPI_Handle.Init.NSS = SPI_NSS_SOFT;
    SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_Handle.Init.CRCPolynomial = 7;

    HAL_SPI_Init(&SPI_Handle); // initialize the SPI interface
    __HAL_SPI_ENABLE(&SPI_Handle); // enable SPI

    // Reset LED Display (RES# = PB4):
    GPIOB->BSRR |= (1 << (4 + 16)); // enable #RST signal (PB4 = 0)
    wait(400);
    GPIOB->BSRR |= (1 << 4); // disable #RST signal (PB4 = 1)
    wait(400);

    // Send initialization commands to LED Display
    for ( unsigned int i = 0; i < sizeof(oled_init_cmds); i++ )
        oled_Write_Cmd(oled_init_cmds[i]);

    // Zero LED Display data memory (GDDRAM)
    for (unsigned char page = 0; page < 8; page++) {
        oled_Write_Cmd(0xB0 | page); // select page (LED Display line)
        oled_Write_Cmd(0x02); // select segment 2 (column)
        oled_Write_Cmd(0x10);
        for (int i = 0; i < 128; i++) oled_Write_Data(0x00); // segment is incremented for us
    }
}



#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
