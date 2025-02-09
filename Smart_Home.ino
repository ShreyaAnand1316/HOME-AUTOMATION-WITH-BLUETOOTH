#include <LiquidCrystal.h>

// LCD pin connections: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// Relay pins
const int relayPins[] = {4, 5, 6, 7};

// Button pins
const int buttonPins[] = {A0, A1, A2, A3};

// Relay states
bool relayStates[] = {false, false, false, false};

// Timeout mechanism
unsigned long lastCommandTime = 0; // Tracks the last time a Bluetooth command was received
const unsigned long timeout = 10000; // 10 seconds timeout

void setup() {
  // Initialize relays as OUTPUT
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Relays OFF initially
  }

  // Initialize buttons as INPUT_PULLUP
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Initialize Serial communication
  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Bluetooth Home");
  lcd.setCursor(0, 1);
  lcd.print("Automation");
  delay(2000);
  lcd.clear();
  lcd.print("Waiting...");
}

void loop() {
  // Check for Bluetooth commands
  if (Serial.available() > 0) {
    char command = Serial.read(); // Read command from HC-05

    // Ensure the command is valid (A-D or heartbeat 'H')
    if ((command >= 'A' && command <= 'D') || command == 'H') {
      handleCommand(command);
      lastCommandTime = millis(); // Update last command time
    } else {
      lcd.clear();
      lcd.print("Invalid Command");
    }
  }

  // Handle timeout if no Bluetooth activity
  if (millis() - lastCommandTime > timeout) {
    lcd.clear();
    lcd.print("No Bluetooth!");
    lcd.setCursor(0, 1);
    lcd.print("Manual Mode");
    resetRelays(); // Optional: Reset relays to default state
  }

  // Check for manual button presses
  checkButtons();
}

// Function to handle Bluetooth commands
void handleCommand(char command) {
  if (command == 'H') {
    // Heartbeat signal, update status
    lcd.setCursor(0, 0);
    lcd.print("Bluetooth OK  ");
  } else {
    switch (command) {
      case 'A': setRelayState(0, true); break; // Turn ON device 1
      case 'B': setRelayState(0, false); break; // Turn OFF device 1
      case 'C': setRelayState(1, true); break; // Turn ON device 2
      case 'D': setRelayState(1, false); break; // Turn OFF device 2
      default:
        lcd.clear();
        lcd.print("Invalid Command");
        break;
    }
  }
}

// Function to toggle relay state manually
void checkButtons() {
  for (int i = 0; i < 4; i++) {
    if (digitalRead(buttonPins[i]) == LOW) { // Button pressed
      delay(200); // Debounce delay
      relayStates[i] = !relayStates[i]; // Toggle relay state
      digitalWrite(relayPins[i], relayStates[i] ? LOW : HIGH); // Update relay
      updateLCD(i); // Update LCD
    }
  }
}

// Function to set relay state and update LCD
void setRelayState(int relay, bool state) {
  relayStates[relay] = state;
  digitalWrite(relayPins[relay], state ? LOW : HIGH); // Activate/Deactivate relay
  updateLCD(relay); // Update LCD
}

// Function to update LCD
void updateLCD(int relay) {
  lcd.setCursor(0, relay);
  lcd.print("Device ");
  lcd.print(relay + 1);
  lcd.print(": ");
  lcd.print(relayStates[relay] ? "ON " : "OFF");
}

// Function to reset all relays to their default state
void resetRelays() {
  for (int i = 0; i < 4; i++) {
    relayStates[i] = false;
    digitalWrite(relayPins[i], HIGH); // Turn off all relays
  }
}

