add_executable(project_2_test project_2_test.cpp common/project_2.cpp)
target_link_libraries(project_2_test ABY::aby ENCRYPTO_utils::encrypto_utils)
# debug for core dump
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")