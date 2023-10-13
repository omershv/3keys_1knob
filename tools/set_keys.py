import yaml

KEY_MAPPING = {
    'LEFT_CTRL': 0x80, 'LEFT_SHIFT': 0x81, 'LEFT_ALT': 0x82, 'LEFT_GUI': 0x83,
    'RIGHT_CTRL': 0x84, 'RIGHT_SHIFT': 0x85, 'RIGHT_ALT': 0x86, 'RIGHT_GUI': 0x87,
    'UP_ARROW': 0xDA, 'DOWN_ARROW': 0xD9, 'LEFT_ARROW': 0xD8, 'RIGHT_ARROW': 0xD7,
    'BACKSPACE': 0xB2, 'TAB': 0xB3, 'RETURN': 0xB0, 'ESC': 0xB1,
    'INSERT': 0xD1, 'DELETE': 0xD4, 'PAGE_UP': 0xD3, 'PAGE_DOWN': 0xD6,
    'HOME': 0xD2, 'END': 0xD5, 'CAPS_LOCK': 0xC1,
    'F1': 0xC2, 'F2': 0xC3, 'F3': 0xC4, 'F4': 0xC5, 'F5': 0xC6, 'F6': 0xC7,
    'F7': 0xC8, 'F8': 0xC9, 'F9': 0xCA, 'F10': 0xCB, 'F11': 0xCC, 'F12': 0xCD,
    'F13': 0xF0, 'F14': 0xF1, 'F15': 0xF2, 'F16': 0xF3, 'F17': 0xF4,
    'F18': 0xF5, 'F19': 0xF6, 'F20': 0xF7, 'F21': 0xF8, 'F22': 0xF9,
    'F23': 0xFA, 'F24': 0xFB
}

REVERSE_KEY_MAPPING = {value: key for key, value in KEY_MAPPING.items()}


def translate_to_code(value):
    if value in KEY_MAPPING:
        return KEY_MAPPING[value]
    else:
        return ord(value)


def translate_to_string(value):
    return REVERSE_KEY_MAPPING.get(value, chr(value))


def write_to_bin_file(key_codes, rgb_values):
    flat_key_codes = [item for sublist in key_codes for item in sublist]
    flat_rgb_values = [item for sublist in rgb_values for item in sublist]
    with open('flashdata.bin', 'wb') as f:
        f.write(bytes(flat_key_codes + flat_rgb_values))


def read_from_bin_file():
    try:
        with open('flashdata.bin', 'rb') as f:
            data = f.read()
            key_codes = [list(data[i:i + 4]) for i in range(0, 24, 4)]
            rgb_values = [list(data[i:i + 3]) for i in range(24, 33, 3)]
            return key_codes, rgb_values
    except FileNotFoundError:
        return [[0] * 4] * 6, [[0] * 3] * 3


def write_to_yaml_file(data):
    with open('flashdata.yaml', 'w') as f:
        yaml.dump(data, f)


def read_from_yaml_file():
    with open('flashdata.yaml', 'r') as f:
        return yaml.safe_load(f)


def main():
    key_codes, rgb_values = read_from_bin_file()

    data = {
        'keys': {f'key{i + 1}': [translate_to_string(code) for code in key] for i, key in enumerate(key_codes)},
        'leds': {f'led{i + 1}': {'R': r, 'G': g, 'B': b} for i, (r, g, b) in enumerate(rgb_values)}
    }

    write_to_yaml_file(data)

    print("Generated flashdata.yaml. Edit this file and press Enter to continue.")
    input()

    updated_data = read_from_yaml_file()
    updated_key_codes = [[translate_to_code(code) for code in updated_data['keys'][f'key{i + 1}']] for i in range(6)]
    updated_rgb_values = [[updated_data['leds'][f'led{i + 1}']['R'], updated_data['leds'][f'led{i + 1}']['G'], updated_data['leds'][f'led{i + 1}']['B']] for i
                          in range(3)]

    write_to_bin_file(updated_key_codes, updated_rgb_values)
    print("Updated flashdata.bin based on the edited flashdata.yaml.")


if __name__ == '__main__':
    main()
