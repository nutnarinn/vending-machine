# Vending Machine Project

This project involves the development of a vending machine system using an Arduino for hardware control and an Odroid for communication and data logging. The system features sales tracking, inventory management, and real-time notifications using the Line Notify API and ThingSpeak for marketing analysis.

## Components

1. **Arduino Uno**: Controls the vending machine operations, including snack selection, payment processing, and inventory management.
2. **Odroid**: Manages serial communication with the Arduino and handles notifications and data logging to external services.

## Features

- **Real-Time Notifications**: Sends notifications to a Line group for real-time updates on sales and stock status.
- **Data Logging**: Logs sales data to ThingSpeak for marketing analysis and real-time monitoring.
- **User Interface**: Uses an LCD display and buttons for user interaction.
- **RFID Card Reader**: Processes transactions using RFID cards.
- **Restocking**: Allows for easy restocking of products via a dedicated button.
- **Music and LED Indicators**: Plays a melody and controls LED indicators for user feedback during transactions.

## Hardware Setup

1. **Arduino Uno**
   ![IMG_0228](https://github.com/nutnarinn/vending-machine/assets/100826301/c056e9b1-7500-4253-b9ae-a7bc31b53dcd)
   
   *Figure 1: Schematic diagram of the hardware setup.*
   - LCD Display
   - RFID Card Reader (MFRC522)
   - Buzzer
   - LED Indicators
   - Buttons (for selecting snacks and restocking)
   - Servo Motors (for dispensing snacks)

3. **Odroid**
   - Serial communication with Arduino
   - Internet connectivity for Line Notify and ThingSpeak

## Arduino Code Overview

The Arduino code controls the vending machine's hardware operations. It manages the user interface, processes RFID card transactions, controls the servo motors for dispensing snacks, and communicates with the Odroid.

### Libraries Required

- `Wire.h`
- `LiquidCrystal_I2C.h`
- `SPI.h`
- `MFRC522.h`
- `pitches.h`
- `TM1637Display.h`
- `Adafruit_PWMServoDriver.h`
- `SoftwareSerial.h`

### Key Functions

- **Initial Setup**: Initializes the components and sets up the display.
- **Button Handling**: Detects button presses for snack selection and restocking.
- **RFID Reading**: Reads and processes RFID card data for transactions.
- **Snack Dispensing**: Controls the servo motors to dispense the selected snack.
- **Restocking**: Resets the stock count for each product.
- **Income Calculation**: Tracks and calculates the total income from sales.

## Odroid Code Overview

The Odroid code manages communication with the Arduino, sends notifications, and logs data to ThingSpeak.

### Libraries Required

- `serial`
- `requests`
- `urllib`

### Key Functions

- **Serial Communication**: Reads data from the Arduino via serial communication.
- **Line Notify Integration**: Sends real-time notifications using the Line Notify API.
- **ThingSpeak Integration**: Logs sales data to ThingSpeak for analysis.

## Getting Started

### Prerequisites

- Arduino Uno
- Odroid
- RFID Card Reader (MFRC522)
- LCD Display
- Servo Motors
- Buzzer and LED Indicators
- Push Buttons
- Internet Connection

### Installation

1. **Arduino Setup**
   - Connect the LCD, RFID reader, buttons, servo motors, buzzer, and LED indicators to the Arduino.
   - Upload the Arduino code to the Arduino Uno.

2. **Odroid Setup**
   - Install necessary libraries (`serial`, `requests`, `urllib`).
   - Configure the Odroid to communicate with the Arduino via serial.
   - Set up Line Notify and ThingSpeak accounts and obtain API keys.

3. **Configuration**
   - Update the Odroid code with the Line Notify token and ThingSpeak API key.
   - Adjust serial port settings if necessary.

### Running the Project

1. Power on the Arduino and Odroid.
2. The Arduino will handle user interactions and snack dispensing.
3. The Odroid will manage notifications and data logging.

## Usage

- **Selecting a Snack**: Press the corresponding button for the desired snack. The price and availability will be displayed on the LCD.
- **Processing a Transaction**: Present an RFID card to the reader. The system will process the transaction, dispense the snack, and update the inventory.
- **Restocking**: Press the restock button to reset the stock counts.

## Results

### Line Notify

The system sends real-time notifications to a designated Line group whenever a snack is sold or restocked. Notifications include snack remaining, total income, and restock completion status.

![Screenshot 2024-07-08 135119](https://github.com/nutnarinn/vending-machine/assets/100826301/5d0bf4d5-2c18-4ef3-8f3f-3519ca8b4031)

*Figure 2: Example of a notification sent to Line when a snack is sold. It includes snack remaining and total income, and when restocked, it indicates restock completion.*

### ThingSpeak Data Logging

Sales data is logged to ThingSpeak, allowing for real-time monitoring and analysis. The ThingSpeak dashboard shows a graph of the total income in real-time.

![Screenshot 2024-07-08 135218](https://github.com/nutnarinn/vending-machine/assets/100826301/faf1631f-11b5-4902-a629-8153cabb7ff5)

*Figure 3: ThingSpeak dashboard showing the graph of total income in real-time.*
