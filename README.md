# Rasberry-Pi-Pico Playfield
# Raspberry Pi Pico LCD Timer Display

## Overview
This is a work-in-progress C project for the Raspberry Pi Pico that displays the **current time and date** on a **Waveshare 1.14" LCD module (ST7789, V2)**.  
It supports two display modes:
- **7-segment style digits** (large HH:MM clock)
- **Text mode** (standard font rendering with date + time)

The display updates automatically once per minute and can also be synchronized via serial input from a host computer.

---

## Features
- Written in **C** using the [Pico SDK](https://github.com/raspberrypi/pico-sdk).
- Renders to **Waveshare 1.14" LCD V2** using the official driver.
- Two rendering modes:
  - **7-seg mode**: custom-drawn segments for digital clock look.
  - **Text mode**: standard fonts for time and date.
- Toggle between display modes using the **onboard joystick/button**.
- Keeps track of time using the Pico’s **built-in RTC**.
- Time synchronization from host PC:
  ```bash
  date '+%Y-%m-%d %H:%M:%S' > /dev/ttyS2
