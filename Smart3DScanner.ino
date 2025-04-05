/*
 * Arduino 3D Scanner Project
 * 
 * 版本資訊：
 * - 版本：v1.1.0
 * - 最後更新：2025-04-05
 * - 作者：EricaHwan
 * 
 * 功能說明：
 * 這是一個基於Arduino的3D掃描系統，結合了步進馬達和伺服馬達，
 * 可以實現自動化的3D模型掃描功能。系統支援手動控制錄影和自動旋轉掃描。
 * 
 * 硬體需求：
 * - Arduino UNO
 * - 步進馬達
 * - 伺服馬達
 * - 觸摸感測器
 * - 按鈕
 * - LED
 * - 蜂鳴器
 * - 旋轉開關
 * - 手機（用於錄影）
 * 
 * 更新日誌：
 * v1.1.0 (2025-04-05)
 * - 移除相機控制相關程式碼
 * - 優化旋轉控制邏輯
 * - 分離錄影和旋轉功能
 * - 改進使用者操作流程
 */

#include <Servo.h>

// 定義腳位
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
int rotationSpeed = 500;            // 旋轉速度
bool systemActive = false;          // 系統狀態
int rotaryValue = 0;                // 旋轉開關值
int operationMode = 0;              // 操作模式（0:展示, 1:掃描）
unsigned long lastButtonTime = 0;   // 上次按鈕時間
bool isRotating = false;            // 是否正在旋轉
int currentAngle = 0;               // 當前角度
const int STEPS_PER_DEGREE = 40;    // 每度需要的步數
const int ROTATION_DELAY = 100;     // 旋轉延遲時間（毫秒）

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

// 旋轉指定角度
void rotateAngle(int degrees) {
  int steps = degrees * STEPS_PER_DEGREE;
  stepMotor(steps, HIGH);
  currentAngle = (currentAngle + degrees) % 360;
}

// 執行360度掃描
void perform360Scan() {
  Serial.println("Starting 360-degree scan...");
  currentAngle = 0;  // 重置角度計數
  
  while(currentAngle < 360) {
    // 旋轉1度
    rotateAngle(1);
    
    // 等待指定時間
    delay(ROTATION_DELAY);
    
    // 輸出當前角度
    Serial.print("Current angle: ");
    Serial.println(currentAngle);
  }
  
  // 掃描完成提示
  tone(BUZZER_PIN, 2000, 500);
  Serial.println("360-degree scan completed!");
}

// 掃描模式函數
void performScan() {
  Serial.println("Ready for 3D scan...");
  tone(BUZZER_PIN, 1000, 200);  // 準備掃描提示音
  
  // 等待系統啟動後按下按鈕
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(500);  // 防彈跳
    
    // 執行360度掃描
    perform360Scan();
    
    // 掃描完成後自動停止
    systemActive = false;
  }
}

void setup() {
  // 初始化腳位
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
  // 讀取旋轉開關值
  rotaryValue = analogRead(ROTARY_PIN);
  
  // 更新旋轉速度（根據旋轉開關值調整）
  rotationSpeed = map(rotaryValue, 0, 1023, 100, 1000);
  
  // 檢查按鈕狀態
  if (digitalRead(BUTTON_PIN) == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonTime > 1000) {  // 防止重複觸發
      lastButtonTime = currentTime;
      
      if (!systemActive) {
        // 第一次按下：啟動系統
        systemActive = true;
        caseServo.write(90);  // 打開展示櫃
        tone(BUZZER_PIN, 1000, 200);  // 短提示音
      } else {
        // 第二次按下：切換模式
        operationMode = (operationMode + 1) % 2;  // 切換操作模式（0:展示, 1:掃描）
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        
        // 模式切換提示音
        if (operationMode == 0) {
          tone(BUZZER_PIN, 800, 200);  // 展示模式
        } else {
          tone(BUZZER_PIN, 1200, 200); // 掃描模式
        }
        
        // 如果是掃描模式，開始掃描
        if (operationMode == 1) {
          performScan();
        }
      }
    }
  }
  
  // 展示模式：持續旋轉
  if (systemActive && operationMode == 0) {
    stepMotor(200, HIGH);
    delay(1000);
  }
  
  // 輸出除錯訊息
  Serial.print("Rotation Speed: ");
  Serial.print(rotationSpeed);
  Serial.print(" Operation Mode: ");
  Serial.print(operationMode);
  Serial.print(" System Active: ");
  Serial.println(systemActive);
  
  delay(100);  // 短延遲以提高響應速度
} 