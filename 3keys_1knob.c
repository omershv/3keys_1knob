// ===================================================================================
// Project:   MacroPad Mini for CH551, CH552 and CH554
// Version:   v1.1
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Firmware example implementation for the MacroPad Mini.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: min. 12 MHz internal
// - Adjust the firmware parameters in include/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - Press a macro key and see what happens.
// - To enter bootloader hold down key 1 while connecting the MacroPad to USB. All
//   NeoPixels will light up white as long as the device is in bootloader mode
//   (about 10 seconds).


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include <config.h>                         // user configurations
#include <system.h>                         // system functions
#include <delay.h>                          // delay functions
#include <neo.h>                            // NeoPixel functions
#include <usb_conkbd.h>                     // USB HID consumer keyboard functions

// Prototypes for used interrupts
void USB_interrupt(void);

void USB_ISR(void)

__interrupt(INT_NO_USB) {
        USB_interrupt();
}

// ===================================================================================
// NeoPixel Functions
// ===================================================================================

// Update NeoPixels
void NEO_update(uint8_t *neo1_rgb, uint8_t *neo2_rgb, uint8_t *neo3_rgb, float neo1_brightness, float neo2_brightness, float neo3_brightness) {
    __idata
    uint8_t values[] = {neo1_rgb[0] * neo1_brightness, neo1_rgb[1] * neo1_brightness, neo1_rgb[2] * neo1_brightness,
                        neo2_rgb[0] * neo2_brightness, neo2_rgb[1] * neo2_brightness, neo2_rgb[2] * neo2_brightness,
                        neo3_rgb[0] * neo3_brightness, neo3_rgb[1] * neo3_brightness, neo3_rgb[2] * neo3_brightness};

    EA = 0;                                   // disable interrupts
    for (uint8_t i = 0; i < 9; i++) {
        NEO_sendByte(values[i]);
    }
    EA = 1;                                   // enable interrupts
}

// Read EEPROM (stolen from https://github.com/DeqingSun/ch55xduino/blob/ch55xduino/ch55xduino/ch55x/cores/ch55xduino/eeprom.c)
uint8_t eeprom_read_byte(uint8_t addr) {
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    ROM_ADDR_L = addr << 1; //Addr must be even
    ROM_CTRL = ROM_CMD_READ;
    return ROM_DATA_L;
}

// Presses keys simultaneously
void press_keys(char keys[]) {
    for (uint8_t i = 0; i < 4; i++) {
        if (keys[i] != 255) {
            KBD_press(keys[i]);
        }
    }
    DLY_ms(1);
    for (uint8_t i = 0; i < 4; i++) {
        if (keys[i] != 255) {
            KBD_release(keys[i]);
        }
    }
}

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
    // Variables

    // NeoPixel variables
    __idata float
            neo1_brightness = 1;                 // brightness of NeoPixel 1
    __idata float
            neo2_brightness = 1;                 // brightness of NeoPixel 2
    __idata float
            neo3_brightness = 1;                 // brightness of NeoPixel 3


    __bit key1last = 0;                       // last state of key 1
    __bit key2last = 0;                       // last state of key 2
    __bit key3last = 0;                       // last state of key 3
    __bit knobswitchlast = 0;                 // last state of knob switch
    __idata
    uint8_t i;                        // temp variable

    // Enter bootloader if key 1 is pressed
    NEO_init();                               // init NeoPixels
    if (!PIN_read(PIN_KEY1)) {                 // key 1 pressed?
        NEO_latch();                            // make sure pixels are ready
        for (i = 9; i; i--) NEO_sendByte(127);     // light up all pixels
        BOOT_now();                             // enter bootloader
    }

    // Setup
    CLK_config();                             // configure system clock
    DLY_ms(5);                                // wait for clock to settle
    KBD_init();                               // init USB HID keyboard
    WDT_start();                              // start watchdog timer

    i = 0;
    char key1_chars[] = {(char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++)};
    char key2_chars[] = {(char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++)};
    char key3_chars[] = {(char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++)};
    char knobsw_chars[] = {(char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++)};
    char knobclockwise_chars[] = {(char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++)};
    char knobcounterclockwise_chars[] = {(char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++), (char) eeprom_read_byte(i++)};
    uint8_t neo1_rgb[] = {eeprom_read_byte(i++), eeprom_read_byte(i++), eeprom_read_byte(i++)};
    uint8_t neo2_rgb[] = {eeprom_read_byte(i++), eeprom_read_byte(i++), eeprom_read_byte(i++)};
    uint8_t neo3_rgb[] = {eeprom_read_byte(i++), eeprom_read_byte(i++), eeprom_read_byte(i++)};


    // Loop
    while (1) {
        // Handle key 1
        if (!PIN_read(PIN_KEY1) != key1last) {   // key 1 state changed?
            key1last = !key1last;                 // update last state flag
            if (key1last) {                        // key was pressed?
                neo1_brightness = 1;                         // light up corresponding NeoPixel
                press_keys(key1_chars);                // press and release
            } else {                                // key was released?
                // nothing to do in this case
            }
        } else if (key1last) {                     // key still being pressed?
            neo1_brightness = 1;                           // keep NeoPixel on
        }

        // Handle key 2
        if (!PIN_read(PIN_KEY2) != key2last) {   // key 2 state changed?
            key2last = !key2last;                 // update last state flag
            if (key2last) {                        // key was pressed?
                neo2_brightness = 1;                         // light up corresponding NeoPixel
                press_keys(key2_chars);                // press and release
            } else {                                // key was released?
                // nothing to do in this case
            }
        } else if (key2last) {                     // key still being pressed?
            neo2_brightness = 1;                           // keep NeoPixel on
        }

        // Handle key 3
        if (!PIN_read(PIN_KEY3) != key3last) {   // key 3 state changed?
            key3last = !key3last;                 // update last state flag
            if (key3last) {                        // key was pressed?
                neo3_brightness = 1;                         // light up corresponding NeoPixel
                press_keys(key3_chars);                // press and release
            } else {                                // key was released?
                // nothing to do in this case
            }
        } else if (key3last) {                     // key still being pressed?
            neo3_brightness = 1;                           // keep NeoPixel on
        }

        // Handle knob switch
        if (!PIN_read(PIN_ENC_SW) != knobswitchlast) {
            knobswitchlast = !knobswitchlast;
            if (knobswitchlast) {
                press_keys(knobsw_chars);
            } else {
            }
        } else if (knobswitchlast) {
        }

        // Handle knob

        if (!PIN_read(PIN_ENC_A)) {                       // encoder turned ?
            if (PIN_read(PIN_ENC_B)) {
                press_keys(knobclockwise_chars);         // clockwise?
            } else {
                press_keys(knobcounterclockwise_chars);  // counter-clockwise?
            }
            DLY_ms(10);                                    // debounce
            while (!PIN_read(PIN_ENC_A));                   // wait until next detent
        }


        // Update NeoPixels
        NEO_update(neo1_rgb, neo2_rgb, neo3_rgb, neo1_brightness, neo2_brightness, neo3_brightness);
        if (neo1_brightness > 0) neo1_brightness -= 0.01;                        // fade down NeoPixel
        if (neo2_brightness > 0) neo2_brightness -= 0.01;                        // fade down NeoPixel
        if (neo3_brightness > 0) neo3_brightness -= 0.01;                        // fade down NeoPixel
        DLY_ms(5);                              // latch and debounce
        WDT_reset();                            // reset watchdog
    }
}
