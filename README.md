# Arduino 3D Scanner Project

## 版本資訊
- 版本：v1.1.0
- 最後更新：2024-03-21
- 作者：EricaHwan

## 專案說明
這是一個基於Arduino的3D掃描系統，可以實現自動化的3D模型掃描功能。系統支援手動控制錄影和自動旋轉掃描，適合用於拍攝物件的360度影片。

## 功能特點
- 自動360度旋轉掃描
- 可調整旋轉速度
- 觸摸感測控制
- 展示模式和掃描模式切換
- 視覺和聲音反饋
- 支援手動控制錄影

## 硬體需求
- Arduino UNO
- 步進馬達
- 伺服馬達
- 觸摸感測器
- 按鈕
- LED
- 蜂鳴器
- 旋轉開關
- 手機（用於錄影）

## 接線說明
1. 觸摸感測器 -> A0
2. 伺服馬達 -> 9
3. 按鈕 -> 2
4. 蜂鳴器 -> 3
5. 旋轉開關 -> A1
6. LED -> 13
7. 步進馬達步進腳 -> 4
8. 步進馬達方向腳 -> 5
9. 步進馬達使能腳 -> 6

## 使用說明
1. 硬體設置
   - 按照接線說明連接所有元件
   - 確保步進馬達有足夠的供電
   - 將手機放置在適當位置準備錄影

2. 操作流程
   - 觸摸感測器啟動系統
   - 按下按鈕切換到掃描模式（會有提示音）
   - 用手機開始錄影
   - 再次按下按鈕開始360度旋轉掃描
   - 等待旋轉完成（會有完成提示音）

3. 模式說明
   - 展示模式：持續旋轉展示
   - 掃描模式：執行一次360度掃描

## 可調整參數
- `STEPS_PER_DEGREE`：每度需要的步數（預設：40）
- `ROTATION_DELAY`：旋轉延遲時間（預設：100ms）
- 旋轉速度：通過旋轉開關即時調整

## 故障排除
1. 系統無反應
   - 檢查電源供應
   - 確認所有接線正確
   - 檢查觸摸感測器閾值

2. 旋轉不順暢
   - 調整旋轉速度
   - 檢查步進馬達供電
   - 確認 `STEPS_PER_DEGREE` 設置

## 授權資訊
本專案採用 MIT 授權條款 - 詳見 [LICENSE](LICENSE) 文件

## 更新日誌
### v1.1.0 (2024-03-21)
- 移除相機控制相關程式碼
- 優化旋轉控制邏輯
- 分離錄影和旋轉功能
- 改進使用者操作流程

### v1.0.0 (2024-03-21)
- 初始版本發布
- 基本功能實現
- 完整的硬體控制
- 相機控制功能 