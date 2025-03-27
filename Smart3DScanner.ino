/*
 * Arduino 3D Scanner Project
 * 
 * 版本資訊：
 * - 版本：v1.0.0
 * - 最後更新：2024-03-21
 * - 作者：EricaHwan
 * 
 * 功能說明：
 * 這是一個基於Arduino的3D掃描系統，結合了步進馬達、伺服馬達和相機控制，
 * 可以實現自動化的3D模型掃描功能。
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
 * - 數位相機（支援遙控拍攝）
 */

#include <Servo.h>

// 定義腳位
const int TOUCH_SENSOR_PIN = A0;    // 觸摸感測器接腳
const int SERVO_PIN = 9;            // 伺服馬達接腳
const int BUTTON_PIN = 2;           // 按鈕接腳
const int BUZZER_PIN = 3;           // 蜂鳴器接腳
const int ROTARY_PIN = A1;          // 旋轉開關接腳
const int LED_PIN = 13;             // LED接腳
const int CAMERA_PIN = 7;           // 相機控制接腳

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
const int PHOTO_DELAY = 2000;       // 拍照延遲時間（毫秒）

// 相機控制相關常數
const int SHUTTER_DURATION = 100;   // 快門觸發時間（毫秒）
const int CAMERA_WARMUP = 1000;     // 相機預熱時間（毫秒）

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
  digitalWrite(CAMERA_PIN, LOW);  // 確保初始狀態為低電位
  delay(CAMERA_WARMUP);           // 等待相機預熱
  Serial.println("Camera ready!");
}

// 拍照函數
void takePhoto() {
  Serial.println("Taking photo...");
  
  // 觸發快門
  digitalWrite(CAMERA_PIN, HIGH);
  delay(SHUTTER_DURATION);
  digitalWrite(CAMERA_PIN, LOW);
  
  // 提供視覺和聲音反饋
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000, 100);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Photo taken!");
}

// 掃描模式函數
void performScan() {
  Serial.println("Starting 3D scan...");
  tone(BUZZER_PIN, 1000, 500);  // 開始掃描提示音
  
  // 初始化相機
  initializeCamera();
  
  currentAngle = 0;  // 重置角度計數
  
  while(currentAngle < 360) {
    // 等待系統啟動後按下按鈕
    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(500);  // 防彈跳
      
      // 拍照
      takePhoto();
      
      // 等待指定時間
      delay(PHOTO_DELAY);
      
      // 旋轉5度
      rotateAngle(5);
      
      // 輸出當前角度
      Serial.print("Current angle: ");
      Serial.println(currentAngle);
    }
  }
  
  tone(BUZZER_PIN, 2000, 500);  // 掃描完成提示音
  Serial.println("Scan completed!");
}

void setup() {
  // 初始化腳位
  pinMode(TOUCH_SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(CAMERA_PIN, OUTPUT);
  
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
          systemActive = false;  // 掃描完成後自動關閉
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