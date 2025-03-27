#include <Servo.h>

// 定義腳位
const int TOUCH_SENSOR_PIN = A0;    // 觸摸感測器接腳
const int SERVO_PIN = 9;            // 伺服馬達接腳
const int BUTTON_PIN = 2;           // 按鈕接腳
const int BUZZER_PIN = 3;           // 蜂鳴器接腳
const int ROTARY_PIN = A1;          // 旋轉開關接腳
const int LED_PIN = 13;             // LED接腳

// 步進馬達腳位
const int STEP_PIN = 4;             // 步進馬達步進腳
const int DIR_PIN = 5;              // 步進馬達方向腳
const int ENABLE_PIN = 6;           // 步進馬達使能腳

// 定義變數
Servo caseServo;                    // 伺服馬達物件
int touchValue = 0;                 // 觸摸值
int rotationSpeed = 500;            // 旋轉速度
bool systemActive = false;          // 系統狀態
int rotaryValue = 0;                // 旋轉開關值
int displayMode = 0;                // 展示模式
unsigned long lastTouchTime = 0;    // 上次觸摸時間
bool isRotating = false;            // 是否正在旋轉

// 步進馬達控制函數
void stepMotor(int steps, int direction) {
  digitalWrite(DIR_PIN, direction);
  digitalWrite(ENABLE_PIN, HIGH);
  
  for(int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(rotationSpeed);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(rotationSpeed);
  }
  
  digitalWrite(ENABLE_PIN, LOW);
}

void setup() {
  // 初始化腳位
  pinMode(TOUCH_SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // 初始化步進馬達腳位
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);  // 初始時禁用步進馬達
  
  // 初始化伺服馬達
  caseServo.attach(SERVO_PIN);
  caseServo.write(0);  // 初始位置（關閉）
  
  // 初始化序列通訊
  Serial.begin(9600);
}

void loop() {
  // 讀取感測器值
  touchValue = analogRead(TOUCH_SENSOR_PIN);
  rotaryValue = analogRead(ROTARY_PIN);
  
  // 更新旋轉速度（根據旋轉開關值調整）
  rotationSpeed = map(rotaryValue, 0, 1023, 100, 1000);
  
  // 檢查按鈕狀態
  if (digitalRead(BUTTON_PIN) == LOW) {
    displayMode = (displayMode + 1) % 3;  // 切換展示模式（0-2）
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
        caseServo.write(90);  // 打開展示櫃
        isRotating = true;    // 開始旋轉
        tone(BUZZER_PIN, 1000, 200);  // 短提示音
        
        // 根據不同模式執行不同的旋轉動作
        switch(displayMode) {
          case 0:  // 持續旋轉
            while(systemActive) {
              stepMotor(200, HIGH);
              delay(1000);
            }
            break;
          case 1:  // 來回旋轉
            while(systemActive) {
              stepMotor(200, HIGH);
              delay(1000);
              stepMotor(200, LOW);
              delay(1000);
            }
            break;
          case 2:  // 間歇旋轉
            while(systemActive) {
              stepMotor(100, HIGH);
              delay(2000);
            }
            break;
        }
      } else {
        caseServo.write(0);   // 關閉展示櫃
        isRotating = false;   // 停止旋轉
        digitalWrite(ENABLE_PIN, LOW);  // 禁用步進馬達
        tone(BUZZER_PIN, 500, 200);     // 短提示音
      }
    }
  }
  
  // 輸出除錯訊息
  Serial.print("Touch Value: ");
  Serial.print(touchValue);
  Serial.print(" Rotation Speed: ");
  Serial.print(rotationSpeed);
  Serial.print(" Display Mode: ");
  Serial.print(displayMode);
  Serial.print(" System Active: ");
  Serial.println(systemActive);
  
  delay(100);  // 短延遲以提高響應速度
} 