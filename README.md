# Smart-Move-Sensor 
이동형 환경 및 충격 감지 시스템
그

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
