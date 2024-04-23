# Airthings Wave Plus ESP32 Interface

This repository contains the source code for interfacing with an Airthings Wave Plus device using an ESP32 microcontroller over Bluetooth Low Energy (BLE).

## Overview

The Airthings Wave Plus is a smart indoor air quality monitor equipped with sensors for Radon, CO2, VOCs, Humidity, Temperature, and Air Pressure. This repository provides a way to read the Humidity, Temperature, and both Short Term and Long Term Radon levels wirelessly over BLE using an ESP32 microcontroller.

## Features

- Read Humidity, Temperature, and Radon levels (Short Term and Long Term) from Airthings Wave Plus over BLE
- Compatible with ESP32 microcontroller
- Provides real-time air quality data

## How it works

The ESP32 microcontroller connects to the Airthings Wave Plus device over BLE and reads the sensor data. The data can then be processed or sent to a server for further analysis and visualization.

## Requirements

- Airthings Wave Plus device
- ESP32 microcontroller
- PlatformIO IDE

## Setup

1. Clone this repository
2. Open the project in PlatformIO IDE
3. Upload the code to your ESP32 microcontroller using the "Upload" button in PlatformIO IDE
4. Ensure your Airthings Wave Plus device is in range of the ESP32

## Usage

Once the ESP32 is programmed and the Airthings Wave Plus device is in range, the ESP32 will start reading the sensor data and output it to the serial monitor. Here is a sample output:

```
Humidity: 28.00 %rH, Temperature: 23.43 *C, Radon STA: 24 Bq/m3, Radon LTA: 45 Bq/m3
```
