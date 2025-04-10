#include <SoftwareSerial.h>
SoftwareSerial gps(11,12); //(Tx, Rx)

String gps_line = "";

void setup() {
  Serial.begin(9600);
  gps.begin(9600);
  // put your setup code here, to run once:
              
}

void loop() {
  while (gps.available()) {
    char c = gps.read();

    if (c == '\n') {
      // 한 줄이 끝났을 때 출력
      Serial.print("GPS: ");
      Serial.print(gps_line);
      Serial.println();  // 줄바꿈

      gps_line = "";  // 버퍼 초기화
      delay(500);    // 한 줄 출력 후 3초 대기
    } else if (c != '\r') {
      gps_line += c;  // 줄바꿈 문자 제외하고 버퍼에 저장
    }
  }
}

/**
#include <SoftwareSerial.h>
SoftwareSerial gps(11,12); // (TX, RX)

String gps_line = "";

void setup() {
  Serial.begin(9600);
  gps.begin(9600);
}

float convertToDecimal(String nmea) {
  float raw = nmea.toFloat();
  int degrees = int(raw / 100);
  float minutes = raw - (degrees * 100);
  return degrees + (minutes / 60.0f);
}

void parseGNGGA(String sentence) {
  int commaCount = 0;
  String token = "";
  float latitude = 0.0;
  float longitude = 0.0;
  int fix = 0;

  for (int i = 0; i < sentence.length(); i++) {
    char c = sentence.charAt(i);
    if (c == ',') {
      commaCount++;

      if (commaCount == 3) {
        latitude = convertToDecimal(token);
        Serial.print("[LAT] ");
        Serial.println(latitude, 6);
      } else if (commaCount == 5) {
        longitude = convertToDecimal(token);
        Serial.print("[LON] ");
        Serial.println(longitude, 6);
      } else if (commaCount == 7) {
        fix = token.toInt();
        Serial.print("[FIX] ");
        Serial.println(fix);
      }

      token = "";
    } else {
      token += c;
    }
  }
}

void loop() {
  while (gps.available()) {
    char c = gps.read();

    if (c == '\n') {
      gps_line.trim();

      if (gps_line.startsWith("$GNGGA")) {
        Serial.print("GPS RAW: ");
        Serial.println(gps_line);
        parseGNGGA(gps_line);
      }

      gps_line = "";
      delay(500);  // 출력 간격 조절
    } else if (c != '\r') {
      gps_line += c;
    }
  }
}
 */
