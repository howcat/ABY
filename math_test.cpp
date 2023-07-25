/**
 \file 		millionaire_prob_test.cpp
 \author	sreeram.sadasivam@cased.de
 \copyright	ABY - A Framework for Efficient Mixed-protocol Secure Two-party Computation
			Copyright (C) 2019 Engineering Cryptographic Protocols Group, TU Darmstadt
			This program is free software: you can redistribute it and/or modify
            it under the terms of the GNU Lesser General Public License as published
            by the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.
            ABY is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
            GNU Lesser General Public License for more details.
            You should have received a copy of the GNU Lesser General Public License
            along with this program. If not, see <http://www.gnu.org/licenses/>.
 \brief		Millionaire problem Test class implementation.
 */

//Utility libs
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <math.h>
#include <cassert>
//ABY Party class
#include "../../abycore/aby/abyparty.h"
#include "../../abycore/circuit/booleancircuits.h"
#include "../../abycore/sharing/sharing.h"
#include "../../abycore/circuit/arithmeticcircuits.h"
#include "../../abycore/circuit/circuit.h"

int32_t read_test_options(int32_t* argcp, char*** argvp, e_role* role,
		uint32_t* bitlen, uint32_t* nvals, uint32_t* secparam, std::string* address,
		uint16_t* port, int32_t* test_op, double* base, double* exp) {

	uint32_t int_role = 0, int_port = 0;

	parsing_ctx options[] =
			{ { (void*) &int_role, T_NUM, "r", "Role: 0/1", true, false },
              { (void*) nvals, T_NUM, "n", "Number of parallel operation elements", false, false }, 
              { (void*) bitlen, T_NUM, "b", "Bit-length, default 32", false, false }, 
              { (void*) secparam, T_NUM, "s", "Symmetric Security Bits, default: 128", false, false }, 
              { (void*) address, T_STR, "a", "IP-address, default: localhost", false, false }, 
              { (void*) &int_port, T_NUM, "p", "Port, default: 7766", false, false }, 
              { (void*) test_op, T_NUM, "t", "Single test (leave out for all operations), default: off", false, false }, 
              { (void*) base, T_DOUBLE, "x", "Base, default: 0.0", false, false }, 
              { (void*) exp, T_DOUBLE, "y", "Exponenet, default: 0.0", false, false } };

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

int64_t aby_pow(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double base, double exp) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

	uint64_t *basefptr = (uint64_t*) &base, *expfptr = (uint64_t*) &exp;
    
    share* s_base = circ->PutINGate(basefptr, 64, SERVER);
    share* s_exp = circ->PutINGate(expfptr, 64, CLIENT);
	
    // 2^3 = e^(3ln(2))
	share* log2_res = circ->PutFPGate(s_base, LOG2);
	share* mul_res = circ->PutFPGate(s_exp, log2_res, MUL);
	share* exp2_res = circ->PutFPGate(mul_res, EXP2);

    share* s_out = circ->PutOUTGate(exp2_res, ALL);
    share* s_base_out = circ->PutOUTGate(s_base, ALL);
    share* s_exp_out = circ->PutOUTGate(s_exp, ALL);

    party->ExecCircuit();

    uint64_t *base_out_vals = (uint64_t*) s_base_out->get_clear_value_ptr();
    double base_val = *((double*) base_out_vals);
    uint64_t *exp_out_vals = (uint64_t*) s_exp_out->get_clear_value_ptr();
    double exp_val = *((double*) exp_out_vals);
    uint64_t *pow_out_vals = (uint64_t*) s_out->get_clear_value_ptr();
    double pow_val = *((double*) pow_out_vals);

    std::cout << "Testing Pow() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << pow(base_val, exp_val) << std::endl;
    std::cout << "POW RES: " << pow_val << std::endl;

	delete party;

	return 0;
}

int64_t aby_ceil(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double input) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    double intpart = (double) (uint64_t) input;
    uint64_t *inputfptr = (uint64_t*) &input;
    share* s_in = circ->PutINGate(inputfptr, 64, SERVER);
    uint64_t *intpartfptr = (uint64_t*) &intpart;
    share* s_intpart = circ->PutINGate(intpartfptr, 64, CLIENT);

    share* s_cmp = circ->PutFPGate(s_in, s_intpart, CMP, bitlen, nvals, no_status);
    s_cmp = circ->PutOUTGate(s_cmp, ALL);

    party->ExecCircuit();

    uint32_t res = s_cmp->get_clear_value<uint32_t>();

    std::cout << "Testing Ceil() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << ceil(input) << std::endl;
    std::cout << "CEIL RES: " << (long int)input + res << std::endl;

	delete party;

	return 0;
}

int64_t aby_floor(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double input) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    double intpart = (double) (uint64_t) input;
    uint64_t *inputfptr = (uint64_t*) &input;
    share* s_in = circ->PutINGate(inputfptr, 64, SERVER);
    uint64_t *intpartfptr = (uint64_t*) &intpart;
    share* s_intpart = circ->PutINGate(intpartfptr, 64, CLIENT);

    share* s_cmp = circ->PutFPGate(s_in, s_intpart, CMP, bitlen, nvals, no_status);
    s_cmp = circ->PutOUTGate(s_cmp, ALL);

    party->ExecCircuit();

    uint32_t res = s_cmp->get_clear_value<uint32_t>();
    res = (res == 1 || input == 0)? res = 0 : res = 1;

    std::cout << "Testing Floor() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << floor(input) << std::endl;
    std::cout << "Floor RES: " << (long int)input - res << std::endl;

	delete party;

	return 0;
}

int64_t aby_abs(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double input) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    uint64_t *inputfptr = (uint64_t*) &input;
    share* s_in = circ->PutINGate(inputfptr, 64, SERVER);
    double zero = 0;
    uint64_t *zerofptr = (uint64_t*) &zero;
    share* s_zero = circ->PutINGate(zerofptr, 64, CLIENT);

    share* s_cmp = circ->PutFPGate(s_in, s_zero, CMP, bitlen, nvals, no_status);    
    share* s_neg = circ->PutFPGate(s_zero, s_in, SUB, bitlen, nvals, no_status);
    share* s_res = circ->PutMUXGate(s_in, s_neg, s_cmp);
    s_res = circ->PutOUTGate(s_res, ALL);

    party->ExecCircuit();

    uint64_t *res_t = (uint64_t*) s_res->get_clear_value_ptr();
    double res = *((double*) res_t);
    
    std::cout << "Testing FABS() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << fabs(input) << std::endl;
    std::cout << "FABS RES: " << res << std::endl;

	delete party;

	return 0;
}

int64_t aby_exp(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, float input) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    uint32_t *inputfptr = (uint32_t*) &input;
    share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
    share* s_res = circ->PutFPGate(s_in, EXP, 32, 1, no_status);
    s_res = circ->PutOUTGate(s_res, ALL);

    party->ExecCircuit();

    uint32_t *res_t = (uint32_t*) s_res->get_clear_value_ptr();
    float res = *((float*) res_t);
    
    std::cout << "Testing EXP() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << exp(input) << std::endl;
    std::cout << "EXP RES: " << res << std::endl;

	delete party;

	return 0;
}

int64_t aby_sqrt(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double input) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    uint64_t *inputfptr = (uint64_t*) &input;
    share* s_in = circ->PutINGate(inputfptr, 64, SERVER);
    share* s_res = circ->PutFPGate(s_in, SQRT, 64, 1, no_status);
    s_res = circ->PutOUTGate(s_res, ALL);

    party->ExecCircuit();

    uint64_t *res_t = (uint64_t*) s_res->get_clear_value_ptr();
    double res = *((double*) res_t);
    
    std::cout << "Testing SQRT() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << sqrt(input) << std::endl;
    std::cout << "SQRT RES: " << res << std::endl;

	delete party;

	return 0;
}

int64_t aby_sin(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, float input) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    float input_tran = input/M_PI;
    uint32_t *inputfptr = (uint32_t*) &input_tran;
    share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
    // s_in = 1/6 => 30 degree(how many pi)
    share* s_res = circ->PutFPGate(s_in, SIN, 32, 1, no_status);
    s_res = circ->PutOUTGate(s_res, ALL);

    party->ExecCircuit();

    uint32_t *res_t = (uint32_t*) s_res->get_clear_value_ptr();
    float res = *((float*) res_t);
    
    std::cout << "Testing SIN() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    // sin(input), input is radian
    std::cout << "CORRECT: " << sin(input) << std::endl;
    std::cout << "SIN RES: " << res << std::endl;

	delete party;

	return 0;
}

int64_t aby_cos(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, float input) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    float input_tran = input/M_PI;
    uint32_t *inputfptr = (uint32_t*) &input_tran;
    share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
    // s_in = 1/6 => 30 degree(how many pi)
    share* s_res = circ->PutFPGate(s_in, COS, 32, 1, no_status);
    s_res = circ->PutOUTGate(s_res, ALL);

    party->ExecCircuit();

    uint32_t *res_t = (uint32_t*) s_res->get_clear_value_ptr();
    float res = *((float*) res_t);
    
    std::cout << "Testing COS() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    // sin(input), input is radian
    std::cout << "CORRECT: " << cos(input) << std::endl;
    std::cout << "SIN RES: " << res << std::endl;

	delete party;

	return 0;
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


    a = 1.515832, b = -10.0685;
    // aby_pow(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, a, b);
    // aby_ceil(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, a);
    // aby_floor(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, a);
    // aby_abs(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, a);
    // aby_exp(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, (float)a);
    // aby_sqrt(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, a);
    // aby_sin(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, a);
    aby_cos(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, a);

    // not finished
    // aby_tan(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, a);
    // aby_ldexp(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, a, b);

	return 0;
}

// int64_t aby_tan(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
// 		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, float input) {
//     uint64_t bitlen = 64;

// 	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
// 			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

// 	std::vector<Sharing*>& sharings = party->GetSharings();

// 	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

//     float input_tran = input/M_PI;
//     uint32_t *inputfptr = (uint32_t*) &input_tran;
//     share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
//     // s_in = 1/6 => 30 degree(how many pi)
//     share* s_sin = circ->PutFPGate(s_in, SIN, 32, 1, no_status);
//     share* s_cos = circ->PutFPGate(s_in, COS, 32, 1, no_status);
//     share* s_res = circ->PutFPGate(s_sin, s_cos, DIV, 32, 1, no_status);
//     s_res = circ->PutOUTGate(s_res, ALL);

//     party->ExecCircuit();

//     uint32_t *res_t = (uint32_t*) s_res->get_clear_value_ptr();
//     float res = *((float*) res_t);
    
//     std::cout << "Testing TAN() in " << get_sharing_name(sharing)
//             << " sharing: " << std::endl;
//     // sin(input), input is radian
//     std::cout << "CORRECT: " << tan(input) << std::endl;
//     std::cout << "TAN RES: " << res << std::endl;

// 	delete party;

// 	return 0;
// }

// int64_t aby_ldexp(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
// 		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double x, double exp) {
//     uint64_t bitlen = 64;

// 	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
// 			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

// 	std::vector<Sharing*>& sharings = party->GetSharings();

// 	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

//     double two = 2;
//     uint64_t *xfptr = (uint64_t*) &x, *expfptr = (uint64_t*) &exp, *twofptr = (uint64_t*) &two;
    
//     share* s_x = circ->PutINGate(xfptr, 64, SERVER);
//     share* s_exp = circ->PutINGate(expfptr, 64, CLIENT);
//     share* s_two = circ->PutINGate(twofptr, 64, ALL);

//     // 2^3 = e^(3ln(2))
// 	share* log2_res = circ->PutFPGate(s_two, LOG2);
// 	share* mul_res = circ->PutFPGate(s_exp, log2_res, MUL);
// 	share* exp2_res = circ->PutFPGate(mul_res, EXP2);
// 	share* ldexp_res = circ->PutFPGate(s_x, exp2_res, MUL);

//     share* s_out = circ->PutOUTGate(ldexp_res, ALL);
//     share* s_base_out = circ->PutOUTGate(s_x, ALL);
//     share* s_exp_out = circ->PutOUTGate(s_exp, ALL);

//     party->ExecCircuit();

//     uint64_t *res_t = (uint64_t*) s_out->get_clear_value_ptr();
//     double res = *((double*) res_t);
    
//     std::cout << "Testing TAN() in " << get_sharing_name(sharing)
//             << " sharing: " << std::endl;
//     // sin(input), input is radian
//     std::cout << "CORRECT: " << ldexp(x, exp) << std::endl;
//     std::cout << "TAN RES: " << res << std::endl;

// 	delete party;

// 	return 0;
// }