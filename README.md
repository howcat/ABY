# ABY
## 大綱
+ practiceABY 目前有我們實作的ceil function
    + shr版本尚未完成，目前完成的是使用aby library的版本。
    + project_2.cpp 裡面的```share* BuildCeilCircuit```
+ 三份 note 是目前看文件做的紀錄
    + report 目前想到的專題方向.
    + papernote : 學長的paper.
    + abyguidenote : abyguide筆記.
---
+ 12/16 update
    + shr版本的 ceil()debug 進行中.
        + 卡在 party->Exect無法執行.
        + segmentation fault , 目前猜測是參數傳遞問題。
        + 之前有用過gdb查看過類似問題，可以用這個試試看。
    + 目前先撰寫註解方便日後debug.
    + 正在建構 aby library 的 power() 版本.
--- 
+ 01/13
    + 找出執行時間3秒以上的MPC program
    + 並用效能分析工具加以分析
+ 執行時間的問題
    + 用TIME直接測和gprof的差異問題

