# AUTOMATIC MEDICINE REMINDER SYSTEM USING LPC2148 (ARM7)

## PROJECT OVERVIEW

The Automatic Medicine Reminder System is an embedded healthcare application developed using the LPC2148 ARM7 microcontroller. The system assists users in taking medicines on time by maintaining accurate real-time using the built-in RTC (Real-Time Clock) and generating reminders through LCD, UART, LEDs, and buzzer notifications.

The project provides a simple menu-driven interface using a 4x4 keypad, allowing users to set the current time, manage medicine alarms, and monitor system status. It demonstrates the integration of multiple LPC2148 peripherals in a practical real-time embedded application.

## OBJECTIVES

* Develop a real-time medicine reminder system.
* Maintain accurate time using the LPC2148 RTC.
* Provide a user-friendly keypad and LCD interface.
* Generate audio and visual alerts for medicine schedules.
* Demonstrate the integration of RTC, UART, ADC, GPIO, and I2C peripherals.

## FEATURES

* Real-Time Clock (RTC) based scheduling
* Set current time using keypad
* Add multiple medicine alarms
* View stored alarms
* Delete alarms
* View current time
* ADC-based voltage/temperature monitoring
* LCD display interface
* UART terminal communication
* LED and buzzer alarm indication
* Snooze and Stop alarm functionality
* 7-Segment display for keypad input

## HARDWARE COMPONENTS

* LPC2148 ARM7 Development Board
* 16x2 LCD Display
* 4x4 Matrix Keypad
* LEDs
* Buzzer
* 7-Segment Display
* ADC Voltage Divider Circuit
* UART Virtual Terminal
* Power Supply

## SOFTWARE TOOLS

* Embedded C
* Keil µVision IDE
* Proteus Design Suite

## LPC2148 PERIPHERALS USED

RTC    : Real-Time Clock for accurate time keeping

UART0  : Serial communication and debugging

GPIO   : Keypad, LEDs, Buzzer, and 7-Segment control

ADC0   : Analog voltage monitoring

I2C    : LCD communication

## WORKING PRINCIPLE

1. Initialize all peripherals.
2. User sets the current time through the keypad.
3. RTC starts maintaining accurate real-time.
4. User can add, view, or delete medicine alarms.
5. Alarm information is stored in an array of structures.
6. The controller continuously compares RTC time with stored alarm timings.
7. When an alarm time matches:

   * LCD displays reminder.
   * UART displays reminder.
   * LEDs and buzzer are activated.
8. User can Snooze or Stop the alarm.
9. System returns to the main menu and continues monitoring.

## MENU OPTIONS

1. View Alarm

2. Add Alarm

3. Delete Alarm

4. View Current Time

5. View Temperature / ADC

6. Home

## PROJECT HIGHLIGHTS

* Developed using LPC2148 ARM7 Microcontroller
* RTC-based real-time medicine reminder
* Multiple alarm management
* Menu-driven embedded application
* UART and LCD dual display interface
* ADC integration for analog monitoring
* Implemented using Embedded C
* Simulated and tested using Proteus
* Compiled using Keil µVision IDE


## FUTURE ENHANCEMENTS

* Battery backup for RTC
* EEPROM/Flash alarm storage
* GSM/SMS reminder notifications
* Bluetooth/Wi-Fi connectivity
* Mobile application integration
* Cloud-based medicine monitoring
* Medicine name and dosage management

## LEARNING OUTCOMES

This project demonstrates practical implementation of:

* Embedded C Programming
* ARM7 LPC2148 Programming
* RTC Configuration
* ADC Interfacing
* UART Communication
* I2C Communication
* Matrix Keypad Scanning
* LCD Interfacing
* Alarm Scheduling Logic
* Embedded System Debugging


## Project Specifications

| Parameter | Details |
|-----------|---------|
| Project Type | Academic Mini Project |
| Platform | LPC2148 ARM7 Microcontroller |
| Language | Embedded C |
| IDE | Keil µVision |
| Simulation | Proteus Design Suite |

