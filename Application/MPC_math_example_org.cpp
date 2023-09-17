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
    ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg, 100000, circuit_dir);
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    share* s_income_a, *s_income_b, *s_hist_score_a, *s_hist_score_b;
    uint64_t* incomefptr = (uint64_t*)& income;
    uint64_t* hist_scorefptr = (uint64_t*)& hist_score;

    double coef_1 = 0.6, coef_2 = 600, coef_3 = 100, coef_4 = 500, coef_5 = 2;
    double round = 2 * M_PIf64;
    uint64_t* coef_1fptr = (uint64_t*)&coef_1;
    uint64_t* coef_2fptr = (uint64_t*)&coef_2;
    uint64_t* coef_3fptr = (uint64_t*)&coef_3;
    uint64_t* coef_4fptr = (uint64_t*)&coef_4;
    uint64_t* coef_5fptr = (uint64_t*)&coef_5;
    uint64_t* roundfptr = (uint64_t*)& round;
    share* s_coef_1 = circ->PutCONSGate(*coef_1fptr, 64);
    share* s_coef_2 = circ->PutCONSGate(*coef_2fptr, 64);
    share* s_coef_3 = circ->PutCONSGate(*coef_3fptr, 64);
    share* s_coef_4 = circ->PutCONSGate(*coef_4fptr, 64);
    share* s_coef_5 = circ->PutCONSGate(*coef_5fptr, 64);
    share* s_r = circ->PutCONSGate(*roundfptr, 64);
    double zero_d = 0, one_d = 1;
    uint64_t* zerofptr = (uint64_t*) &zero_d;
    uint64_t* onefptr = (uint64_t*) &one_d;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_one = circ->PutCONSGate(*onefptr, 64);

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
    // share* s_pow_a = abymath.aby_pow(party, circ, s_income_a, s_coef_1);
    share* s_log2 = circ->PutFPGate(s_income_a, LOG2);
    s_log2->set_bitlength(64);
	share* s_mul = circ->PutFPGate(s_coef_1, s_log2, MUL);
	share* s_pow_a = circ->PutFPGate(s_mul, EXP2);
    s_pow_a->set_bitlength(64);
    share* s_sub_a = circ->PutFPGate(s_hist_score_a, s_coef_2, SUB);
    // share* s_fabs_a = abymath.aby_abs(party, circ, s_sub_a);
    share* s_cmp_a = circ->PutFPGate(s_sub_a, s_zero, CMP);    
    share* s_neg = circ->PutFPGate(s_zero, s_sub_a, SUB);
    share* s_fabs_a = circ->PutMUXGate(s_sub_a, s_neg, s_cmp_a);
    share* s_mul_a = circ->PutFPGate(s_pow_a, s_fabs_a, MUL);
    s_score_a = circ->PutFPGate(s_mul_a, s_coef_3, DIV);
    // share* s_pow_b = abymath.aby_pow(party, circ, s_income_b, s_coef_1);
    s_log2 = circ->PutFPGate(s_income_b, LOG2);
    s_log2->set_bitlength(64);
	s_mul = circ->PutFPGate(s_coef_1, s_log2, MUL);
	share* s_pow_b = circ->PutFPGate(s_mul, EXP2);
    s_pow_b->set_bitlength(64);
    share* s_sub_b = circ->PutFPGate(s_hist_score_b, s_coef_2, SUB);
    // share* s_fabs_b = abymath.aby_abs(party, circ, s_sub_b);
    share* s_cmp_b = circ->PutFPGate(s_sub_b, s_zero, CMP);    
    s_neg = circ->PutFPGate(s_zero, s_sub_b, SUB);
    share* s_fabs_b = circ->PutMUXGate(s_sub_b, s_neg, s_cmp_b);
    share* s_mul_b = circ->PutFPGate(s_pow_b, s_fabs_b, MUL);
    s_score_b = circ->PutFPGate(s_mul_b, s_coef_3, DIV);
    // share* s_floor_b = abymath.aby_floor(party, circ, s_score_b);
    double toint = 1 / DBL_EPSILON;
    uint64_t* toifptr = (uint64_t*) &toint;
    share* s_toi = circ->PutCONSGate(*toifptr, 64);
    share* s_cmp_1 = circ->PutFPGate(s_zero, s_score_b, CMP);
    share* s_x = circ->PutFPGate(s_score_b, s_toi, SUB);
    s_x = circ->PutFPGate(s_x, s_toi, ADD);
    s_x = circ->PutFPGate(s_x, s_score_b, SUB);
    share* s_y = circ->PutFPGate(s_score_b, s_toi, ADD);
    s_y = circ->PutFPGate(s_y, s_toi, SUB);
    s_y = circ->PutFPGate(s_y, s_score_b, SUB);
    share* s_mux_1 = circ->PutMUXGate(s_x, s_y, s_cmp_1);
    share* s_cmp_2 = circ->PutFPGate(s_mux_1, s_zero, CMP);
    share* s_intpart = circ->PutFPGate(s_score_b, s_mux_1, ADD);
    s_x = circ->PutFPGate(s_intpart, s_one, SUB);
    s_y = circ->PutFPGate(s_intpart, s_zero, ADD);
    share* s_floor_b = circ->PutMUXGate(s_x, s_y, s_cmp_2);
    share* s_cmp_3 = circ->PutFPGate(s_zero, s_score_b, CMP);
    share* s_cmp_4 = circ->PutFPGate(s_score_b, s_zero, CMP);
    share* s_j = circ->PutORGate(s_cmp_3, s_cmp_4);
    s_floor_b = circ->PutMUXGate(s_floor_b, s_zero, s_j);
    // share* s_ceil_a = abymath.aby_ceil(party, circ, s_score_a);
    s_cmp_1 = circ->PutFPGate(s_zero, s_score_a, CMP);
    s_x = circ->PutFPGate(s_score_a, s_toi, SUB);
    s_x = circ->PutFPGate(s_x, s_toi, ADD);
    s_x = circ->PutFPGate(s_x, s_score_a, SUB);
    s_y = circ->PutFPGate(s_score_a, s_toi, ADD);
    s_y = circ->PutFPGate(s_y, s_toi, SUB);
    s_y = circ->PutFPGate(s_y, s_score_a, SUB);
    s_mux_1 = circ->PutMUXGate(s_x, s_y, s_cmp_1);
    s_cmp_2 = circ->PutFPGate(s_zero, s_mux_1, CMP);
    s_intpart = circ->PutFPGate(s_score_a, s_mux_1, ADD);
    s_x = circ->PutFPGate(s_intpart, s_one, ADD);
    s_y = circ->PutFPGate(s_intpart, s_zero, ADD);
    share* s_ceil_a = circ->PutMUXGate(s_x, s_y, s_cmp_2);
    share* s_cmp_5 = circ->PutFPGate(s_zero, s_score_a, CMP);
    share* s_cmp_6 = circ->PutFPGate(s_score_a, s_zero, CMP);
    s_j = circ->PutORGate(s_cmp_5, s_cmp_6);
    s_ceil_a = circ->PutMUXGate(s_ceil_a, s_zero, s_j);
    
    share* s_add_1 = circ->PutFPGate(s_floor_b, s_ceil_a, ADD);
    share* s_add_2 = circ->PutFPGate(s_score_a, s_score_b, ADD);
    share* s_div = circ->PutFPGate(s_add_2, s_coef_5, DIV);
    // share* s_sin = abymath.aby_sin(party, circ, s_div);
    // share* s_fmod = aby_fmod(party, circ, s_in, s_r);
    share* s_cmp_fmod = circ->PutFPGate(s_zero, s_div, CMP);
    share* s_div_fmod = circ->PutFPGate(s_div, s_r, DIV);
    // share* s_ceil_fmod = aby_ceil(party, circ, s_div_fmod);
    s_cmp_1 = circ->PutFPGate(s_zero, s_div_fmod, CMP);
    s_x = circ->PutFPGate(s_div_fmod, s_toi, SUB);
    s_x = circ->PutFPGate(s_x, s_toi, ADD);
    s_x = circ->PutFPGate(s_x, s_div_fmod, SUB);
    s_y = circ->PutFPGate(s_div_fmod, s_toi, ADD);
    s_y = circ->PutFPGate(s_y, s_toi, SUB);
    s_y = circ->PutFPGate(s_y, s_div_fmod, SUB);
    s_mux_1 = circ->PutMUXGate(s_x, s_y, s_cmp_1);
    s_cmp_2 = circ->PutFPGate(s_zero, s_mux_1, CMP);
    s_intpart = circ->PutFPGate(s_div_fmod, s_mux_1, ADD);
    s_x = circ->PutFPGate(s_intpart, s_one, ADD);
    s_y = circ->PutFPGate(s_intpart, s_zero, ADD);
    share* s_ceil_fmod = circ->PutMUXGate(s_x, s_y, s_cmp_2);
    share* s_cmp_7 = circ->PutFPGate(s_zero, s_div_fmod, CMP);
    share* s_cmp_8 = circ->PutFPGate(s_div_fmod, s_zero, CMP);
    s_j = circ->PutORGate(s_cmp_7, s_cmp_8);
    s_ceil_fmod = circ->PutMUXGate(s_ceil_fmod, s_zero, s_j);
    // share* s_floor_fmod = aby_floor(party, circ, s_div_fmod);
    s_cmp_1 = circ->PutFPGate(s_zero, s_div_fmod, CMP);
    s_x = circ->PutFPGate(s_div_fmod, s_toi, SUB);
    s_x = circ->PutFPGate(s_x, s_toi, ADD);
    s_x = circ->PutFPGate(s_x, s_div_fmod, SUB);
    s_y = circ->PutFPGate(s_div_fmod, s_toi, ADD);
    s_y = circ->PutFPGate(s_y, s_toi, SUB);
    s_y = circ->PutFPGate(s_y, s_div_fmod, SUB);
    s_mux_1 = circ->PutMUXGate(s_x, s_y, s_cmp_1);
    s_cmp_2 = circ->PutFPGate(s_mux_1, s_zero, CMP);
    s_intpart = circ->PutFPGate(s_div_fmod, s_mux_1, ADD);
    s_x = circ->PutFPGate(s_intpart, s_one, SUB);
    s_y = circ->PutFPGate(s_intpart, s_zero, ADD);
    share* s_floor_fmod = circ->PutMUXGate(s_x, s_y, s_cmp_2);
    share* s_cmp_9 = circ->PutFPGate(s_zero, s_div_fmod, CMP);
    share* s_cmp_10 = circ->PutFPGate(s_div_fmod, s_zero, CMP);
    s_j = circ->PutORGate(s_cmp_9, s_cmp_10);
    s_floor_fmod = circ->PutMUXGate(s_floor_fmod, s_zero, s_j);
    share* s_mux_fmod = circ->PutMUXGate(s_floor_fmod, s_ceil_fmod, s_cmp_fmod);
    share* s_mul_fmod = circ->PutFPGate(s_mux_fmod, s_r, MUL);
    share* s_fmod = circ->PutFPGate(s_div, s_mul_fmod, SUB);
    share* s_sin = s_fmod;
    share* s_pow_sin = s_fmod;
    share* s_fac_sin = s_one;
    share* s_temp_sin = s_one;
    // taylor series
    for(int i = 3; i < 17; i += 2) {
        // input^i
        s_pow_sin = circ->PutFPGate(s_pow_sin, s_fmod, MUL);
        s_pow_sin = circ->PutFPGate(s_pow_sin, s_fmod, MUL);

        // i!
        s_temp_sin = circ->PutFPGate(s_temp_sin, s_one, ADD);
        s_fac_sin = circ->PutFPGate(s_fac_sin, s_temp_sin, MUL);
        s_temp_sin = circ->PutFPGate(s_temp_sin, s_one, ADD);
        s_fac_sin = circ->PutFPGate(s_fac_sin, s_temp_sin, MUL);

        // pow / fac
        share* s_i = circ->PutFPGate(s_pow_sin, s_fac_sin, DIV);
        
        // -1
        if((i-1) / 2 % 2 == 0) {
            s_sin = circ->PutFPGate(s_sin, s_i, ADD);
        }else{
            s_sin = circ->PutFPGate(s_sin, s_i, SUB);
        }
    }
    // share* s_abs = abymath.aby_abs(party, circ, s_sin);
    share* s_cmp = circ->PutFPGate(s_sin, s_zero, CMP);    
    share* s_neg_sin = circ->PutFPGate(s_zero, s_sin, SUB);
    share* s_abs = circ->PutMUXGate(s_sin, s_neg_sin, s_cmp);
    share* s_joint_score = circ->PutFPGate(s_add_1, s_abs, DIV);
    share* s_cmp_bool = circ->PutFPGate(s_joint_score, s_coef_4, CMP);
    uint64_t zero = 0, one = 1;
    share* s_zero_ = circ->PutCONSGate(zero, 64);
    share* s_one_ = circ->PutCONSGate(one, 64);
    share* s_res = circ->PutMUXGate(s_one_, s_zero_, s_cmp_bool);

    share* res = circ->PutOUTGate(s_res, ALL);
    share* score = circ->PutOUTGate(s_joint_score, ALL);

    party->ExecCircuit();

    uint64_t ans = res->get_clear_value<uint64_t>();
    uint64_t output = score->get_clear_value<uint64_t>();
    std::cout << "joint score: " << *(double*)& output << "\n";

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