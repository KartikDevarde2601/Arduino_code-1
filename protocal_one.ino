#include <SoftwareSerial.h>
#include <TimeLib.h>

SoftwareSerial myBluetooth(10, 11); // RX, TX


unsigned long lastMillis = 0;

float fValue = 0;
int sensor_one = 0;
int sensor_two = 0;
bool collectData = false;

const String ACK = "ACK:"; // Assuming ACK is a string command

void setup() {
  Serial.begin(9600);
  myBluetooth.begin(9600);

  // Add code to check Bluetooth connection
  // ...

  // Add any other setup code as needed
}

void handleCommand(String command, String payload) {
  if (command == "SET") {
    fValue = payload.substring(0, payload.indexOf(',')).toFloat();
    payload = payload.substring(payload.indexOf(',') + 1);

    sensor_one = payload.substring(0, payload.indexOf(',')).toInt();

    payload = payload.substring(payload.indexOf(',') + 1);
    sensor_two = payload.toInt();

  

    String Ack = ACK + fValue + ',' + sensor_one + ',' + sensor_two + '\n';

    myBluetooth.println(Ack);// send Ack to the app recived values

  } else if (command == "START") {
    collectData = true;
  } else if (command == "BREAK") {
    collectData = false;
    String BreakAck = "BREAK_RECIVED";
    myBluetooth.println(BreakAck);    // send ack to the app for break command
  } else {
    myBluetooth.print("Invalid command");
  }
}

void readBluetoothData() {
  while (myBluetooth.available()) {
    String receivedData = myBluetooth.readStringUntil('\n');

    String command = receivedData.substring(0, receivedData.indexOf(':'));
    receivedData = receivedData.substring(receivedData.indexOf(':') + 1);

    String payload = receivedData;
    Serial.print("Command: " + command + ", Payload: " + payload);

    handleCommand(command, payload);
  }
}

String generateRandomObservation() {
  String observationString = "";

  // Get current time
  tmElements_t tm;

  observationString += "Time:" + String(tm.Hour) + ":" + String(tm.Minute) + ":" + String(tm.Second) + "." + String(millis() % 1000) + ",";
  observationString += "Freq:" + String(fValue) + ",";
  observationString += "RzMag:" + String(random(1, 99)) + "Ohm" + ",";
  observationString += "Rzphase:" + String(random(-0.00393, 0.9999)) + ",";


  return observationString;
}

void loop() {
  readBluetoothData();

  if (collectData) {
    String observationData = generateRandomObservation();
    delay(40);
    myBluetooth.println(observationData);
  }
}
