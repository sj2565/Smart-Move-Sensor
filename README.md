# 이동형 환경 및 충격 감지 시스템

## 1. 프로젝트 소개
내용쓰기

## 2. 프로젝트 이미지
<img width="450" alt="작품사진" src="https://github.com/user-attachments/assets/cb320aaf-70e7-4cd5-95c1-a1dc190236f9">

## 3. 프로젝트 회로도
<img width="850" alt="회로도" src="https://github.com/user-attachments/assets/3ae1a685-1812-4e40-8ec8-9c6f7cd46ecb" />

## 4. 활용된 기술

### ⚙ 임베디드 시스템
- **STM32 Black Pill (STM32F411CEU6)** (센서 수집 및 실시간 제어 (FreeRTOS 기반))
- **MPU6050** (인터럽트 및 I2C 기반 6축 자이로/가속도 센서 (충격 감지 및 이동 평균 필터 적용))
- **DHT22** (GPIO 기반 온습도 센서 (Bit-banging 방식 + DWT 딜레이 사용))
- **MQ135** (ADC 기반 공기질 센서 (0~1023 → PPM 계산))
- **NEO-6M GPS** (UART 기반 위치 수신, GNGGA 파싱, Fix 상태 판단)
- **RGB LED**  (PWM 기반 색상 제어 (공기질에 따른 색상 반응))
- **Push Button** (인터럽트 기반 RGB 센서 on/off 제어)

### 🧩 RTOS & 시스템 제어
- **FreeRTOS** (CMSIS-RTOS v1 기반 Task/Semaphore/Queue구조)
- **Task 구성** (SensorTask, GpsTask, CommTask, ButtonTask)
- **Semaphore** (버튼 이벤트 제어 (디바운싱 처리 포함))
- **Queue** (구조체 기반 센서 데이터 전달 (Task 간 통신))
  
### 📡 통신
- **UART** (STM32 ↔ GPS (UART1), STM32 ↔ ESP32 (UART6, JSON 전송))
- **I2C** (MPU6050 제어 및 충격 감지 인터럽트 설정)
- **Wi-Fi** (MQTT 데이터 전송)

### 🐞 디버깅
- **CP2102** (__write 오버라이드 UART 디버깅 (STM32 ↔ PC Serial Monitor))
- **TeraTerm** (각종 센서 데이터 수신 및 JSON 전송 로그 확인)

### 💻 네트워크 / 시각화
- **ESP32-WROOM-32** (Wi-Fi 기반 MQTT 전송)
- **MQTT(Mosquitto)** (JSON 포맷 실시간 센서 전송)
- **Google Maps API(HTML/JS 기반)** (실시간 위치 기반 센서 시각화 + 충격 알림 마커 + 히트맵 레이어)

### 🛠 IDE
- **STM32CubeIDE**
- **Visual Studio Code**
- **ArduinoIDE**

## 5. 프로젝트 실행 이미지
<img width="790" alt="데이터" src="https://github.com/user-attachments/assets/416b198a-5735-48fd-88cd-596cd45eb9ed" /> <br>실시간 센서 데이터 MQTT 송수신 (STM32 -> ESP32) <br><br><br><br>

<img width="330" alt="구글맵 지도" src="https://github.com/user-attachments/assets/6d567f0f-5ea6-4b56-a1be-ba38a0a08b3b" /> &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp; 
<img width="330" alt="공기질 나쁨" src="https://github.com/user-attachments/assets/565ed62f-3459-4d73-91d8-9bc762672253" /> <br> 센서 데이터 구글맵 시각화 및 위치 확인 &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp; 공기질에 따른 핀 색상 변경 (빨강, 노랑 ,초록)<br><br><br><br><br><br>
<img width="330" alt="충격 감지 2" src="https://github.com/user-attachments/assets/88ee9eed-28f0-448e-8d3f-c78d477be70b" /> &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;
<img width="330" alt="포트홀 감지 히트맵2" src="https://github.com/user-attachments/assets/457a4daf-6916-4616-8d28-4564768a1bd3" /> <br> 포트홀 충격으로 인한 알림 자동 표시 &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; 포트홀 감지 빈도에 따른 히트맵 시각화 <br><br><br>

## 6. 설치 및 실행
**펌웨어 플래싱 (STM32)**
```bash
# STM32CubeIDE에서 전체 프로젝트 빌드 후, 보드에 업로드
# 필요시 ST-Link 사용

# 주요 센서 및 모듈
- MPU6050 (I2C)
- DHT22 (GPIO)
- MQ135 (ADC)
- GPS (UART)
```
**ESP32 펌웨어 업로드**
```bash
# PlatformIO / Arduino IDE를 사용하여 업로드
# 설정: buad rate : 115200 / ESP32-WROOM-32

# 주요 기능
- STM32에서 UART로 수신한 JSON 데이터 MQTT로 송신
- Wi-Fi 연결 및 MQTT 브로커 연동
```

**MQTT 브로커 설치 (선택)**
```bash
# 로컬 MQTT 브로커 실행 (Mosquitto 등)
sudo apt install mosquitto
mosquitto

# 또는 public MQTT broker 사용 가능
```

**전체 실행 순서 요약**
```text
1. STM32 보드 전원 ON (센서 → ESP32로 UART 송신)
2. ESP32 Wi-Fi 연결 및 MQTT 메시지 발행
3. 웹 클라이언트(mqtt.html)에서 WebSocket으로 브로커에 연결
4. 브라우저에서 실시간 상태 확인
```

## 7. 향후 개선할 점
+ 수집된 센서 데이터를 기반으로 이동 패턴 분석 및 포트홀 위험도 예측 등 지능형 판단이 가능한 머신러닝 모델 도입.
+ 외부에서도 안정적으로 동작할 수 있도록 리튬 배터리 + 충전 모듈(TP4056 등)을 도입하여 휴대성과 실용성 증진.
+ 시스템 전체를 저전력 기반으로 설계하여, 대기 중에는 Sleep 모드로 진입하고 이벤트 발생 시에만 인터럽트를 통해 활성화되는 구조 구현.
-------------------------------------------------------------------------------------
고려해 볼 만한 사항
+ 이동평균필터 외에 다양한 DSP 적용.
+ 단순한 HTML이 아닌 서버와 데이터베이스 도입.
