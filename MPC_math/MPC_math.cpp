#include "MPC_math.h"
#include <iomanip>

int64_t ABYmath::aby_pow(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
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
    log2_res->set_bitlength(64);
	share* mul_res = circ->PutFPGate(s_exp, log2_res, MUL);
	share* exp2_res = circ->PutFPGate(mul_res, EXP2);
    exp2_res->set_bitlength(64);

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

int64_t ABYmath::aby_ceil(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double input) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    double intpart = (double) (int64_t) input;
    double one = 1;
    uint64_t *inputfptr = (uint64_t*) &input;
    uint64_t *onefptr = (uint64_t*) &one;
    share* s_in = circ->PutINGate(inputfptr, 64, SERVER);
    uint64_t *intpartfptr = (uint64_t*) &intpart;
    share* s_intpart = circ->PutINGate(intpartfptr, 64, CLIENT);
    share* s_one = circ->PutINGate(onefptr, 64, SERVER);

    share* s_cmp_1 = circ->PutFPGate(s_intpart, s_in, CMP);
    share* s_cmp_2 = circ->PutFPGate(s_in, s_intpart, CMP);
    share* s_gt = s_intpart;
    share* s_le = circ->PutFPGate(s_intpart, s_one, ADD);
    share* s_res = circ->PutMUXGate(s_gt, s_le, s_cmp_1);
    s_res = circ->PutMUXGate(s_le, s_intpart, s_cmp_2);

    s_res = circ->PutOUTGate(s_res, ALL);

    party->ExecCircuit();

    uint64_t *res_t = (uint64_t*) s_res->get_clear_value_ptr();
    double res = *((double*) res_t);

    std::cout << "Testing Ceil() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << ceil(input) << std::endl;
    std::cout << "CEIL RES: " << res << std::endl;

	delete party;

	return 0;
}

int64_t ABYmath::aby_floor(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
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

int64_t ABYmath::aby_abs(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
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

int64_t ABYmath::aby_exp(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
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

int64_t ABYmath::aby_sqrt(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
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

int64_t ABYmath::aby_sin(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
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

int64_t ABYmath::aby_cos(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
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

int64_t ABYmath::aby_tan(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
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
    share* s_sin = circ->PutFPGate(s_in, SIN);
    s_sin->set_bitlength(32);
    share* s_cos = circ->PutFPGate(s_in, COS);
    s_cos->set_bitlength(32);
    share* s_res = circ->PutFPGate(s_sin, s_cos, DIV, 32, 1, no_status);
    s_res = circ->PutOUTGate(s_res, ALL); 

    party->ExecCircuit();

    uint32_t *res_t = (uint32_t*) s_res->get_clear_value_ptr();
    float res = *((float*) res_t);
    
    std::cout << "Testing TAN() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    // sin(input), input is radian
    std::cout << "CORRECT: " << tan(input) << std::endl;
    std::cout << "TAN RES: " << res << std::endl;

	delete party;

	return 0;
}

int64_t ABYmath::aby_acos(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, float input) {
    uint64_t bitlen = 64;

    // this function just for 0 ~ 1/2 PI, temporarily.
    int neg = 0;
    if(input < 0){
        input = -input;
        neg = 1;
    }

    float low = 0.0;
    float high = M_PI / 2;
    float mid, mid_tran;
    float epsilon = 1e-4;
    float one = 1, zero = 0, two = 2, pi = M_PI;
    uint32_t *onefptr = (uint32_t*) &one;
    uint32_t *zerofptr = (uint32_t*) &zero;
    uint32_t *highfptr = (uint32_t*) &high;
    uint32_t *lowfptr = (uint32_t*) &low;
    uint32_t *epsilonfptr = (uint32_t*) &epsilon;
    uint32_t *inputfptr = (uint32_t*) &input;
    uint32_t *twofptr = (uint32_t*) &two;
    uint32_t *pifptr = (uint32_t*) &pi;
    uint32_t *cmp_t, *cmp_loop_t, *mid_t;
    float cmp, cmp_loop = 1;
    int cnt = 0;

    while (cmp_loop && cnt <= 30) {
        ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
        std::vector<Sharing*>& sharings = party->GetSharings();
        BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();
        
        share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
        share* s_two = circ->PutINGate(twofptr, 32, CLIENT);
        share* s_one = circ->PutINGate(onefptr, 32, SERVER);
        share* s_zero = circ->PutINGate(zerofptr, 32, CLIENT);
        share* s_high = circ->PutINGate(highfptr, 32, SERVER);
        share* s_low = circ->PutINGate(lowfptr, 32, CLIENT);
        share* s_epsilon = circ->PutINGate(epsilonfptr, 32, SERVER);
        share* s_pi = circ->PutINGate(pifptr, 32, CLIENT);
        
        // mid = (low + high) / 2 / M_PI
        share* s_add = circ->PutFPGate(s_low, s_high, ADD);
        share* s_mid_r = circ->PutFPGate(s_add, s_two, DIV);
        share* s_mid = circ->PutFPGate(s_mid_r, s_pi, DIV);
        s_add = circ->PutOUTGate(s_add, ALL);
        s_mid_r = circ->PutOUTGate(s_mid_r, ALL);

        // s_cmp = cos(mid) > x
        share* s_cos = circ->PutFPGate(s_mid, COS);
        s_cos->set_bitlength(32);
        share* s_cmp = circ->PutFPGate(s_cos, s_in, CMP);
        // s_cmp->set_bitlength(32);
        s_cmp = circ->PutMUXGate(s_one, s_zero, s_cmp);
        s_cmp = circ->PutOUTGate(s_cmp, ALL);

        // s_cmp_loop = (high - low) > epsilon
        share* s_sub = circ->PutFPGate(s_high, s_low, SUB);
        share* s_cmp_loop = circ->PutFPGate(s_sub, s_epsilon, CMP);
        s_cmp_loop = circ->PutMUXGate(s_one, s_zero, s_cmp_loop);
        s_cmp_loop = circ->PutOUTGate(s_cmp_loop, ALL);
        
        party->ExecCircuit();

        cmp_t = (uint32_t*) s_cmp->get_clear_value_ptr();
        cmp = *((float*) cmp_t);
        mid_t = (uint32_t*) s_mid_r->get_clear_value_ptr();
        mid = *((float*) mid_t);

        // prevent float point problem
        if(abs(mid - low) > 10 || abs(mid - high) > 10){
            mid = (low + high) / 2;
            continue;
            // delete party;
            // break;
        }

        if(cmp == 1){
            low = mid;
        }else{
            high = mid;
        }

        cnt++;
        delete party;
    }
    
    std::cout << "Testing ACOS() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    // sin(input), input is radian
    input = neg ? -input : input;
    std::cout << "CORRECT: " << acos(input) << std::endl;
    std::cout << "ACOS RES: " << (mid = (neg) ? (M_PI - mid) : mid) << std::endl;

	return 0;
}

int64_t ABYmath::aby_cosh(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, float x) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    float two = 2, negx = -x;
    uint32_t *twofptr = (uint32_t*) &two;
    uint32_t *xfptr = (uint32_t*) &x;
    uint32_t *negxfptr = (uint32_t*) &negx;

    share *s_x = circ->PutINGate(xfptr, 32, SERVER);
    share *s_x_neg = circ->PutINGate(negxfptr, 32, SERVER);
    share *s_two = circ->PutINGate(twofptr, 32, CLIENT);

    share *s_exp_pos = circ->PutFPGate(s_x, EXP);
    s_exp_pos->set_bitlength(32);
    share *s_exp_neg = circ->PutFPGate(s_x_neg, EXP);
    s_exp_neg->set_bitlength(32);
    share *s_add = circ->PutFPGate(s_exp_pos, s_exp_neg, ADD);
    share *s_div = circ->PutFPGate(s_add, s_two, DIV);

    share *s_out = circ->PutOUTGate(s_div, ALL);

    party->ExecCircuit();

    uint32_t *res_t = (uint32_t*) s_out->get_clear_value_ptr();
    float res = *((float*) res_t);
    
    std::cout << "Testing COSH() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << cosh(x) << std::endl;
    std::cout << "COSH RES: " << res << std::endl;
	delete party;

	return 0;
}

int64_t ABYmath::aby_ldexp(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double x, double exp) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    double two = 2;
    uint64_t *xfptr = (uint64_t*) &x, *expfptr = (uint64_t*) &exp;
    uint64_t *twofptr = (uint64_t*) &two;
    
    share* s_x = circ->PutINGate(xfptr, 64, SERVER);
    share* s_exp = circ->PutINGate(expfptr, 64, CLIENT);
    share* s_two = circ->PutINGate(twofptr, 64, SERVER);
    // circ->PutPrintValueGate(s_two, "s_two: ");

    // 2^3 = e^(3ln(2))
	share* log2_res = circ->PutFPGate(s_two, LOG2);
    log2_res->set_bitlength(64);
	share* mul_res = circ->PutFPGate(s_exp, log2_res, MUL);
	share* exp2_res = circ->PutFPGate(mul_res, EXP2);
    exp2_res->set_bitlength(64);
	share* ldexp_res = circ->PutFPGate(s_x, exp2_res, MUL);
    share* s_out = circ->PutOUTGate(ldexp_res, ALL);

    party->ExecCircuit();

    uint64_t *res_t = (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) res_t);
    
    std::cout << "Testing LDEXP() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << ldexp(x, exp) << std::endl;
    std::cout << "LDEXP RES: " << res << std::endl;

	delete party;

	return 0;
}

int64_t ABYmath::aby_cabs(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double a, double b) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    uint64_t *afptr = (uint64_t*) &a, *bfptr = (uint64_t*) &b;
    share* s_ain = circ->PutINGate(afptr, 64, SERVER);
    share* s_bin = circ->PutINGate(bfptr, 64, CLIENT);

    share* s_asqr = circ->PutFPGate(s_ain, s_ain, MUL);
    share* s_bsqr = circ->PutFPGate(s_bin, s_bin, MUL);
    share* s_add = circ->PutFPGate(s_asqr, s_bsqr, ADD);
    share* s_res = circ->PutFPGate(s_add, SQRT);

    s_res = circ->PutOUTGate(s_res, ALL);

    party->ExecCircuit();

    uint64_t *res_t = (uint64_t*) s_res->get_clear_value_ptr();
    double res = *((double*) res_t);
    
    std::cout << "Testing CABS() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    // double complex c = 3.01 + 4.02 * I;
    // std::cout << "CORRECT: " << cabs(c) << std::endl;
    std::cout << "CABS RES: " << res << std::endl;

	delete party;

	return 0;
}

int64_t ABYmath::aby_fmod(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double a, double b) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    if(b == 0){
        std::cout << "Testing FMOD() in " << get_sharing_name(sharing)
        << " sharing: " << std::endl;
        std::cout << "CORRECT: " << fmod(a, b) << std::endl;
        std::cout << "FMOD RES: 0" << std::endl;        
        return 0;
    }

    double q = (double) (int64_t) (a / b);
    uint64_t *qiptr = (uint64_t*) &q;
    double zero = 0, one = 1;
    uint64_t *zerofptr = (uint64_t*) &zero, *onefptr = (uint64_t*) &one;
    uint64_t *afptr = (uint64_t*) &a, *bfptr = (uint64_t*) &b;
    share *s_a = circ->PutINGate(afptr, 64, SERVER);
    share *s_b = circ->PutINGate(bfptr, 64, CLIENT);
    share *s_zero = circ->PutINGate(zerofptr, 64, SERVER);
    share *s_one = circ->PutINGate(onefptr, 64, CLIENT);
    share *s_q_intpart = circ->PutINGate(qiptr, 64, SERVER);
    share *s_mul = circ->PutFPGate(s_q_intpart, s_b, MUL);
    share *s_sub = circ->PutFPGate(s_a, s_mul, SUB);
    share *s_out = circ->PutOUTGate(s_sub, ALL);

    party->ExecCircuit();

    uint64_t *res_t = (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) res_t);
    
    std::cout << "Testing FMOD() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << fmod(a, b) << std::endl;
    std::cout << "FMOD RES: " << res << std::endl;

	delete party;

	return 0;
}

int64_t ABYmath::aby_modf(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double a, double *intptr) {
    uint64_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

    double intpart = (double) (int64_t) a;
    uint64_t *intpartptr = (uint64_t*) &intpart;
    uint64_t *afptr = (uint64_t*) &a;
    share *s_a = circ->PutINGate(afptr, 64, SERVER);
    share *s_intpart = circ->PutINGate(intpartptr, 64, CLIENT);

    share *s_sub = circ->PutFPGate(s_a, s_intpart, SUB);
    share *s_out = circ->PutOUTGate(s_sub, ALL);

    party->ExecCircuit();

    uint64_t *res_t = (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) res_t);
    *intptr = intpart;
    
    std::cout << "Testing MODF() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << modf(a, intptr) << std::endl;
    std::cout << "MODF RES: " << res << std::endl;

	delete party;

	return 0;
}