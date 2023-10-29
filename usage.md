# MPC_math.h Usage

1. 將"MPC_math"資料夾底下的"MPC_math.h"和"MPC_math.cpp"下載下來
2. 使用"include <MPC_math.h>"
3. 編譯時將下載下來的檔案一起編譯即可

例：使用ABY Library時，在CMakeLists.txt中加入下面兩行：
~~~
add_executable(yourexe yourprogramfile.cpp MPC_math.cpp)
target_link_libraries(yourexe ABY::aby ENCRYPTO_utils::encrypto_utils)
~~~

詳請可以參考ABY Library中的CMakeLists.txt，或是有其他可以一起編譯的方法都可以。