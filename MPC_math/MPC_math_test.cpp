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

	read_test_options(&argc, &argv, &role, &bitlen, &nvals, &secparam, &address,
			&port, &test_op, &a, &b);
    
	seclvl seclvl = get_sec_lvl(secparam);

    ABYmath abymath;

    // testing data
    a = 5.1, b = -3;

    ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg, 100000, circuit_dir);
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();

    uint64_t* aptr = (uint64_t*)& a;
    uint64_t* bptr = (uint64_t*)& b;
    share* input_a = circ->PutINGate(aptr, 64, SERVER);
    share* input_b = circ->PutINGate(bptr, 64, CLIENT);
    
    // ==== testing function ====
    std::cout << "CORRECT: " << std::fixed  << std::setprecision(12) << fmod(14, -0.7) << "\n";
    // share* in = abymath.aby_test(party, circ, input_a);
    share* in = abymath.aby_fmod(party, circ, input_a, input_b);
    // ==========================
    
    share* res = circ->PutOUTGate(in, ALL);
    // share* intpart = circ->PutOUTGate(input_b, ALL);

    party->ExecCircuit();

    uint64_t s_ans = res->get_clear_value<uint64_t>();
    double ans = *(double*)& s_ans;
    // uint64_t s_intpart = intpart->get_clear_value<uint64_t>();
    // double ans_intpart = *(double*)& s_intpart;

    std::cout << "ANSWER: " << std::fixed  << std::setprecision(12) << ans << "\n";
    // std::cout << "INTPART: " << ans_intpart << "\n";

	return 0;
}

