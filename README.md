# Smart-Move-Sensor 
# 이동형 환경 및 충격 감지 시스템

## 1. 프로젝트 소개
내용쓰기

## 2. 프로젝트 이미지

<img width="450" alt="작품사진" src="https://github.com/user-attachments/assets/043153a3-3059-4a3f-bdfa-932dab903b04">

## 3. 프로젝트 회로도

<img width="850" alt="지하철 회로도" src="https://github.com/user-attachments/assets/6d929607-3185-4ba5-b549-eba194ff2b51">

## 5. 프로젝트 실행 이미지
<img width="330" alt="빈좌석" src="https://github.com/user-attachments/assets/2745f2a7-9344-44ce-b4c8-71d6fbafec09" /><p>&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;빈좌석 이미지</p>
<img width="330" alt="좌석점유" src="https://github.com/user-attachments/assets/796346cc-e048-4780-8966-c9bb245755ea" />
<img width="330" alt="이상좌석" src="https://github.com/user-attachments/assets/30dfd44f-be11-4f61-9228-3ffd9485f989" /> <br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;좌석점유 이미지 &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;이상값 좌석 이미지<br><br><br> 
<img width="800" alt="이상값" src="https://github.com/user-attachments/assets/732a0738-6e32-4dbf-b688-ed0406b4a4d9" /> <br> AI 모델 이상값 탐지 : distance가 1213.4로 측정되어 이상값이라 판단.

## 6. 설치 및 실행
**C프로그램 컴파일**
```bash
# 라즈베리파이에서 컴파일 진행
gcc -o SubwaySensor SubwaySensor.c -lwiringPi
```
**Python 라이브러리 설치**
```bash
python3 -m pip install numpy pandas scipy scikit-learn

# pip 설치 안될 시 가상환경을 통해서 설치
python3 -m venv 설정할 이름

# 가상환경 활성화
source 설정할 이름/bin/activate
```

**Node 서버 가동 및 실행**
```bash
node SubwayServer
```
## 7. 향후 개선할 점
+ C -> Node(서버)로 보낼 때 단순 버퍼 형식이 아니라 UART나 TCP/UDP 같은 통신 활용.
+ AI 모델 평가를 위해 정확도 및 손실함수를 적용.
+ 전송 데이터를 문자열 -> 바이너리로 바꿔서 데이터 크기 감소 및 전송 속도 향상.
+ LM35(온도센서)에 디지털 신호 처리(DSP) 연습 겸 적용.
-------------------------------------------------------------------------------------
고려해 볼 만한 사항
+ CRC를 추가하여 비트 검사.
+ 오프라인 모드를 추가하여 네트워크가 끊기더라도 최근 좌석 상태를 로컬에서 유지하도록 개선.
+ Isolation Forest 외에 다른 모델 추가 (Ensemble Learning).

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
