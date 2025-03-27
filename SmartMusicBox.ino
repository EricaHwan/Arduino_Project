#include <Servo.h>

// 定義腳位
const int TOUCH_SENSOR_PIN = A0;    // 觸摸感測器接腳
const int SERVO_PIN = 9;            // 伺服馬達接腳
const int BUTTON_PIN = 2;           // 按鈕接腳
const int BUZZER_PIN = 3;           // 蜂鳴器接腳
const int ROTARY_PIN = A1;          // 旋轉開關接腳
const int LED_PIN = 13;             // LED接腳

// 定義變數
Servo boxServo;                     // 伺服馬達物件
int touchValue = 0;                 // 觸摸值
int volume = 500;                   // 音量
bool systemActive = false;          // 系統狀態
int rotaryValue = 0;                // 旋轉開關值
int musicMode = 0;                  // 音樂模式
unsigned long lastTouchTime = 0;    // 上次觸摸時間

// 定義音樂音符頻率
const int NOTE_B0 = 31;
const int NOTE_C1 = 33;
const int NOTE_D1 = 37;
const int NOTE_E1 = 41;
const int NOTE_F1 = 44;
const int NOTE_G1 = 49;
const int NOTE_A1 = 55;
const int NOTE_B1 = 62;
const int NOTE_C2 = 65;

// 定義音樂模式1（小星星）
const int melody1[] = {
  NOTE_C1, NOTE_C1, NOTE_G1, NOTE_G1, NOTE_A1, NOTE_A1, NOTE_G1,
  NOTE_F1, NOTE_F1, NOTE_E1, NOTE_E1, NOTE_D1, NOTE_D1, NOTE_C1
};

// 定義音樂模式2（生日快樂）
const int melody2[] = {
  NOTE_C1, NOTE_C1, NOTE_D1, NOTE_C1, NOTE_F1, NOTE_E1,
  NOTE_C1, NOTE_C1, NOTE_D1, NOTE_C1, NOTE_G1, NOTE_F1
};

// 定義音樂模式3（歡樂頌）
const int melody3[] = {
  NOTE_E1, NOTE_E1, NOTE_F1, NOTE_G1, NOTE_G1, NOTE_F1, NOTE_E1, NOTE_D1,
  NOTE_C1, NOTE_C1, NOTE_D1, NOTE_E1, NOTE_E1, NOTE_D1, NOTE_D1
};

void setup() {
  // 初始化腳位
  pinMode(TOUCH_SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // 初始化伺服馬達
  boxServo.attach(SERVO_PIN);
  boxServo.write(0);  // 初始位置（關閉）
  
  // 初始化序列通訊
  Serial.begin(9600);
}

void playMelody(const int melody[], int length) {
  for (int i = 0; i < length; i++) {
    int noteDuration = 1000 / 4;  // 四分音符
    tone(BUZZER_PIN, melody[i], noteDuration);
    delay(noteDuration * 1.3);  // 音符間隔
  }
  noTone(BUZZER_PIN);
}

void loop() {
  // 讀取感測器值
  touchValue = analogRead(TOUCH_SENSOR_PIN);
  rotaryValue = analogRead(ROTARY_PIN);
  
  // 更新音量（根據旋轉開關值調整）
  volume = map(rotaryValue, 0, 1023, 200, 800);
  
  // 檢查按鈕狀態
  if (digitalRead(BUTTON_PIN) == LOW) {
    musicMode = (musicMode + 1) % 3;  // 切換音樂模式（0-2）
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(500);  // 防彈跳
  }
  
  // 檢查觸摸感測
  if (touchValue > 500) {  // 觸摸閾值
    unsigned long currentTime = millis();
    if (currentTime - lastTouchTime > 2000) {  // 防止重複觸發
      systemActive = !systemActive;  // 切換系統狀態
      lastTouchTime = currentTime;
      
      if (systemActive) {
        boxServo.write(90);  // 打開音樂盒
        // 根據當前模式播放音樂
        switch(musicMode) {
          case 0:
            playMelody(melody1, 14);
            break;
          case 1:
            playMelody(melody2, 12);
            break;
          case 2:
            playMelody(melody3, 15);
            break;
        }
      } else {
        boxServo.write(0);  // 關閉音樂盒
        noTone(BUZZER_PIN);
      }
    }
  }
  
  // 輸出除錯訊息
  Serial.print("Touch Value: ");
  Serial.print(touchValue);
  Serial.print(" Volume: ");
  Serial.print(volume);
  Serial.print(" Music Mode: ");
  Serial.print(musicMode);
  Serial.print(" System Active: ");
  Serial.println(systemActive);
  
  delay(100);  // 短延遲以提高響應速度
} 