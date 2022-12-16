add_executable(SHR_test SHR_test.cpp common/SHR.cpp)
target_link_libraries(SHR_test ABY::aby ENCRYPTO_utils::encrypto_utils)