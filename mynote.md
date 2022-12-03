{%hackmd BJrTq20hE %}
# About MPC


---
* ABY party 計算的一坨東西
  要用兩個
  計算過程要共享
  第三方進來的 put shared in
* putcons : 一起的
* template uint8 uint32
* 不公開的蒜
* sharing : 一個公司的三個部門  
* 三個部門要1起做事情
* share和sharing的差別
* share->circuit->gate,wire
---
## 碰到的問題與解決方法
- 路徑問題 : 要+/
- fpGate參數問題 : 不能用gtgate
    - FPGATE都是0
    - ? : 
---
## Question0
---
* data type?
* wrapper?


* 把電路合成變成c語言
---

## Question1
---
* 汇编语言SHR（右移）指令：将操作数逻辑右移一位

  mov dl, 32           ;移动前：00100000 = 32
  shr dl, 1               ;移动后：00010000 = 16

---


## master_v9
---
* SHR
![](https://i.imgur.com/nZ4oPn5.png)(NO)
---
* multiplexer 
condition : selection line
only one output

---
### MPC compiler 做的事情
* prehandling
  找出被保護的變數
* 1
  處理迴圈邊界
* 2
  做完for和while後，換做switch
  接著處理陣列中的index
  
###  SHR PROCEDURE
做了一個類似初始化的動作
類似CONSTRUCTOR

---

* ORAM 
  把等號右邊寫成機器語言
### Experiment
* 做實驗的準確性

---

### Evaluation

* MPC_cov() -> coverance 
* MPC_mean() -> medium

  math operation 
  
---

###  Conclusion

* compiler : 將c語言轉換成mpc
  definition of MPC program
  
* 用c++ 的API 來跨平台

* Definition of Lan
  single pc versus networking 
  
---

### 目前可公開情報
* .MUX()中的參數未知<sub>(p.32)</sub>
* .ORAM()中的參數未知<sub>(p.35)</sub>
* ORAM近來就是來解決這個問題，隱藏數據的訪問模式。比如一種簡單的方案，就是每次把雲上存儲的數據都讀到本地，然後找到自己需要讀取或者更新的數據，然後再全部寫回到雲服務器上。這樣雲服務器就不知道你讀了或者更新了什麼數據。這就是一種最簡單的ORAM方案，只是開銷太大了，ORAM研究的一大方向就是針對計算複雜度和帶寬開銷進行優化。

// 看起來像把資料型態藏起來
* 什麼是局部區域網絡 (LAN)？
  局部區域網絡 (LAN) 是家中或辦公室內一組已連線至有線網絡和 Wi-Fi 的   裝置。局部區域網絡是您的個人網絡，由電腦、手機、平板電腦、路由器等裝置    組成。
  
  
 ABY library 去寫MPC code
1.  understand MPC code ( v )
2.  example( )

https://www.youtube.com/watch?v=_mDlLKgiFDY&t=1s
https://www.youtube.com/watch?v=90jcXCHsBF0

## code
* 1.函式....
* 2.result...
* 3.計算的過程都當成share 
* 4.統一型態 ， 為了安全 。  ( 都是share )

---
## COMPILER
* 產生目標碼 
* 目標碼 
1. **檔案**形式儲存
2. C -> 組語
3. compiler vs interpreter
   有執行環境，可直接執行。
### 把source code 轉成 object code 
通常object code 是低階語言
可以給機器直接執行

### 從source code 到 object code ， compiler幹了啥
![](https://i.imgur.com/FuDgRJs.png)


#### front-end
  
  AST : Abstract Syntax Tree 抽象語法樹
  parser : 把 token 變成語法樹
  CFG : 上下文無關文法 Context Free Grammar
  定義好CFG後，丟給parser generator
  
---
  ![](https://i.imgur.com/tvJOGzT.png)
  
  ![](https://i.imgur.com/Qo76AJG.png)

  **把CODE變成TREE**
  
---
* Semantic Analyzer  語意分析
根據上下文來判斷是否合乎該語言的語意
ex : 同樣的 scope 重複宣告相同變數
  
* ![](https://i.imgur.com/NzDHaER.png)

#### middle-end and back-end
---
![](https://i.imgur.com/LHISSkW.png)

---
### 寫compiler需要啥
* Lexer 詞彙分析
* Semantic Analyzer 語法分析 

---

### LLVM  ?

* VM : virtual machine
* EX : php 的 ZendEngine
---
![](https://i.imgur.com/XfzT5XY.png)

---

## FRONT - END 
### 引用資料
* https://blog.csdn.net/u010833547/article/details/108501924
* https://hackmd.io/@ShenTengTu/HJzCM3aDr
* https://hackmd.io/@sysprog/c-compiler-optimization?type=view
### front-end 流程
![](https://i.imgur.com/yHbOnN2.png)
#### Lexer
* 想辦法把接收到的一堆字個別分出來並且標註「名詞」「動詞」「 介系詞」等等的。把程式碼切成一個一個 token
* Lexical grammer 會定義這個語言的文法中每個字詞的規則
---
![](https://i.imgur.com/8Nfsk0u.png)
字串內容 和 type 

---

![](https://i.imgur.com/R8s7I92.png)

---

#### Parser / syntax analyzer 
* 你要先把規則告訴她才行
---
![](https://i.imgur.com/yATgMyP.png)

#### semantic analyer 
* ![](https://i.imgur.com/rbNuCBF.png)
* ![](https://i.imgur.com/pvMYkgv.png)
---

### BNF 【編譯器學習】
 ![](https://i.imgur.com/er08YEk.png)
 