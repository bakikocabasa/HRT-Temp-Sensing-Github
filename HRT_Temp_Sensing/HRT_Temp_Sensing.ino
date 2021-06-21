//In this PCB design and code, 60 temperature sensor wanted to be sensed. On PCB design, arduino nano is used,
//temp sensors are 13k NTC types, as a voltage divider 4.7 kohm resistors are choosen. HP4087 16 channel mux is used to digital to analog converter
//because otherwise there would be not enough input numbers on the central controller (Arduino nano. We have can bus shield to communicate with the rest of the subsystems.
//we have 4 mux, each of the mux has connected with 15 temp sensors however mux has 16 channels. That is why we do not count the loop for 16 but 15.
//

#include <mcp_can.h>
#include <SPI.h>
MCP_CAN CAN0(10);     // Set CS to pin 10  these 3 rows are for the CAN BUS communication

int pin_Out_S0 = 2; // these are for the mux identification. All of these 4 muxes are connected together to the these digital outputs to have a identification.
int pin_Out_S1 = 3;
int pin_Out_S2 = 4;
int pin_Out_S3 = 5;
int overtemp = 8;
int noinfo = 9;
int pin_In_Mux1 = A0;
int pin_In_Mux2 = A1;
int pin_In_Mux3 = A2;
int pin_In_Mux4 = A3;
int a[60];

int Mux1_State[15] = {0};
int Mux2_State[15] = {0};
int Mux3_State[15] = {0};
int Mux4_State[15] = {0};
float logR2_1[15], logR2_2[15], logR2_3[15], logR2_4[15], R2_1[15], R2_2[15], R2_3[15], R2_4[15], T_1[15], T_2[15], T_3[15], T_4[15];
float R1 = 4700; //resistance of known resistor which is connected in series with thermistor
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
int sum = 0;
byte data[8];
int sequence22[60];

void setup() {
  for (int i = 0; i < 60; i ++) {
    sequence22[i] = i;
  }

  Serial.begin(9600);

  // Initialize MCP2515 running at 8MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted


  byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};


  pinMode(pin_Out_S0, OUTPUT);
  pinMode(pin_Out_S1, OUTPUT);
  pinMode(pin_Out_S2, OUTPUT);
  pinMode(pin_Out_S3, OUTPUT);
  pinMode(pin_In_Mux1, INPUT);
  pinMode(pin_In_Mux2, INPUT);
  pinMode(pin_In_Mux3, INPUT);
  pinMode(pin_In_Mux4, INPUT);

  pinMode(overtemp, OUTPUT);
  pinMode(noinfo, OUTPUT);

}

void loop() {
  updateMux1();

  for (int i = 0; i < 15; i ++) {
    if (i == 14) {
      Serial.println(T_1[i]);
      Serial.println("Mux1");
      Serial.println("end");
    } else {

      Serial.print(T_1[i]);
      Serial.print(",");

    }
  }
  for (int i = 0; i < 15; i ++) {
    if (i == 14) {
      Serial.println(T_2[i]);
      Serial.println("Mux2");
      Serial.println("end");
    }
    else {
      Serial.print(T_2[i]);
      Serial.print(",");

    }
  }
  for (int i = 0; i < 15; i ++) {
    if (i == 14) {
      Serial.println(T_3[i]);
      Serial.println("Mux3");
      Serial.println("end");
    }
    else {
      Serial.print(T_3[i]);
      Serial.print(",");
    }
  }
  for (int i = 0; i < 15; i ++) {
    if (i == 14) {
      Serial.println(T_4[i]);
      Serial.println("Mux4");
      Serial.println("end");

    }
    else {
      Serial.print(T_4[i]);
      Serial.print(",");

    }
  }
  //          for(int i = 0; i < 60; i ++) {
  //          if(i == 59){
  //     Serial.println(a[i]);
  //     Serial.println("a");
  //     Serial.println("end");
  //
  //    }
  //    else {
  //      Serial.print(a[i]);
  //      Serial.print(",");
  //
  //      }
  //    }

  // temp check and output signal for latching
  for (int i = 0; i < 15; i ++) {
    if (T_1[i] >= 58 || T_2[i] >= 58 || T_3[i] >= 58 || T_4[i] >= 58) { // ortalama 60 dereceye cikarsa low sinyali ver
      digitalWrite(overtemp, LOW);

    }
    else {
      digitalWrite(overtemp, HIGH);
    }
    // when we have got no signal from the sensor or mux is broken. we have a value near to the 0 celcious
    //        if(T_1[i]<? || T_2[i]>=? || T_3[i]>=? || T_4[i]>=?) { // ortalama 0 dereceyse yani sensorlerden bilgi gelmiyorsa ya da mux bozulduysa
    //      digitalWrite(noinfo,LOW);
    //
    //    }
    //      else {
    //      digitalWrite(noinfo,HIGH);
    //    }
  }

  //for(int i = 0; i < 60; i ++) {
  //  // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
  //
  //    if (i<8) {
  //    int sndStat1 = CAN0.sendMsgBuf(0x00, 0, 8, sequence);
  //    delay(100); }
  //    if (i>=8 && i<16) {
  //    int sndStat2 = CAN0.sendMsgBuf(0x01, 0, 8, sequence);
  //    delay(100); }
  //    if (i>=8 && i<16) {
  //    int sndStat3 = CAN0.sendMsgBuf(0x02, 0, 8, sequence);
  //    delay(100); }
  //    if (i>=16 && i<24) {
  //    int sndStat4 = CAN0.sendMsgBuf(0x03, 0, 8, sequence);
  //    delay(100); }
  //    if (i>=24 && i<32) {
  //    int sndStat5 = CAN0.sendMsgBuf(0x04, 0, 8, sequence);
  //    delay(100); }
  //    if (i>=32 && i<40) {
  //    int sndStat6 = CAN0.sendMsgBuf(0x05, 0, 8, sequence);
  //    delay(100); }
  //    if (i>=40 && i<48) {
  //    int sndStat7 = CAN0.sendMsgBuf(0x06, 0, 8, sequence);
  //    delay(100); }
  //    if (i>=48 && i<56) {
  //    int sndStat8 = CAN0.sendMsgBuf(0x07, 0, 8, sequence);
  //    delay(100); }
  //    if (i>=56 && i<60) {
  //    int sndStat10 = CAN0.sendMsgBuf(0x08, 0, 8, sequence);
  //    delay(100); }
  //}



  for (int i = 0; i < 8; i ++) {

    data[i] = sequence22[i];
  }
  int sndStat1 = CAN0.sendMsgBuf(0x1, 0, 8, data);


  delay(100);
  for (int i = 8; i < 16; i ++) {
    data[i - 8] = sequence22[i];
  }
  int sndStat2 = CAN0.sendMsgBuf(0x01, 0, 8, data);


  for (int i = 16; i < 24; i ++) {

    data[i - 16] = sequence22[i];
  }
  int sndStat3 = CAN0.sendMsgBuf(0x02, 0, 8, data);


  delay(100);
  for (int i = 24; i < 32; i ++) {
    data[i - 24] = sequence22[i];
  }
  int sndStat4 = CAN0.sendMsgBuf(0x03, 0, 8, data);


  for (int i = 32; i < 40; i ++) {

    data[i - 32] = sequence22[i];
  }
  int sndStat5 = CAN0.sendMsgBuf(0x04, 0, 8, data);


  delay(100);
  for (int i = 40; i < 48; i ++) {
    data[i - 40] = sequence22[i];
  }
  int sndStat6 = CAN0.sendMsgBuf(0x05, 0, 8, data);


  for (int i = 48; i < 56; i ++) {

    data[i - 48] = sequence22[i];
  }
  int sndStat7 = CAN0.sendMsgBuf(0x06, 0, 8, data);


  delay(100);
  for (int i = 56; i < 60; i ++) {
    data[i - 56] = sequence22[i];
  }
  int sndStat8 = CAN0.sendMsgBuf(0x07, 0, 8, data);



  //  int sndStat1 = CAN0.sendMsgBuf(0x01, 0, 8, a[i]);
  //  int sndStat2 = CAN0.sendMsgBuf(0x02, 0, 8, b[i]);
  //  int sndStat3 = CAN0.sendMsgBuf(0x03, 0, 8, c[i]);
  //  int sndStat4 = CAN0.sendMsgBuf(0x04, 0, 8, d[i]);

  //byte sndStat2 = CAN0.sendMsgBuf(0x02, 0, 8, T_2[i]);
  //byte sndStat3 = CAN0.sendMsgBuf(0x03, 0, 8, T_3[i]);
  //byte sndStat4 = CAN0.sendMsgBuf(0x04, 0, 8, T_4[i]);
  //if(sndStat1 == CAN_OK && sndStat2 == CAN_OK && sndStat3 == CAN_OK && sndStat4 == CAN_OK){ // if all the information sent at the same time without problem, it gives us a message
  // if(sndStat1 == CAN_OK){ // if all the information sent at the same time without problem, it gives us a message

  //  Serial.println("Message Sent Successfully!");
  //} else {
  //  Serial.println("Error Sending Message...");
  // }
  delay(100);   // send data per 100ms

}



void updateMux1 () {
  delay(1000);
  for (int i = 0; i < 15; i++) { // 15 because of 60 sensors divided by 4 = 15 for each mux. It starts from the 0 till 14 so 15 values overall.
    digitalWrite(pin_Out_S0, HIGH && (i & B00000001));
    digitalWrite(pin_Out_S1, HIGH && (i & B00000010));
    digitalWrite(pin_Out_S2, HIGH && (i & B00000100));
    digitalWrite(pin_Out_S3, HIGH && (i & B00001000)); // normally if you want to use the 16 channel, this comment must be used. Otherwise, as given like this
    //digitalWrite(pin_Out_S3, HIGH && (i & B00000111)); ???
    Mux1_State[i] = analogRead(pin_In_Mux1);
    Mux2_State[i] = analogRead(pin_In_Mux2);
    Mux3_State[i] = analogRead(pin_In_Mux3);
    Mux4_State[i] = analogRead(pin_In_Mux4);
    R2_1[i] = R1 * (1023.0 / Mux1_State[i] - 1.0);
    R2_2[i] = R1 * (1023.0 / Mux2_State[i] - 1.0);
    R2_3[i] = R1 * (1023.0 / Mux3_State[i] - 1.0);
    R2_4[i] = R1 * (1023.0 / Mux4_State[i] - 1.0);
    logR2_1[i] = log(R2_1[i]);
    logR2_2[i] = log(R2_2[i]);
    logR2_3[i] = log(R2_3[i]);
    logR2_4[i] = log(R2_4[i]);
    T_1[i] = (1.0 / (c1 + c2 * logR2_1[i] + c3 * logR2_1[i] * logR2_1[i] * logR2_1[i]));
    T_1[i] = T_1[i] - 273.15;

    T_2[i] = (1.0 / (c1 + c2 * logR2_2[i] + c3 * logR2_2[i] * logR2_2[i] * logR2_2[i]));
    T_2[i] = T_2[i] - 273.15;

    T_3[i] = (1.0 / (c1 + c2 * logR2_3[i] + c3 * logR2_3[i] * logR2_3[i] * logR2_3[i]));
    T_3[i] = T_3[i] - 273.15;

    T_4[i] = (1.0 / (c1 + c2 * logR2_4[i] + c3 * logR2_4[i] * logR2_4[i] * logR2_4[i]));
    T_4[i] = T_4[i] - 273.15;


    a[i] = T_1[i];
    a[i + 15] = T_2[i];
    a[i + 30] = T_3[i];
    a[i + 45] = T_4[i];

  }

}
