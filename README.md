Solenoid Door Access Control PCB Design Using Altium Designer
Overview

This project is a password-based door access control system designed on a custom PCB using Altium Designer. The system uses an Arduino Nano, 0.96-inch OLED display, 4×4 matrix keypad, 5V relay module, and a piezo buzzer to control a solenoid door lock.
<img width="892" height="1588" alt="WhatsApp Image 2026-06-14 at 2 05 59 PM" src="https://github.com/user-attachments/assets/965f22db-3393-428d-a71b-23027488dbac" />

The PCB dimensions are approximately 100 mm × 50 mm, providing a compact solution for secure electronic door access.

Design Process

The design began with creating the schematic in Altium Designer. Individual sections such as the Arduino Nano, OLED interface, keypad connector, relay driver, and buzzer circuit were connected and verified using Electrical Rule Checks (ERC).

After schematic completion, the design was transferred to the PCB editor where components were arranged to minimize routing complexity. The Arduino Nano was placed at the center of the board, the OLED display at the top, the relay module at the bottom, and the keypad connector near the board edge for easy access.

PCB Features
Arduino Nano based controller
0.96-inch I²C OLED display
4×4 keypad interface
5V relay output for solenoid lock control
Piezo buzzer for audio indication
Single-board integrated design
Compact and easy-to-manufacture layout
Working Principle

When a user enters a password through the keypad, the Arduino Nano verifies the entered code. If the password is correct, the relay is activated for a predefined duration (typically 3–5 seconds) to unlock the solenoid door lock. The OLED displays an "Access Granted" message and the buzzer provides an audible confirmation. For an incorrect password, the display shows "Access Denied" and the buzzer generates an alert tone.

PCB Design Considerations

During layout design:

Signal traces were routed with a width of 10 mil.
Power traces were increased to 20–30 mil for better current handling.
Ground connections were kept short to reduce noise.
Adequate clearance was maintained between relay and logic circuits.
Design Rule Check (DRC) was performed before Gerber generation.
Conclusion

This project demonstrates the complete design flow of a practical embedded system PCB using Altium Designer. The final board integrates all major components required for a keypad-based door access system while maintaining a compact footprint and reliable operation. The design can be further extended with RFID, fingerprint, or wireless authentication features for enhanced security.
