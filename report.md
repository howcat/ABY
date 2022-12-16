# 如何改善效能問題
## 動機
+ 讓MPC program可以兼顧效能和安全運算的特性。
## 用效能工具分析
+ using tool perf/gropf
    + 分析程式碼的hotspot
    + 花在哪個code segment上面的時間最多
### 目前的想法
+ 改善用很多的部分 -> 效能提升
#### 實際上想像和跑過分析軟體的差別
+ 計算 
    + SHR
    + 改寫 shr 
    + ![](https://i.imgur.com/HShS3GS.png)
+ I/O
    + 直接改gate function 
+ 分析
+ 改善
### 目前需要解決的困難。
#### 特別是要考量浮點數的誤差。
- 設計實驗的方法
    - 怎麼樣讓實驗前和實驗後的數據有明顯差異
    - 正明說這個方法是有效的
- use SIMD 指令
    - 預處理DATA的擺放
- Using of SHR
    - 把原本的CODE變簡潔一點
    - 是否能增加compiler翻譯的速度還有待驗證。
- 針對PutFPGate的使用
    - 特別是處理浮點數的議題
    - 目前的做法是丟進gate前針對整數和小數部分處理
    - 但是需要真正運算(sqrt)可能的作法
        - 只能用逼近的值
        - 會有誤差
### 遇到困難嘗試解決的方式
- 程式碼 SEGMENTATION FAULT 
    - CAMKE要使用GDB需要安裝另外的套件
#### REFERENCE
- https://bytefreaks.net/programming-2/cc-how-do-you-set-gdb-debug-flag-g-with-cmake