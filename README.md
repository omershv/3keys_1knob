# 3keys_1knob
Custom firmware for a 3-key + rotary encoder macropad (https://hackaday.io/project/189914)

FORKED FROM https://github.com/biemster/3keys_1knob

### putting device in flash mode (required for flashing or setting keys):
- if on original firmware: connect P1.5 to GND and connect USB
- if on this firmware: press key1 while connecting USB

### compile & flash:
`$ build_and_flash.sh`

### configure keys & RGB:
`$ set_keys.sh`
During the execution the file `flashdata.yaml` will be created, edit it with your keys and RGB values
 - Each key can press up to 4 keys simultaneously, you can use ascii values for keys or any of the following values:
    LEFT_CTRL, LEFT_SHIFT, LEFT_ALT, LEFT_GUI, RIGHT_CTRL, RIGHT_SHIFT, RIGHT_ALT, RIGHT_GUI, UP_ARROW, DOWN_ARROW, LEFT_ARROW, RIGHT_ARROW, BACKSPACE, TAB, RETURN, ESC, INSERT, DELETE, PAGE_UP, PAGE_DOWN, HOME, END, CAPS_LOCK, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24


