#include <WiFi.h>
#include <PubSubClient.h>

// UART 핀 설정
#define RXD2 4
#define TXD2 17

// Wi-Fi 정보
const char* ssid = "KT_GiGA_2G_Wave2_CCB1";
const char* password = "ecxaheh406";

// MQTT 브로커 정보
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

// 수신 버퍼
String recvBuffer = "";

void setup() {
  Serial.begin(115200); // PC 디버깅용
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); 

  // Wi-Fi 연결
  Serial.print("Wi-Fi 연결 중");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n[✓] Wi-Fi 연결됨");

  // MQTT 연결
  client.setServer(mqtt_server, 1883);
  Serial.print("MQTT 연결 중");
  while (!client.connected()) {
    if (client.connect("esp32Client")) {
      Serial.println("\n[✓] MQTT 연결됨");
    } else {
      Serial.print(".");
      delay(500);
    }
  }
  Serial.println("UART2 수신 시작...");
}

unsigned long lastSent = 0;
unsigned long sendInterval = 200;  // 200ms마다 1번 전송

void loop() {
  client.loop();  // MQTT keep-alive

  // UART 데이터 수신 및 라인 단위 처리
  while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '\n') {
      recvBuffer.trim();  // \r이나 공백 제거
      if (recvBuffer.length() > 0) {
        if (millis() - lastSent >= sendInterval) {
          client.publish("sensor/data", recvBuffer.c_str());
          
          // Serial 확인용
          Serial.print("[✓] 전송: "); 
          Serial.println(recvBuffer);
          lastSent = millis();  // 시간 갱신
        }
      }
      recvBuffer = "";  // 버퍼 초기화
    } else {
      recvBuffer += c;
    }
  }
}
