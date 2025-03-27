#include <Servo.h>

// 定義腳位
const int WATER_SENSOR_PIN = A0;    // 水位感測器接腳
const int SERVO_PIN = 9;            // 伺服馬達接腳
const int BUTTON_PIN = 2;           // 按鈕接腳
const int BUZZER_PIN = 3;           // 蜂鳴器接腳
const int ROTARY_PIN = A1;          // 旋轉開關接腳
const int LED_PIN = 13;             // LED接腳

// 定義變數
Servo wateringServo;                // 伺服馬達物件
int waterLevel = 0;                 // 水位值
int threshold = 500;                // 水位閾值
bool systemActive = false;          // 系統狀態
int rotaryValue = 0;                // 旋轉開關值

void setup() {
  // 初始化腳位
  pinMode(WATER_SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // 初始化伺服馬達
  wateringServo.attach(SERVO_PIN);
  wateringServo.write(0);  // 初始位置
  
  // 初始化序列通訊
  Serial.begin(9600);
}

void loop() {
  // 讀取感測器值
  waterLevel = analogRead(WATER_SENSOR_PIN);
  rotaryValue = analogRead(ROTARY_PIN);
  
  // 更新閾值（根據旋轉開關值調整）
  threshold = map(rotaryValue, 0, 1023, 200, 800);
  
  // 檢查按鈕狀態
  if (digitalRead(BUTTON_PIN) == LOW) {
    systemActive = !systemActive;  // 切換系統狀態
    digitalWrite(LED_PIN, systemActive);  // 更新LED狀態
    delay(500);  // 防彈跳
  }
  
  // 系統運作邏輯
  if (systemActive) {
    if (waterLevel < threshold) {
      // 水位過低，開始澆水
      wateringServo.write(90);  // 轉動伺服馬達
      tone(BUZZER_PIN, 1000);  // 發出提示音
      delay(1000);  // 澆水1秒
      wateringServo.write(0);  // 回到原位
      noTone(BUZZER_PIN);      // 停止提示音
    }
  } else {
    wateringServo.write(0);  // 確保系統關閉時伺服馬達回到原位
  }
  
  // 輸出除錯訊息
  Serial.print("Water Level: ");
  Serial.print(waterLevel);
  Serial.print(" Threshold: ");
  Serial.print(threshold);
  Serial.print(" System Active: ");
  Serial.println(systemActive);
  
  delay(1000);  // 延遲1秒
} 