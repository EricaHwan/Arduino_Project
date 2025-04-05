/*
 * Arduino 3D Scanner Project
 * 
 * 版本資訊：
 * - 版本：v1.1.0
 * - 最後更新：2024-03-21
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
 * v1.1.0 (2024-03-21)
 * - 移除相機控制相關程式碼
 * - 優化旋轉控制邏輯
 * - 分離錄影和旋轉功能
 * - 改進使用者操作流程
 */

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
int operationMode = 0;              // 操作模式（0:展示, 1:掃描）
unsigned long lastTouchTime = 0;    // 上次觸摸時間
bool isRotating = false;            // 是否正在旋轉
int currentAngle = 0;               // 當前角度
const int STEPS_PER_DEGREE = 40;    // 每度需要的步數
const int ROTATION_DELAY = 100;     // 旋轉延遲時間（毫秒）

// 相機控制相關常數
const int VIDEO_START_DURATION = 100;   // 開始錄影觸發時間（毫秒）
const int CAMERA_WARMUP = 1000;         // 相機預熱時間（毫秒）

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

// 相機初始化
void initializeCamera() {
  Serial.println("Initializing camera...");
  digitalWrite(LED_PIN, LOW);  // 確保初始狀態為低電位
  delay(CAMERA_WARMUP);           // 等待相機預熱
  Serial.println("Camera ready!");
}

// 開始錄影函數
void startVideo() {
  Serial.println("Starting video recording...");
  
  // 觸發開始錄影
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000, 100);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Video recording started!");
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
  
  // 初始化相機
  initializeCamera();
}

void loop() {
  // 讀取感測器值
  touchValue = analogRead(TOUCH_SENSOR_PIN);
  rotaryValue = analogRead(ROTARY_PIN);
  
  // 更新旋轉速度（根據旋轉開關值調整）
  rotationSpeed = map(rotaryValue, 0, 1023, 100, 1000);
  
  // 檢查按鈕狀態
  if (digitalRead(BUTTON_PIN) == LOW) {
    operationMode = (operationMode + 1) % 2;  // 切換操作模式（0:展示, 1:掃描）
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(500);  // 防彈跳
    
    // 模式切換提示音
    if (operationMode == 0) {
      tone(BUZZER_PIN, 800, 200);  // 展示模式
    } else {
      tone(BUZZER_PIN, 1200, 200); // 掃描模式
    }
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
        
        if (operationMode == 0) {
          // 展示模式：持續旋轉
          while(systemActive) {
            stepMotor(200, HIGH);
            delay(1000);
          }
        } else {
          // 掃描模式：執行掃描程序
          performScan();
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
  Serial.print(" Operation Mode: ");
  Serial.print(operationMode);
  Serial.print(" System Active: ");
  Serial.println(systemActive);
  
  delay(100);  // 短延遲以提高響應速度
} 