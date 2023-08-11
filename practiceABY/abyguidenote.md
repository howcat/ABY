# ABY Dev Guide筆記
## Overview
+ computing parties將所有不想公開的值，結合ABY做計算(p.4)
+ sharings
    + A
    + B
    + Y
+ circuit
    + ArithmeticCircuit
    + BooleanCircuit
+ gate
    + linear
    + non-linear：越少越好
+ wire
    + 有wire ID(uint32_t)
    + 連接gate
+ share
    + 有點像一個動作
    + 一個部分<sub>(晨恩)</sub>

## Environment

### ABYParty

有以下多個parameters：

1. role：扮演的角色，通常是SERVER或CLIENT
2. address：**預設127.0.0.1**，如果role是SERVER，他會在這個IP address開一個socket(連線的一端)；而role是CLIENT的話，他會嘗試連接address。
3. port：**預設7766**。
4. seclvl：security level，**預設LT**。分為以下幾種：
      + short(ST, 80-bit symmetric security)
      + mid (MT, 112-bit)
      + long (LT, 128-bit)
      + extra-long (XLT, 192-bit)
      + extra-extra-long (XXLT, 256-bit)
5. bitlen：變數大小，**預設32bits**。在arithmetic sharing中可以選擇{8,16,32,64}，boolean sharing和Yao's grabled circuit使用預設值。
6. ntreads：可使用的thread數量。
7. mg_algo：**預設MT_OT**。改變計算過程中的演算法，有MT_OT、MT_PAILLIER、MT_DGK。
8. maxgates：**預設4000000**。最大gate數。

還有以下函式：
1. GetSharings()：拿到其他三個sharings的東西<sub>(p.10)</sub>
2. GetCircuitBuildRoutine()：拿到sharings下的circuit，arithmetic和另外兩個sharings的type不一樣<sub>(p.11)</sub>
3. ExecCircuit()：執行運算<sub>(p.11)</sub>
4. Reset()：重設<sub>(p.11)</sub>
5. delete：刪掉整個ABYParty

### Shares

1. get_wire_id()：回傳對應wire的id<sub>(p.12)</sub>
2. get_wire_ids()：回傳整串wires<sub>(p.12)</sub>
3. get_wire_ids_as_share()：回傳所在的share<sub>(p.12)</sub>
4. set_wire_ids()、set_wire_id()：設定，跟上面差不多<sub>(p.12)</sub>
5. get_clear_value()、get_clear_value_ptr()：從share那取回plaintext<sub>(p.13)</sub>
6. create_new_share()：創建一個share<sub>(p.13)</sub>
Parameters：
    + size：**預設是0**。share中wire的數量。
    + wireids：share中的wire。
    + circ：wire在的circuit。
7. get_bitlength()、set_bitlength()、get_max_bitlength()、set_max_bitlength()：處理bitlength<sub>(p.14)</sub>

## Gates

在circuit中計算的關鍵。分成**I/O Gates**、**Function Gates**、**Conversion Gates**和**Debug Gates**。I/O Gates負責plaintext value和secret-shared value之間的轉換；Function Gates負責secret-shared value的計算；Conversion Gates負責secret-shared value在不同sharings之間的轉換；Debug Gates負責除錯。

> I/O Gates
1. PutINGate()：plaintex -> encryption<sub>(p.15)</sub>
Parameters<sub>(p.16)</sub>：
    + val：可以放array。
    + bitlen：明文讀取長度和share裡wires的數量。
    + role：which party?
3. PutCONSGate()：將雙方都知道的值放入circuit<sub>(p.15)</sub>
4. PutOUTGate()：encryption -> plaintex<sub>(p.15)</sub>
Parameters<sub>(p.17)</sub>：
    + s_out：計算過後產生的share。
    + role：which party?
5. PutSharedINGate()、PutSharedOUTGate()：pre-shared value的I/O，推測是跟先前計算的值，或是來自其他ABY的值有關，IN只在AB有用<sub>(p.15)</sub>
6.  PutDummyINGate()：假裝成別人的input，不用提供plaintext<sub>(p.16)</sub>

> Function Gates
> ![](https://i.imgur.com/AGDU2Hu.png)

1. PutANDGate()...：將兩個share做AND(XOR...)<sub>(p.19)</sub>
2. GT：大於的話回傳1<sub>(p.19)</sub>
3. MUX：sel是1的話回傳ina，否則回傳inb<sub>(p.20)</sub>
4. INV：在A的話，會是2<sup>l</sup>-in；在B的話，會是input每個bit的1跟0顛倒<sub>(p.20)</sub>

> Conversion Gates
> 其實並不是直接改變secret-shared value，而是改變share的型態(像是從arithmetic share變成Yao share)。在ABY互相轉換的6種可能之間，只有A2Y、B2A、B2Y、Y2B是需要實作的，剩下兩個可以用前面四個中的兩個組合。且過程會牽涉都後面sharing的circuit。

1. A2Y、B2A、B2Y、Y2B：請見<sub>(p.21)</sub>
2. A2B(x) = Y2B(A2Y(x))<sub>(p.21)</sub>
3. Y2A(x) = B2A(A2B(x))<sub>(p.21)</sub>

> Debug Gates

1. PutPrintValueGate()：將share中的plaintext value輸出<sub>(p.21)</sub>
2. PutAssertGate()：檢查input和assert_val是否一樣，不一樣的話program會停下來<sub>(p.22)</sub>

## SIMD Gates

> Single Instruction Multiple Data
> 單一指令處理多筆資料，節省時間成本。
> 
> 上述的share都是non-SIMD，share原本是一個一維陣列，裡面存放多個wires。在SIMD的話，share會是二維陣列，具體如下圖：
> ![](https://i.imgur.com/S74UI6S.png)

// nval >> number_of_value

1. PutSIMDINGate()：PutINGate的SIMD版本，增加nval參數，而且可以一次input多個值<sub>(p.24~26)</sub>
2. PutSIMDCONSGate()、PutSIMDAssertGate()：跟上面大同小異<sub>(p.26~27)</sub>
3. assert : debug

> Data Management Gate
> 不是直接更改wire上的值，而是改成更方便計算的SIMD share

1. PutCombinerGate()：將non-SIMD轉換成SIMD<sub>(p.27)</sub>
![](https://i.imgur.com/1ufklDp.png)
2. PutSplitterGate()：將SIMD轉換成non-SIMD<sub>(p.28)</sub>
3. PutCombineAtPosGate()：從SIMD share中抓出指定的值，並放到另外一個SIMD share中<sub>(p.29)</sub>
![](https://i.imgur.com/s18nrqM.png)
4. PutSubsetGate()：從SIMD share中抓出多個值，到另外一個SIMD share中<sub>(p.30)</sub>
![](https://i.imgur.com/ZATvYCY.png)
5. PutPermutationGate()：從不同wire取出不同位置的值，放進新的SIMD share<sub>(p.31)</sub>
![](https://i.imgur.com/eZsSmZm.png)
6. PutRepeaterGate()：將輸入的non-SIMD值轉成SIMD並重複nval次<sub>(p.32)</sub>
7. get_nvals_on_wire()：根據wireid拿到nval<sub>(p.33)</sub>
8. get_clear_value_vec()：取得plaintext，放到vector裡<sub>(p.34)</sub>

## Benchmarking

1. PRINT_PERFORMANCE_STATS：gate的數量和runtime的資訊<sub>(p.35)</sub>
2. PRINT_COMMUNICATION_STATS：送出和接收的次數<sub>(p.35)</sub>
3. BENCHONLINEPHASE：<sub>(p.35)</sub>

<!-- 類似建志的log -->

