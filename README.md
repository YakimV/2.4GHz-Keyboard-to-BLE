# 2.4GHz Keyboard to BLE

A classic story: I had a great 2.4 GHz wireless keyboard, but the original USB dongle went missing. Instead of throwing it away or letting it gather dust, I decided to not only bring it back to life but also upgrade it into a full-fledged Bluetooth device.

The most straightforward approach was tapping directly into the physical key matrix. The keyboard membrane breaks out into two ribbon cables:
* **A 17-pin ribbon cable** — matrix rows.
* **An 8-pin ribbon cable** — matrix columns.

Naturally, the compact **ESP32-C3 Supermini** doesn't have enough GPIOs to handle that many lines directly. To expand the I/O, I used shift registers:
* **2× 74HC595 (cascaded):** drive the first 16 rows using only 3 GPIO pins on the ESP32.
* **1× 74HC165:** reads the 8 columns via parallel-in/serial-out, with hardware 10kΩ pull-up resistors to 3.3V.
* **Direct Pin:** the 17th row is connected straight to a single spare GPIO on the microcontroller.

### How It Works

The controller sequentially drives each row LOW using the 595 registers and reads the column states through the 165. The state of the entire matrix is tracked in a local array, ensuring reliable key press and release events without repeating artifacts. The inputs are then transmitted to a PC or phone using the standard **Bluetooth HID** stack.

This is currently a **rough prototype rather than a finished product**, but the core concept works and the project definitely has the right to exist. Next up: finishing the complete `keyMap` for all special keys, adding a Li-Po/Li-Ion battery with a charging module, and fitting everything neatly back into the original enclosure for a fully autonomous wireless setup.




### Right now it looks terrible, but it works and definitely has a right to exist.

<img width="333" height="412" alt="image" src="https://github.com/user-attachments/assets/63d79b6a-dbb8-4103-af9a-bccce2768d6a" />
