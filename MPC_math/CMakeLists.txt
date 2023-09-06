add_executable(MPC_math_test MPC_math_test.cpp MPC_math.cpp)
target_link_libraries(MPC_math_test ABY::aby ENCRYPTO_utils::encrypto_utils)
add_executable(MPC_math_verification MPC_math_verification.cpp MPC_math.cpp)
target_link_libraries(MPC_math_verification ABY::aby ENCRYPTO_utils::encrypto_utils)