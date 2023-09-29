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
	std::string circuit_dir = "/home/howcat/ABY/bin/circ/";
	int32_t test_op = -1;
	e_mt_gen_alg mt_alg = MT_OT;
    double a = 0, b = 0;

	read_test_options(&argc, &argv, &role, &bitlen, &nvals, &secparam, &address, &port, &test_op, &a, &b);
    
	seclvl seclvl = get_sec_lvl(secparam);

    std::ofstream outputFile_1("./verification_fiile/correct/tanh_0.txt");
    std::ofstream outputFile_2("./verification_fiile/aby/tanh_0.txt");
    if (!outputFile_1.is_open() || !outputFile_2.is_open()) {
        std::cerr << "無法開啟檔案。" << std::endl;
        return 1;
    }

    // double lower_bound = -1000000;
    // double upper_bound = 1000000;
    // std::default_random_engine re;
    // std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
    int upper_bound = 25, lower_bound = -25;
    srand(time(0));
    ABYmath abymath;
    ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg, 100000, circuit_dir);
    std::vector<Sharing*>& sharings = party->GetSharings();
    BooleanCircuit* circ = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();
    int times = 500;

    for (int k = 0; k < times; k++) {
        std::cout << "(" << k+1 << "/" << times <<")\n";
        int i = rand() % (upper_bound - lower_bound + 1) + lower_bound;
        int t = rand() % (upper_bound - lower_bound + 1) + lower_bound;
        double d_1 = (double)i + (double)t / (2*upper_bound);
        // double d_1 = fmod((double)i + (double)t / 2000, (2*M_PIf64));
        // float d_32 = (float)d/M_PIf32;
        i = rand() % (upper_bound - lower_bound + 1) + lower_bound;
        t = rand() % (upper_bound - lower_bound + 1) + lower_bound;
        double d_2 = (double)i + (double)t / (2*upper_bound);

        // correct file
        outputFile_1 << d_1
                    //  << "," << d_2
                    << " ";
        double correct = tanh(d_1);
        outputFile_1 << std::fixed << std::setprecision(6) << correct 
                    //  << "," << d_2 
                     << std::endl;

        // answer file
        uint64_t* aptr = (uint64_t*)& d_1;
        uint64_t* bptr = (uint64_t*)& d_2;
        share* input_a = circ->PutINGate(aptr, 64, SERVER);
        share* input_b = circ->PutINGate(bptr, 64, CLIENT);
        share* ans_64 = abymath.aby_tanh(party, circ, input_a);
        share* res_64 = circ->PutOUTGate(ans_64, ALL);
        share* res_64_2 = circ->PutOUTGate(input_b, ALL);
        party->ExecCircuit();
        uint64_t s_ans_64 = res_64->get_clear_value<uint64_t>();
        double answer_64 = *(double*)& s_ans_64;
        uint64_t s_ans_64_2 = res_64_2->get_clear_value<uint64_t>();
        double answer_64_2 = *(double*)& s_ans_64_2;
        party->Reset();
        outputFile_2 << d_1
                    //  << "," << d_2
                     << " " << std::fixed << std::setprecision(6) << answer_64
                    //  << "," << answer_64_2 
                     << std::endl;
        
        std::cout << "test data: " << d_1 << "," << d_2 << "\n"
                  << "correct  : " << correct << "," << d_2 << "\n"
                //   << "answer_32: " << answer_32 << "\n"
                  << "answer_64: " << answer_64 << "," << answer_64_2 << "\n"; 
    }

    outputFile_1.close();
    outputFile_2.close();

	return 0;
}

