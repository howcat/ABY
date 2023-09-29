#include <iostream>
#include <cmath>
#include "MPC_math.h"

int32_t read_test_options(int32_t* argcp, char*** argvp, e_role* role,
		uint32_t* bitlen, uint32_t* nvals, uint32_t* secparam, std::string* address,
		uint16_t* port, int32_t* test_op, double* a, double* b) {

	uint32_t int_role = 0, int_port = 0;

	parsing_ctx options[] =
			{ { (void*) &int_role, T_NUM, "r", "Role: 0/1", true, false }, 
              {	(void*) nvals, T_NUM, "n", "Number of parallel operation elements", false, false }, 
              { (void*) bitlen, T_NUM, "b", "Bit-length, default 32", false, false }, 
              { (void*) secparam, T_NUM, "s", "Symmetric Security Bits, default: 128", false, false }, 
              { (void*) address, T_STR, "a", "IP-address, default: localhost", false, false }, 
              { (void*) &int_port, T_NUM, "p", "Port, default: 7766", false, false }, 
              { (void*) test_op, T_NUM, "t", "Single test (leave out for all operations), default: off", false, false },
              { (void*) a, T_DOUBLE, "i", "Your Income", true, false },
              { (void*) b, T_DOUBLE, "h", "Your Credit History Score (0 ~ 2000)", true, false },
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

bool calculateCreditScore(e_role role, const std::string& address, uint16_t port, seclvl seclvl, 
        uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, std::string circuit_dir, 
        double income, double hist_score) {

    ABYmath abymath;

    ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg, 100000, circuit_dir);
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    share* s_income_a, *s_income_b, *s_hist_score_a, *s_hist_score_b;
    uint64_t* incomefptr = (uint64_t*)& income;
    uint64_t* hist_scorefptr = (uint64_t*)& hist_score;

    double coef_1 = 0.6, coef_2 = 600, coef_3 = 100, coef_4 = 700, coef_5 = 2;
    uint64_t* coef_1fptr = (uint64_t*)&coef_1;
    uint64_t* coef_2fptr = (uint64_t*)&coef_2;
    uint64_t* coef_3fptr = (uint64_t*)&coef_3;
    uint64_t* coef_4fptr = (uint64_t*)&coef_4;
    uint64_t* coef_5fptr = (uint64_t*)&coef_5;
    share* s_coef_1 = circ->PutCONSGate(*coef_1fptr, 64);
    share* s_coef_2 = circ->PutCONSGate(*coef_2fptr, 64);
    share* s_coef_3 = circ->PutCONSGate(*coef_3fptr, 64);
    share* s_coef_4 = circ->PutCONSGate(*coef_4fptr, 64);
    share* s_coef_5 = circ->PutCONSGate(*coef_5fptr, 64);

	if(role == SERVER) {
		s_income_a = circ->PutDummyINGate(bitlen);
		s_hist_score_a = circ->PutDummyINGate(bitlen);
		s_income_b = circ->PutINGate(incomefptr, bitlen, SERVER);
		s_hist_score_b = circ->PutINGate(hist_scorefptr, bitlen, SERVER);
	}else{ //role == CLIENT
		s_income_a = circ->PutINGate(incomefptr, bitlen, CLIENT);
		s_hist_score_a = circ->PutINGate(hist_scorefptr, bitlen, CLIENT);
		s_income_b = circ->PutDummyINGate(bitlen);
		s_hist_score_b = circ->PutDummyINGate(bitlen);
	}
    
    share* s_score_a, *s_score_b;
    share* s_pow_a = abymath.aby_pow(party, circ, s_income_a, s_coef_1);
    share* s_sub_a = circ->PutFPGate(s_hist_score_a, s_coef_2, SUB);
    share* s_fabs_a = abymath.aby_abs(party, circ, s_sub_a);
    share* s_mul_a = circ->PutFPGate(s_pow_a, s_fabs_a, MUL);
    s_score_a = circ->PutFPGate(s_mul_a, s_coef_3, DIV);
    share* s_pow_b = abymath.aby_pow(party, circ, s_income_b, s_coef_1);
    share* s_sub_b = circ->PutFPGate(s_hist_score_b, s_coef_2, SUB);
    share* s_fabs_b = abymath.aby_abs(party, circ, s_sub_b);
    share* s_mul_b = circ->PutFPGate(s_pow_b, s_fabs_b, MUL);
    s_score_b = circ->PutFPGate(s_mul_b, s_coef_3, DIV);
    
    share* s_floor_b = abymath.aby_floor(party, circ, s_score_b);
    share* s_ceil_a = abymath.aby_ceil(party, circ, s_score_a);
    share* s_add_1 = circ->PutFPGate(s_floor_b, s_ceil_a, ADD);
    share* s_add_2 = circ->PutFPGate(s_score_a, s_score_b, ADD);
    share* s_div = circ->PutFPGate(s_add_2, s_coef_5, DIV);
    share* s_sin = abymath.aby_sin_64(party, circ, s_div);
    share* s_abs = abymath.aby_abs(party, circ, s_sin);
    share* s_joint_score = circ->PutFPGate(s_add_1, s_abs, DIV);

    share* s_cmp = circ->PutFPGate(s_joint_score, s_coef_4, CMP);
    uint64_t zero = 0, one = 1;
    share* s_zero = circ->PutCONSGate(zero, 64);
    share* s_one = circ->PutCONSGate(one, 64);
    share* s_res = circ->PutMUXGate(s_one, s_zero, s_cmp);
 
    share* res = circ->PutOUTGate(s_res, ALL);

    party->ExecCircuit();

    uint64_t ans = res->get_clear_value<uint64_t>();

    if(ans)
        return true;
    else
        return false;
}

int main(int argc, char** argv) {
	e_role role;
	uint32_t bitlen = 64, nvals = 4, secparam = 128, nthreads = 1;
	uint16_t port = 7766;
	std::string address = "127.0.0.1";
	std::string circuit_dir = "/home/howcat/ABY/bin/circ/";
	int32_t test_op = -1;
	e_mt_gen_alg mt_alg = MT_OT;
    double income = 0, hist_score = 0;

	read_test_options(&argc, &argv, &role, &bitlen, &nvals, &secparam, &address, &port, &test_op, &income, &hist_score);

	seclvl seclvl = get_sec_lvl(secparam);

    bool r = calculateCreditScore(role, address, port, seclvl, 64, nthreads, mt_alg, S_BOOL, circuit_dir, income, hist_score);

    if (r) {
        std::cout << "Loan Approved!" << std::endl;
    } else {
        std::cout << "Loan Denied." << std::endl;
    }

    return 0;
}