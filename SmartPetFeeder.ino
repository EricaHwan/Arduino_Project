#include <Servo.h>

// 定義腳位
const int FOOD_SENSOR_PIN = A0;     // 食物感測器接腳
const int SERVO_PIN = 9;            // 伺服馬達接腳
const int BUTTON_PIN = 2;           // 按鈕接腳
const int BUZZER_PIN = 3;           // 蜂鳴器接腳
const int ROTARY_PIN = A1;          // 旋轉開關接腳
const int LED_PIN = 13;             // LED接腳

// 定義變數
Servo foodServo;                    // 伺服馬達物件
int foodLevel = 0;                  // 食物量值
int feedAmount = 500;               // 餵食量
bool systemActive = false;          // 系統狀態
int rotaryValue = 0;                // 旋轉開關值
unsigned long lastFeedTime = 0;     // 上次餵食時間
const unsigned long FEED_INTERVAL = 3600000;  // 餵食間隔（1小時）

void setup() {
  // 初始化腳位
  pinMode(FOOD_SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // 初始化伺服馬達
  foodServo.attach(SERVO_PIN);
  foodServo.write(0);  // 初始位置（關閉）
  
  // 初始化序列通訊
  Serial.begin(9600);
}

void loop() {
  // 讀取感測器值
  foodLevel = analogRead(FOOD_SENSOR_PIN);
  rotaryValue = analogRead(ROTARY_PIN);
  
  // 更新餵食量（根據旋轉開關值調整）
  feedAmount = map(rotaryValue, 0, 1023, 200, 800);
  
  // 檢查按鈕狀態
  if (digitalRead(BUTTON_PIN) == LOW) {
    systemActive = !systemActive;  // 切換系統狀態
    digitalWrite(LED_PIN, systemActive);  // 更新LED狀態
    delay(500);  // 防彈跳
  }
  
  // 系統運作邏輯
  if (systemActive) {
    unsigned long currentTime = millis();
    
    // 檢查是否到達餵食時間
    if (currentTime - lastFeedTime >= FEED_INTERVAL) {
      // 檢查食物量是否足夠
      if (foodLevel > feedAmount) {
        // 開始餵食
        foodServo.write(90);  // 打開食物出口
        tone(BUZZER_PIN, 1000);  // 發出提示音
        delay(2000);  // 餵食2秒
        foodServo.write(0);  // 關閉食物出口
        noTone(BUZZER_PIN);  // 停止提示音
        
        lastFeedTime = currentTime;  // 更新上次餵食時間
      } else {
        // 食物不足警告
        for(int i = 0; i < 3; i++) {
          tone(BUZZER_PIN, 2000);
          delay(200);
          noTone(BUZZER_PIN);
          delay(200);
        }
      }
    }
  } else {
    foodServo.write(0);  // 確保系統關閉時伺服馬達回到原位
  }
  
  // 輸出除錯訊息
  Serial.print("Food Level: ");
  Serial.print(foodLevel);
  Serial.print(" Feed Amount: ");
  Serial.print(feedAmount);
  Serial.print(" System Active: ");
  Serial.println(systemActive);
  
  delay(1000);  // 延遲1秒
} 