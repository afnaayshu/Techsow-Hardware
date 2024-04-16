# TechSow - Integrated Crop and Soil Monitoring System

## NPK Sensor with Arduino and ESP32

This project is designed to measure the nitrogen (N), phosphorus (P), and potassium (K) levels in soil using an Arduino board and an ESP32 microcontroller. The Arduino reads sensor values and transmits the NPK data to the ESP32 over serial communication. The ESP32 then sends the data to a Firebase Realtime Database for storage and monitoring.

## Hardware Requirements

- Arduino Uno or compatible board
- ESP32 board (e.g., ESP32 DevKit)
- LDR (Light Dependent Resistor) sensor
- Jumper wires
- Breadboard (optional)

## Software Requirements

- Arduino IDE
- ESP32 board support package for Arduino IDE
- Firebase Arduino library
- WiFi network credentials

## Setup

1. Install the Arduino IDE and the ESP32 board support package.
2. Install the Firebase Arduino library by following the instructions in the [Firebase Arduino library documentation](https://github.com/mobizt/Firebase-ESP32).
3. Connect the Arduino and ESP32 boards using the RX, TX, and GND pins.
4. Connect the LDR sensor to the Arduino according to your circuit design.
5. Update the Wi-Fi SSID, password, Firebase API key, and database URL in the ESP32 code.

## Usage

1. Upload the Arduino code to the Arduino board.
2. Upload the ESP32 code to the ESP32 board.
3. The ESP32 will connect to your Wi-Fi network and Firebase.
4. The ESP32 will send a start command to the Arduino to trigger the sensor reading.
5. The Arduino will read the sensor values with different LED colors (red, blue, and green) and transmit the NPK data to the ESP32.
6. The ESP32 will parse the received data and send the NPK values to the Firebase Realtime Database every 5 seconds.
7. You can monitor the NPK data in the Firebase console or integrate it with other applications or services.

## Customization

- Modify the pin assignments and sensor configuration in the Arduino code according to your hardware setup.
- Adjust the number of readings and the mapping function in the Arduino code to calibrate the sensor values to the NPK levels.
- Modify the Firebase connection settings and database paths in the ESP32 code as needed.

## Contributing

Contributions to this project are welcome! If you find any issues or have suggestions for improvements, please open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
