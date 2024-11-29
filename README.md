# unsTABLE Pico Project

This project powers the **unsTABLE Pico**, a device that integrates an OLED display, buzzer, button, and MQTT functionality to create a dynamic interactive experience. The device connects to an MQTT broker and responds to JSON-based commands to display messages, manage notifications, and handle user interactions.

## Features

- **Wi-Fi Connectivity**:
  - Automatically connects to a specified Wi-Fi network.

- **MQTT Integration**:
  - Subscribes to a specific topic and processes JSON messages with the following actions:
    - **Action 0**: Display a welcome message (`"Hello, {name}"`) and start a timer.
    - **Action 1**: Display an alert on the screen for 1 minute without interrupting the timer.
    - **Action 2**: Display a goodbye message and clear the screen.
    - **Action 3**: Enable or disable notification sounds.

- **Display Features**:
  - Displays messages, QR codes, and timer.
  - Typing and scrolling animations for a smooth user experience.

- **Buzzer**:
  - Plays notification sounds with the ability to toggle sound on/off.

- **Button Interaction**:
  - Single-click: Display a QR code and clear after 5 seconds.
  - Double-click: Clear the display immediately.

## Requirements

### Hardware
- Raspberry Pi Pico or compatible board.
- OLED Display (SSD1306).
- Button and buzzer.

### Libraries
Install the following libraries via the Arduino Library Manager:
- **Adafruit GFX Library**
- **Adafruit SSD1306**
- **PubSubClient**
- **ArduinoJson**

## Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/your-repo/unsTABLE-Pico.git```
2. Open the project in the Arduino IDE.

3. Configure the Wi-Fi and MQTT settings in config.h from config.h.example:

4. Compile and upload the code to your Raspberry Pi Pico.

5. Use an MQTT client (e.g., MQTT Explorer) to send messages to the subscribed topic.