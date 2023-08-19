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

    std::ofstream outputFile_1("./verification_fiile/file_1.txt");
    std::ofstream outputFile_2("./verification_fiile/file_2.txt");
    if (!outputFile_1.is_open() || !outputFile_2.is_open()) {
        std::cerr << "無法開啟檔案。" << std::endl;
        return 1;
    }

    double lower_bound = -100;
    double upper_bound = 100;
    std::default_random_engine re;
    std::uniform_real_distribution<double> unif(lower_bound,upper_bound);

    for (int i = 0; i < 20; i++) {
        std::cout << "(" << i+1 << "/20)\n";
        double a_random_double = unif(re);
        // double a_random_double = -343.532;
        // file 1
        outputFile_1 << a_random_double << " " << sinh(a_random_double) << std::endl;
        // file
        ABYmath abymath;
        pack p = abymath.read_parameter(role, address, port, seclvl, bitlen, nvals, nthreads, mt_alg, S_BOOL);
        float test = abymath.aby_sinh(p, a_random_double);
        outputFile_2 << a_random_double << " " << test << std::endl;
    }

    outputFile_1.close();
    outputFile_2.close();

	return 0;
}

