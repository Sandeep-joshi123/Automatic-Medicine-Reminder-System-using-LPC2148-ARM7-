# Automatic-Medicine-Reminder-System-using-LPC2148-ARM7-
Automatic Medicine Reminder System using LPC2148 (ARM7)

Project Title

Automatic Medicine Reminder System using LPC2148 with RTC, LCD, Keypad,
UART, ADC, and Alarm Management

Project Overview

This project is an embedded healthcare application developed on the
LPC2148 ARM7 microcontroller. It reminds users to take medicines on time
using the internal RTC and provides a menu-driven interface through
keypad, LCD and UART.

Features

-   Set current time
-   Add/View/Delete alarms
-   RTC-based reminder system
-   LCD and UART display
-   ADC monitoring
-   Buzzer and LED alerts
-   Snooze/Stop alarm
-   7-segment keypad feedback

Hardware

LPC2148, 16x2 LCD, 4x4 Keypad, LEDs, Buzzer, 7-Segment Display, ADC
Voltage Divider, UART Terminal.

Software

Embedded C, Keil uVision, Proteus.

Working

Initialize peripherals -> Set current time -> Store alarms -> Compare
RTC with alarms -> Trigger buzzer/LED/LCD/UART alert -> Snooze/Stop ->
Continue monitoring.

LPC2148 Peripherals Used

RTC, UART0, ADC0, GPIO, I2C.

Future Scope

Battery backup, EEPROM storage, GSM/Bluetooth/Wi-Fi, mobile app
integration and cloud monitoring.

Author

Academic Embedded Systems Mini Project.
