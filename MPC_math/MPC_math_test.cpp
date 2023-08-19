//Utility libs
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <random>
//ABY Party class
#include "MPC_math.h"

int32_t read_test_options(int32_t* argcp, char*** argvp, e_role* role,
		uint32_t* bitlen, uint32_t* nvals, uint32_t* secparam, std::string* address,
		uint16_t* port, int32_t* test_op, double* base, double* exp) {

	uint32_t int_role = 0, int_port = 0;

	parsing_ctx options[] =
			{ { (void*) &int_role, T_NUM, "r", "Role: 0/1", true, false }, 
              {	(void*) nvals, T_NUM, "n", "Number of parallel operation elements", false, false }, 
              { (void*) bitlen, T_NUM, "b", "Bit-length, default 32", false, false }, 
              { (void*) secparam, T_NUM, "s", "Symmetric Security Bits, default: 128", false, false }, 
              { (void*) address, T_STR, "a", "IP-address, default: localhost", false, false }, 
              { (void*) &int_port, T_NUM, "p", "Port, default: 7766", false, false }, 
              { (void*) test_op, T_NUM, "t", "Single test (leave out for all operations), default: off", false, false }
              
            };

	if (!parse_options(argcp, argvp, options,
			sizeof(options) / sizeof(parsing_ctx))) {
		print_usage(*argvp[0], options, sizeof(options) / sizeof(parsing_ctx));
		std::cout << "Exiting" << std::endl;
		exit(0);
	}

	assert(int_role < 2);
	*role = (e_role) int_role;

	if (int_port != 0) {
		assert(int_port < 1 << (sizeof(uint16_t) * 8));
		*port = (uint16_t) int_port;
	}

	//delete options;

	return 1;
}

int main(int argc, char** argv) {
	e_role role;
	uint32_t bitlen = 64, nvals = 4, secparam = 128, nthreads = 1;
	uint16_t port = 7766;
	std::string address = "127.0.0.1";
	int32_t test_op = -1;
    double a = 0.0, b = 0.0;
	e_mt_gen_alg mt_alg = MT_OT;

	read_test_options(&argc, &argv, &role, &bitlen, &nvals, &secparam, &address,
			&port, &test_op, &a, &b);
    
	seclvl seclvl = get_sec_lvl(secparam);

    // a = -2.56681, b = 3.1666;
    // std::cout << "Testing Data\n"
    //           << "============\n"
    //           << "a: " << a << "\n"
    //           << "b: " << b << "\n";

    ABYmath abymath;

    pack p = abymath.read_parameter(role, address, port, seclvl, bitlen, nvals, nthreads, mt_alg, S_BOOL);

    double test; 
    // abymath.aby_pow(p, a, b, 6);
    // abymath.aby_ceil(p, a);
    // abymath.aby_floor(p, a);
    // abymath.aby_abs(p, a);
    // abymath.aby_exp(p, a);
    // abymath.aby_sqrt(p, a);
    // abymath.aby_sin(p, a);
    // abymath.aby_cos(p, a);
    // abymath.aby_tan(p, a);
    // abymath.aby_asin(p, a);
    // abymath.aby_acos(p, a);
    test = abymath.aby_atan(p, -343.532);
    std::cout << test << std::endl;
    // abymath.aby_atan2(p, a);
    // abymath.aby_sinh(p, a);
    // abymath.aby_cosh(p, a);
    // abymath.aby_tanh(p, a);
    // abymath.aby_cabs(p, a, b);
    // abymath.aby_ldexp(p, a, b);
    // abymath.aby_fmod(p, a, b);
    // abymath.aby_modf(p, a, &b);

    // std::cout << "test return: " << test << "\n";

	return 0;
}

