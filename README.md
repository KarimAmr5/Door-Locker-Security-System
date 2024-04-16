
# Door-Locker-Security-System

> [!NOTE]
>This project was made during the Full Embedded Systems Diploma under the supervision of Engineer Mohamed Tarek.

The Door Lokcer Secuirty System aims to implement a robust door locking mechanism linked with a password set by the use . It uses dual ATMEGA32 microcontroller based on a layered architecture and the usage of several controller peripherals .


## Table of contents 

- [Overview](#Overview)
- [Requirements](#Requirements)
- [Components](#Components)
- [Circuit](#Circuit)
- [How to run ](#How-to-run)


## Overview

The system should contain two ECU's for the following purposes :

1. **HMI_ECU (Human Machine Interface)** :
- Take actions and input from user , send the required commands to the other ECU & display feedback on screen 
- Connected to a (2x16) LCD screen for messages display & 4x4 keypad for data input from user 

2. **Control_ECU** :
- Sets the required password inside EEPROM , checks and validates password entries , controld DC_motor & controls buzzer for alarm
- Connected to a DC_Motor , EEPROM & Buzzer 

## Requirements

Follow the fully detailed project requiremnts in the following file : [Project requirements file](https://github.com/KarimAmr5/Door-Locker-Security-System/blob/main/Final_Project.pdf)

## Required Drivers :
- GPIO driver 
- Timer driver (Timer1 w/ call-back function)
- UART driver
- LCD driver 
- Keypad driver (4x4 keypad)
- Buzzer driver
- EEPROM driver ( External EEPROM via I2C )
- I2C driver ( also called TWI )
- DC_Motor driver

## Layered Architecture

### HMI_ECU 

![image](![HMI](https://github.com/KarimAmr5/Door-Locker-Security-System/assets/93788982/885efd25-ae25-4472-9914-425f85f65e1e))

### Control_ECU

![image](![Control](https://github.com/KarimAmr5/Door-Locker-Security-System/assets/93788982/52f68b11-4ea3-4a70-9493-1faf5c80f0e3))

## Components

- Two ATmega32 Microcontrollers running @ 8 MHz
- HMI_ECU: 2x16 LCD, 4x4 keypad.
- Control_ECU: EEPROM, Buzzer, DC-Motor.

## Circuit 

![circuit](![Circuit](https://github.com/KarimAmr5/Door-Locker-Security-System/assets/93788982/edecf699-71d2-41f2-9524-23a9d26795c8))

## How to run

> The purpose of this project is **simulate** the the run on Proteus simulation software , but it could still run on real AtMega32 Microcontrollers with small changes to the code , for instace the frequenct (set to 16 Mhz) , etc.

**Required software for simulation run :** 
- Install Proteus 8 professional v10.0 or higher
- Install required Arduino & AVR libraries (Arduino_Proteus_Libraries)
- Run the .pdsprj file !

**Application procedure**

- Enter 5 digit password
- Re-enter the same password 
- Choose whether to Unlock the door OR change the password 
