#include "board.h"
#include "mw.h"

#define L3G4200_ADDRESS         0x69

// Registers
#define L3G4200_WHOAMI 0x0F
#define L3G4200_CTRL_REG1 0x20
#define L3G4200_CTRL_REG2 0x21
#define L3G4200_CTRL_REG3 0x22
#define L3G4200_CTRL_REG4 0x23
#define L3G4200_CTRL_REG5 0x24
#define L3G4200_TEMP_REG 0x26
#define L3G4200_STATUS_REG 0x27
#define L3G4200_OUT_X_L 0x28
#define L3G4200_OUT_X_H 0x29
#define L3G4200_OUT_Y_L 0x2A
#define L3G4200_OUT_Y_H 0x2B
#define L3G4200_OUT_Z_L 0x2C
#define L3G4200_OUT_Z_H 0x2D


static void l3g4200Init(void);
static void l3g4200Read(int16_t *gyroData);
static void l3g4200Align(int16_t *gyroData);

bool l3g4200Detect(sensor_t *gyro)
{
    bool ack;

    delay(25); // datasheet page 13 says 20ms. other stuff could have been running meanwhile. but we'll be safe

    ack = i2cWrite(L3G4200_ADDRESS, L3G4200_CTRL_REG1, 0x0f);
    if (!ack)
        return false;
    /* uartPrint("l3g ack\r\n"); */

    gyro->init = l3g4200Init;
    gyro->read = l3g4200Read;
    gyro->align = l3g4200Align;

    return true;
}

static void l3g4200Init(void)
{
    bool ack;

    delay(25); // datasheet page 13 says 20ms. other stuff could have been running meanwhile. but we'll be safe

    ack = i2cWrite(L3G4200_ADDRESS, L3G4200_CTRL_REG1, 0x8f);
    if (!ack)
        failureMode(3);
    delay(5);
    i2cWrite(L3G4200_ADDRESS, L3G4200_CTRL_REG2, 0x0);
    delay(5);
    i2cWrite(L3G4200_ADDRESS, L3G4200_CTRL_REG3, 0x0);
    delay(5);
    i2cWrite(L3G4200_ADDRESS, L3G4200_CTRL_REG4, 0x0);
    delay(5);
    i2cWrite(L3G4200_ADDRESS, L3G4200_CTRL_REG5, 0x02);
    delay(5);
}

static void l3g4200Align(int16_t *gyroData)
{
    // official direction is RPY
    gyroData[0] = gyroData[0] / 4;
    gyroData[1] = gyroData[1] / 4;
    gyroData[2] = -gyroData[2] / 4;
}

// Read 3 gyro values into user-provided buffer. No overrun checking is done.
static void l3g4200Read(int16_t *gyroData)
{
    uint8_t buf[6], sbuf[1], tbuf[1];
    char abuf[10];

    i2cRead(L3G4200_ADDRESS, L3G4200_STATUS_REG, 1, sbuf);
    i2cRead(L3G4200_ADDRESS, L3G4200_TEMP_REG, 1, tbuf);
    i2cRead(L3G4200_ADDRESS, L3G4200_OUT_X_L, 1, buf);
    i2cRead(L3G4200_ADDRESS, L3G4200_OUT_X_H, 1, buf + 1);
    i2cRead(L3G4200_ADDRESS, L3G4200_OUT_Y_L, 1, buf + 2);
    i2cRead(L3G4200_ADDRESS, L3G4200_OUT_Y_H, 1, buf + 3);
    i2cRead(L3G4200_ADDRESS, L3G4200_OUT_Z_L, 1, buf + 4);
    i2cRead(L3G4200_ADDRESS, L3G4200_OUT_Z_H, 1, buf + 5);
    gyroData[0] = (buf[1] << 8) | buf[0];
    gyroData[1] = (buf[3] << 8) | buf[2];
    gyroData[2] = (buf[5] << 8) | buf[4];

    /*
    uartPrint("l3g: ");
    itoa(sbuf[0], abuf, 16);
    uartPrint(abuf);
    uartPrint(" ");
    itoa(tbuf[0], abuf, 16);
    uartPrint(abuf);
    uartPrint(" ");
    itoa(gyroData[0], abuf, 16);
    uartPrint(abuf);
    uartPrint(" ");
    itoa(gyroData[1], abuf, 16);
    uartPrint(abuf);
    uartPrint(" ");
    itoa(gyroData[2], abuf, 16);
    uartPrint(abuf);
    uartPrint("\r\n");
    */
}
