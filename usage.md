# MPC_math.h Usage

1. 將"MPC_math"資料夾底下的"MPC_math.h"和"MPC_math.cpp"下載下來
2. 使用"include <MPC_math.h>"
3. 編譯時將下載下來的檔案一起編譯即可

例：使用ABY Library時，在CMakeLists.txt中加入下面兩行：
~~~
add_executable(yourexe yourprogramfile.cpp MPC_math.cpp)
target_link_libraries(yourexe ABY::aby ENCRYPTO_utils::encrypto_utils)
~~~

對ABY Library的[example](https://github.com/encryptogroup/ABY/tree/public/src/examples
)下面的CMakeLists.txt和各資料夾下的CMakeLists.txt去參考和改動就可以了，或是有其他可以一起編譯的方法都可以。

## function使用範例
1. 引入函式庫
`#include "MPC_math.h"`
2. 宣告變數`ABYmath yourVarName`
3. 使用時像這樣`share* in = abymath.aby_fmod(party, circ, input_a, input_b);`

具體範例可以參考[這裡](https://github.com/howcat/ABY/blob/main/MPC_math/MPC_math_test.cpp)