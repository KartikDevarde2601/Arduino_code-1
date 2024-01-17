#include <SoftwareSerial.h>
#include <TimeLib.h>

SoftwareSerial myBluetooth(10, 11); // RX, TX


unsigned long lastMillis = 0;

float fValue = 0;
int sensor_one = 0;
int sensor_two = 0;
int datapoints = 0;

const String INVALIDACK = "404:InvalidCommand:\n";
const String ADCOMACK = "100:CommandReceived:\n";

struct DecodedData {
    String command;
    String payload;
};

void setup() {
  Serial.begin(9600);
  myBluetooth.begin(9600);

  // Add code to check Bluetooth connection
  // ...

  // Add any other setup code as needed
}


void handleCommand(String command, String payload) {
  if (command == "SET") {
    // Android Command for getting sensor data format: Frequncy, No. of datapoints, Sensor position1, Sensor position2
    fValue = payload.substring(0, payload.indexOf(',')).toFloat();
    payload = payload.substring(payload.indexOf(',') + 1);
    //datapoints=;
    sensor_one = payload.substring(0, payload.indexOf(',')).toInt();
    payload = payload.substring(payload.indexOf(',') + 1);
    sensor_two = payload.toInt();

    writeBluetoothData(ADCOMACK);
    sendSensorData(fValue, datapoints, sensor_one, sensor_two);

  } else {
    writeBluetoothData(INVALIDACK);
  }
}

void readBluetoothData() {
  while (myBluetooth.available()) {
    String receivedData = myBluetooth.readStringUntil('\n');

    DecodedData androidData = decodeReceivedMsg(receivedData);
    
    Serial.println("Command: " + androidData.command + ", Payload: " + androidData.payload);

    handleCommand(androidData.command, androidData.payload);
  }
}

void writeBluetoothData(String msg){
   myBluetooth.print(msg);
}

// send msg format:: MSGCODE:PAYLOAD:END
String encodeSendMsg(String msgCode, String msgPayload){
    String data = msgCode+":"+msgPayload+":"+"\n";
    return data;
}

// received msg format:: MSGCODE:PAYLOAD:END
DecodedData decodeReceivedMsg(String recmsg){
    String command = recmsg.substring(0, recmsg.indexOf(':'));
    String payload = recmsg.substring(recmsg.indexOf(':') + 1);
    Serial.print("Command: " + command + ", Payload: " + payload);
    
    return {command,payload};
}

void sendSensorData(float freq,int datapoints, int senspos1, int senspos2){
  // In fututre , we call AD5940 and send the above parameters to get sensor data 
  //********************* Instruct AD5940 to start sending data
  //********************* Instruct Ardunio to configure ELctrodes positions 
  int count = 0;
  do{
    String data = generateRandomObservation();
    count = count + 1;
    data = encodeSendMsg("1", data);
    writeBluetoothData(data);
  }while(count<datapoints);
  
  //******************** Interrupt AD5940
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
  delay(100);
}
