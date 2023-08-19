#include "MPC_math.h"

pack ABYmath::read_parameter(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
    uint64_t bitlen, uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing){
    
    pack r;
    r.role = role;
    r.address = address;
    r.port = port;
    r.seclevel = seclvl;
    r.bitlen = bitlen;
    r.nvals = nvals;
    r.nthreads = nthreads;
    r.mt_alg = mt_alg;
    r.sharing = sharing;

    return r;
}

double ABYmath::aby_pow(pack p, double base, double exp, int precision = 6){
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

    // build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
	uint64_t* basefptr = (uint64_t*) &base, *expfptr = (uint64_t*) &exp;
    share* s_base = circ->PutINGate(basefptr, 64, SERVER);
    share* s_exp = circ->PutINGate(expfptr, 64, CLIENT);

    // 2^3 = e^(3ln(2))
    share* s_log2 = circ->PutFPGate(s_base, LOG2);
    s_log2->set_bitlength(64);
	share* s_mul = circ->PutFPGate(s_exp, s_log2, MUL);
	share* s_exp2 = circ->PutFPGate(s_mul, EXP2);
    s_exp2->set_bitlength(64);

    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_exp2, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint64_t* pow_val = (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) pow_val);

	delete party;

    // for debug
#ifdef DEBUG_MPCMATH
    std::cout << "Testing Pow() in " << get_sharing_name(sharing) << " sharing: " << std::endl;
    std::cout << std::fixed;
    std::cout << "CORRECT: " << std::setprecision(precision) << pow(base, exp) << std::endl;
    std::cout << "POW RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_ceil(pack p, double input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    double intpart = (double) (int64_t) input;
    double one = 1;
    uint64_t* inputfptr = (uint64_t*) &input;
    uint64_t* intpartfptr = (uint64_t*) &intpart;
    uint64_t* onefptr = (uint64_t*) &one;
    share* s_in = circ->PutINGate(inputfptr, 64, SERVER);
    share* s_intpart = circ->PutINGate(intpartfptr, 64, CLIENT);
    share* s_one = circ->PutINGate(onefptr, 64, SERVER);

    // compare float's integer part
    share* s_cmp_1 = circ->PutFPGate(s_intpart, s_in, CMP);
    share* s_cmp_2 = circ->PutFPGate(s_in, s_intpart, CMP);
    share* s_gt = s_intpart;
    share* s_le = circ->PutFPGate(s_intpart, s_one, ADD);
    share* s_mux = circ->PutMUXGate(s_gt, s_le, s_cmp_1);
    s_mux = circ->PutMUXGate(s_le, s_intpart, s_cmp_2);

    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_mux, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint64_t* ceil_val= (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) ceil_val);

	delete party;

    // for debug
#ifdef DEBUG_MPCMATH
    std::cout << "Testing Ceil() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << ceil(input) << std::endl;
    std::cout << "CEIL RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_floor(pack p, double input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    double intpart = (double) (int64_t) input;
    double one = 1;
    uint64_t* inputfptr = (uint64_t*) &input;
    uint64_t* intpartfptr = (uint64_t*) &intpart;
    uint64_t* onefptr = (uint64_t*) &one;
    share* s_in = circ->PutINGate(inputfptr, 64, SERVER);
    share* s_intpart = circ->PutINGate(intpartfptr, 64, CLIENT);
    share* s_one = circ->PutINGate(onefptr, 64, SERVER);

    // compare float's integer part
    share* s_cmp_1 = circ->PutFPGate(s_intpart, s_in, CMP);
    share* s_cmp_2 = circ->PutFPGate(s_in, s_intpart, CMP);
    share* s_gt = circ->PutFPGate(s_intpart, s_one, SUB);
    share* s_le = s_intpart;
    share* s_res = circ->PutMUXGate(s_gt, s_le, s_cmp_1);
    s_res = circ->PutMUXGate(s_intpart, s_gt, s_cmp_2);

    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_res, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint64_t* floor_val = (uint64_t*) s_res->get_clear_value_ptr();
    double res = *((double*) floor_val);

	delete party;

    // for debug
#ifdef DEBUG_MPCMATH
    std::cout << "Testing Floor() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << floor(input) << std::endl;
    std::cout << "Floor RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_abs(pack p, double input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    double zero = 0;
    uint64_t* inputfptr = (uint64_t*) &input;
    uint64_t* zerofptr = (uint64_t*) &zero;
    share* s_in = circ->PutINGate(inputfptr, 64, SERVER);
    share* s_zero = circ->PutINGate(zerofptr, 64, CLIENT);

    share* s_cmp = circ->PutFPGate(s_in, s_zero, CMP);    
    share* s_neg = circ->PutFPGate(s_zero, s_in, SUB);
    share* s_mux = circ->PutMUXGate(s_in, s_neg, s_cmp);

    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_mux, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint64_t* abs_val = (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) abs_val);

	delete party;

    // for debug
#ifdef DEBUG_MPCMATH
    std::cout << "Testing FABS() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << fabs(input) << std::endl;
    std::cout << "FABS RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_exp(pack p, float input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    uint32_t* inputfptr = (uint32_t*) &input;
    share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
    
    share* s_exp = circ->PutFPGate(s_in, EXP);

    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_exp, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint32_t* exp_val = (uint32_t*) s_out->get_clear_value_ptr();
    float res = *((float*) exp_val);

	delete party;

    // for debug
#ifdef DEBUG_MPCMATH
    std::cout << "Testing EXP() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << exp(input) << std::endl;
    std::cout << "EXP RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_sqrt(pack p, double input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    uint64_t* inputfptr = (uint64_t*) &input;
    share* s_in = circ->PutINGate(inputfptr, 64, SERVER);
    
    share* s_sqrt = circ->PutFPGate(s_in, SQRT);
    
    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_sqrt, ALL);

    party->ExecCircuit();

    uint64_t* sqrt_val = (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) sqrt_val);

	delete party;

    // for debug
#ifdef DEBUG_MPCMATH
    std::cout << "Testing SQRT() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << sqrt(input) << std::endl;
    std::cout << "SQRT RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_sin(pack p, float input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    float input_tran = input/M_PI;
    uint32_t* inputfptr = (uint32_t*) &input_tran;
    share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
    
    // s_in = 1/6 => 30 degree(how many pi)
    share* s_sin = circ->PutFPGate(s_in, SIN);

    //PutOUTGate
    share* s_out = circ->PutOUTGate(s_sin, ALL);

    party->ExecCircuit();

    uint32_t* sin_val = (uint32_t*) s_out->get_clear_value_ptr();
    float res = *((float*) sin_val);

	delete party;

    // for debug
#ifdef DEBUG_MPCMATH
    std::cout << "Testing SIN() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    // sin(input), input is radian
    std::cout << "CORRECT: " << sin(input) << std::endl;
    std::cout << "SIN RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_cos(pack p, float input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    float input_tran = input/M_PI;
    uint32_t* inputfptr = (uint32_t*) &input_tran;
    share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
    
    // s_in = 1/6 => 30 degree(how many pi)
    share* s_cos = circ->PutFPGate(s_in, COS);

    // PutOUTGate    
    share* s_out = circ->PutOUTGate(s_cos, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint32_t* cos_val = (uint32_t*) s_cos->get_clear_value_ptr();
    float res = *((float*) cos_val);

	delete party;

    // for debug
#ifdef DEBUG_MPCMATH    
    std::cout << "Testing COS() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    // sin(input), input is radian
    std::cout << "CORRECT: " << cos(input) << std::endl;
    std::cout << "SIN RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_tan(pack p, float input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    float input_tran = input/M_PI;
    uint32_t* inputfptr = (uint32_t*) &input_tran;
    share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
    
    // s_in = 1/6 => 30 degree(how many pi)
    share* s_sin = circ->PutFPGate(s_in, SIN);
    s_sin->set_bitlength(32);
    share* s_cos = circ->PutFPGate(s_in, COS);
    s_cos->set_bitlength(32);
    share* s_div = circ->PutFPGate(s_sin, s_cos, DIV);
    
    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_div, ALL); 

    // execute
    party->ExecCircuit();

    // get output
    uint32_t* tan_val = (uint32_t*) s_out->get_clear_value_ptr();
    float res = *((float*) tan_val);

	delete party;

    // for debug
#ifdef DEBUG_MPCMATH
    std::cout << "Testing TAN() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    // sin(input), input is radian
    std::cout << "CORRECT: " << tan(input) << std::endl;
    std::cout << "TAN RES: " << res << std::endl;
#endif

	return res;
}

float ABYmath::aby_asin(pack p, float input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;
    
    // judge range
    if(input > 1 || input < -1){
#ifdef DEBUG_MPCMATH
        std::cout << "Testing ASIN() in Bool sharing: " << std::endl;
        std::cout << "CORRECT: " << asin(input) << std::endl;
        std::cout << "ASIN RES: nan" << std::endl;
#endif
        double nanValue = std::numeric_limits<double>::quiet_NaN();
        return nanValue;
    }

    // mark for negative number
    int neg = 0;
    if(input < 0){
        input = -input;
        neg = 1;
    }

    // prepare
    float low = 0.0;
    float high = M_PI / 2;
    float mid, mid_tran;
    float epsilon = 1e-4;
    float cmp, cmp_loop = 1;
    float zero = 0, one = 1, two = 2, pi = M_PI;
    int cnt = 0, spec_1 = 0, spec_2 = 0;
    uint32_t* highfptr = (uint32_t*) &high;
    uint32_t* lowfptr = (uint32_t*) &low;
    uint32_t* epsilonfptr = (uint32_t*) &epsilon;
    uint32_t* inputfptr = (uint32_t*) &input;
    uint32_t* onefptr = (uint32_t*) &one;
    uint32_t* zerofptr = (uint32_t*) &zero;
    uint32_t* twofptr = (uint32_t*) &two;
    uint32_t* pifptr = (uint32_t*) &pi;
    uint32_t* cmp_t, *cmp_loop_t, *mid_t, *spec_t;

    // cmp_loop only show 1 or 0 for condition
    // max iteration is 30 
    while (cmp_loop && cnt <= 30) {
        // build circuit
        ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
        std::vector<Sharing*>& sharings = party->GetSharings();
        BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();
        
        // PutINGate
        share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
        share* s_two = circ->PutINGate(twofptr, 32, CLIENT);
        share* s_one = circ->PutINGate(onefptr, 32, SERVER);
        share* s_zero = circ->PutINGate(zerofptr, 32, CLIENT);
        share* s_high = circ->PutINGate(highfptr, 32, SERVER);
        share* s_low = circ->PutINGate(lowfptr, 32, CLIENT);
        share* s_epsilon = circ->PutINGate(epsilonfptr, 32, SERVER);
        share* s_pi = circ->PutINGate(pifptr, 32, CLIENT);
        
        // in == 0
        share* s_spec_1 = circ->PutFPGate(s_in, s_zero, CMP);
        share* s_spec_2 = circ->PutFPGate(s_zero, s_in, CMP);
        s_spec_1 = circ->PutMUXGate(s_one, s_zero, s_spec_1);
        s_spec_2 = circ->PutMUXGate(s_one, s_zero, s_spec_2);
        
        // mid = (low + high) / 2 / M_PI
        share* s_add = circ->PutFPGate(s_low, s_high, ADD);
        share* s_mid_r = circ->PutFPGate(s_add, s_two, DIV);
        share* s_mid = circ->PutFPGate(s_mid_r, s_pi, DIV);

        // s_cmp = sin(mid) > x
        share* s_sin = circ->PutFPGate(s_mid, SIN);
        s_sin->set_bitlength(32);
        share* s_cmp = circ->PutFPGate(s_sin, s_in, CMP);
        s_cmp = circ->PutMUXGate(s_one, s_zero, s_cmp);

        // s_cmp_loop = (high - low) > epsilon
        share* s_sub = circ->PutFPGate(s_high, s_low, SUB);
        share* s_cmp_loop = circ->PutFPGate(s_sub, s_epsilon, CMP);
        s_cmp_loop = circ->PutMUXGate(s_one, s_zero, s_cmp_loop);
       
        // PutOUTGate
        s_spec_1 = circ->PutOUTGate(s_spec_1, ALL);
        s_spec_2 = circ->PutOUTGate(s_spec_2, ALL);
        s_mid_r = circ->PutOUTGate(s_mid_r, ALL);
        s_cmp = circ->PutOUTGate(s_cmp, ALL);
        s_cmp_loop = circ->PutOUTGate(s_cmp_loop, ALL);
        
        // execute
        party->ExecCircuit();

        // get output
        spec_t = (uint32_t*) s_spec_1->get_clear_value_ptr();
        spec_1 = (int) *((float*) spec_t);
        spec_t = (uint32_t*) s_spec_2->get_clear_value_ptr();
        spec_2 = (int) *((float*) spec_t);
        cmp_t = (uint32_t*) s_cmp->get_clear_value_ptr();
        cmp =* ((float*) cmp_t);
        mid_t = (uint32_t*) s_mid_r->get_clear_value_ptr();
        mid =* ((float*) mid_t);
        
        // judge for special case
        if(spec_1 == 0 && spec_2 == 0){
            mid = 0;
            delete party;
            break;
        }

        // prevent float point problem
        while(mid > high || mid < low){
            mid = (low + high) / 2;
            cmp = -1;
        }

        // modify bounds
        if(cmp == 1){
            high = mid;
        }else if(cmp == 0){
            low = mid;
        }

        cnt++;
        delete party;
    }

#ifdef DEBUG_MPCMATH
    std::cout << "Testing ASIN() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    // sin(input), input is radian
    input = neg ? -input : input;
    std::cout << "CORRECT: " << asin(input) << std::endl;
    std::cout << "ASIN RES: " << (mid = (neg) ? (-mid) : mid) << std::endl;
#endif

	return (mid = (neg) ? (-mid) : mid);
}

float ABYmath::aby_acos(pack p, float input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;
    
    // judge range
    if(input > 1 || input < -1){
#ifdef DEBUG_MPCMATH
        std::cout << "Testing ACOS() in Bool sharing: " << std::endl;
        std::cout << "CORRECT: " << acos(input) << std::endl;
        std::cout << "ACOS RES: nan" << std::endl;
#endif
        double nanValue = std::numeric_limits<double>::quiet_NaN();
        return nanValue;
    }

    // mark for negative number
    int neg = 0;
    if(input < 0){
        input = -input;
        neg = 1;
    }

    // prepare
    float low = 0.0;
    float high = M_PI / 2;
    float mid, mid_tran;
    float epsilon = 1e-4;
    float cmp, cmp_loop = 1;
    float zero = 0, one = 1, two = 2, pi = M_PI;
    int cnt = 0, spec_1 = 0, spec_2 = 0;
    uint32_t* highfptr = (uint32_t*) &high;
    uint32_t* lowfptr = (uint32_t*) &low;
    uint32_t* epsilonfptr = (uint32_t*) &epsilon;
    uint32_t* inputfptr = (uint32_t*) &input;
    uint32_t* onefptr = (uint32_t*) &one;
    uint32_t* zerofptr = (uint32_t*) &zero;
    uint32_t* twofptr = (uint32_t*) &two;
    uint32_t* pifptr = (uint32_t*) &pi;
    uint32_t* cmp_t, *cmp_loop_t, *mid_t, *spec_t;

    // cmp_loop only show 1 or 0 for condition
    // max iteration is 30 
    while (cmp_loop && cnt <= 30) {
        // build circuit
        ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
        std::vector<Sharing*>& sharings = party->GetSharings();
        BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();
        
        // PutINGate
        share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
        share* s_two = circ->PutINGate(twofptr, 32, CLIENT);
        share* s_one = circ->PutINGate(onefptr, 32, SERVER);
        share* s_zero = circ->PutINGate(zerofptr, 32, CLIENT);
        share* s_high = circ->PutINGate(highfptr, 32, SERVER);
        share* s_low = circ->PutINGate(lowfptr, 32, CLIENT);
        share* s_epsilon = circ->PutINGate(epsilonfptr, 32, SERVER);
        share* s_pi = circ->PutINGate(pifptr, 32, CLIENT);
        
        // in == 1
        share* s_spec_1 = circ->PutFPGate(s_in, s_one, CMP);
        share* s_spec_2 = circ->PutFPGate(s_one, s_in, CMP);
        s_spec_1 = circ->PutMUXGate(s_one, s_zero, s_spec_1);
        s_spec_2 = circ->PutMUXGate(s_one, s_zero, s_spec_2);

        // mid = (low + high) / 2 / M_PI
        share* s_add = circ->PutFPGate(s_low, s_high, ADD);
        share* s_mid_r = circ->PutFPGate(s_add, s_two, DIV);
        share* s_mid = circ->PutFPGate(s_mid_r, s_pi, DIV);

        // s_cmp = cos(mid) > x
        share* s_cos = circ->PutFPGate(s_mid, COS);
        s_cos->set_bitlength(32);
        share* s_cmp = circ->PutFPGate(s_cos, s_in, CMP);
        s_cmp = circ->PutMUXGate(s_one, s_zero, s_cmp);

        // s_cmp_loop = (high - low) > epsilon
        share* s_sub = circ->PutFPGate(s_high, s_low, SUB);
        share* s_cmp_loop = circ->PutFPGate(s_sub, s_epsilon, CMP);
        s_cmp_loop = circ->PutMUXGate(s_one, s_zero, s_cmp_loop);
        
        // PutOUTGate
        s_spec_1 = circ->PutOUTGate(s_spec_1, ALL);
        s_spec_2 = circ->PutOUTGate(s_spec_2, ALL);
        s_mid_r = circ->PutOUTGate(s_mid_r, ALL);
        s_cmp = circ->PutOUTGate(s_cmp, ALL);
        s_cmp_loop = circ->PutOUTGate(s_cmp_loop, ALL);
        
        // execute
        party->ExecCircuit();

        // get output
        spec_t = (uint32_t*) s_spec_1->get_clear_value_ptr();
        spec_1 = (int) *((float*) spec_t);
        spec_t = (uint32_t*) s_spec_2->get_clear_value_ptr();
        spec_2 = (int) *((float*) spec_t);
        cmp_t = (uint32_t*) s_cmp->get_clear_value_ptr();
        cmp =* ((float*) cmp_t);
        mid_t = (uint32_t*) s_mid_r->get_clear_value_ptr();
        mid =* ((float*) mid_t);

        // judge for special case
        if(spec_1 == 0 && spec_2 == 0){
            mid = 0;
            delete party;
            break;
        }

        // prevent float point problem
        while(mid > high || mid < low){
            mid = (low + high) / 2;
            cmp = -1;
        }

        // modify bounds
        if(cmp == 1){
            low = mid;
        }else if(cmp == 0){
            high = mid;
        }

        cnt++;
        delete party;
    }

#ifdef DEBUG_MPCMATH    
    std::cout << "Testing ACOS() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    input = neg ? -input : input;
    std::cout << "CORRECT: " << acos(input) << std::endl;
    std::cout << "ACOS RES: " << (mid = (neg) ? (M_PI - mid) : mid) << std::endl;
#endif

	return (mid = (neg) ? (M_PI - mid) : mid);
}

float ABYmath::aby_atan(pack p, float input) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

    // this function just for 0 ~ 1/2 PI, temporarily.
    int neg = 0;
    if(input < 0){
        input = -input;
        neg = 1;
    }

    // prepare
    float low = 0.0;
    float high = M_PI / 2;
    float mid, mid_tran;
    float epsilon = 1e-4;
    float cmp, cmp_loop = 1;
    float zero = 0, one = 1, two = 2, pi = M_PI;
    int cnt = 0, spec_1 = 0, spec_2 = 0;
    uint32_t* highfptr = (uint32_t*) &high;
    uint32_t* midfptr = (uint32_t*) &mid;
    uint32_t* lowfptr = (uint32_t*) &low;
    uint32_t* epsilonfptr = (uint32_t*) &epsilon;
    uint32_t* inputfptr = (uint32_t*) &input;
    uint32_t* zerofptr = (uint32_t*) &zero;
    uint32_t* onefptr = (uint32_t*) &one;
    uint32_t* twofptr = (uint32_t*) &two;
    uint32_t* pifptr = (uint32_t*) &pi;
    uint32_t* cmp_t, *cmp_loop_t, *mid_t, *spec_t;
    
    // cmp_loop only show 1 or 0 for condition
    // max iteration is 30 
    while (cmp_loop && cnt <= 30) {
        // build circuit
        ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
        std::vector<Sharing*>& sharings = party->GetSharings();
        BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();
        
        // PutINGate
        share* s_in = circ->PutINGate(inputfptr, 32, SERVER);
        share* s_two = circ->PutINGate(twofptr, 32, CLIENT);
        share* s_one = circ->PutINGate(onefptr, 32, SERVER);
        share* s_zero = circ->PutINGate(zerofptr, 32, CLIENT);
        share* s_high = circ->PutINGate(highfptr, 32, SERVER);
        share* s_low = circ->PutINGate(lowfptr, 32, CLIENT);
        share* s_epsilon = circ->PutINGate(epsilonfptr, 32, SERVER);
        share* s_pi = circ->PutINGate(pifptr, 32, CLIENT);
        
        // in == 0
        share* s_spec_1 = circ->PutFPGate(s_in, s_zero, CMP);
        share* s_spec_2 = circ->PutFPGate(s_zero, s_in, CMP);
        s_spec_1 = circ->PutMUXGate(s_one, s_zero, s_spec_1);
        s_spec_2 = circ->PutMUXGate(s_one, s_zero, s_spec_2);

        
        // mid = (low + high) / 2 / M_PI
        share* s_add = circ->PutFPGate(s_low, s_high, ADD);
        share* s_mid_r = circ->PutFPGate(s_add, s_two, DIV);
        share* s_mid = circ->PutFPGate(s_mid_r, s_pi, DIV);

        // s_cmp = tan(mid) > x
        share* s_sin = circ->PutFPGate(s_mid, SIN);
        s_sin->set_bitlength(32);
        share* s_cos = circ->PutFPGate(s_mid, COS);
        s_cos->set_bitlength(32);
        share* s_tan = circ->PutFPGate(s_sin, s_cos, DIV);
        share* s_cmp = circ->PutFPGate(s_tan, s_in, CMP);
        s_cmp = circ->PutMUXGate(s_one, s_zero, s_cmp);

        // s_cmp_loop = (high - low) > epsilon
        share* s_sub = circ->PutFPGate(s_high, s_low, SUB);
        share* s_cmp_loop = circ->PutFPGate(s_sub, s_epsilon, CMP);
        s_cmp_loop = circ->PutMUXGate(s_one, s_zero, s_cmp_loop);
        
        // PutOUTGate
        s_spec_1 = circ->PutOUTGate(s_spec_1, ALL);
        s_spec_2 = circ->PutOUTGate(s_spec_2, ALL);
        s_mid_r = circ->PutOUTGate(s_mid_r, ALL);
        s_cmp = circ->PutOUTGate(s_cmp, ALL);
        s_cmp_loop = circ->PutOUTGate(s_cmp_loop, ALL);

        // execute
        party->ExecCircuit();

        // get output
        spec_t = (uint32_t*) s_spec_1->get_clear_value_ptr();
        spec_1 = (int) *((float*) spec_t);
        spec_t = (uint32_t*) s_spec_2->get_clear_value_ptr();
        spec_2 = (int) *((float*) spec_t);
        cmp_t = (uint32_t*) s_cmp->get_clear_value_ptr();
        cmp =* ((float*) cmp_t);
        mid_t = (uint32_t*) s_mid_r->get_clear_value_ptr();
        mid =* ((float*) mid_t);
        
        // judge for special case
        if(spec_1 == 0 && spec_2 == 0){
            std::cout << "!!!!\n";
            mid = 0;
            delete party;
            break;
        }

        // prevent float point problem
        while(mid > high || mid < low){
            mid = (low + high) / 2;
            cmp = -1;
        }

        // modify bounds
        if(cmp == 1){
            high = mid;
        }else if(cmp == 0){
            low = mid;
        }

        cnt++;
        delete party;
    }

#ifdef DEBUG_MPCMATH    
    std::cout << "Testing ATAN() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    input = neg ? -input : input;
    std::cout << "CORRECT: " << atan(input) << std::endl;
    std::cout << "ATAN RES: " << (mid = (neg) ? -mid : mid) << std::endl;
#endif

	return (mid = (neg) ? -mid : mid);
}

double ABYmath::aby_atan2(pack p, float a, float b) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

    float input = a/b;
    float res = 0;

    if(b > 0){
        res = ABYmath::aby_atan(p, input);
    }else if(b == 0){
        if(a > 0){
            res = M_PI / 2;
            // std::cout << "CORRECT: " << atan2(a, b) << std::endl;
            // std::cout << "ATAN RES: " << M_PI / 2 << std::endl;
        }else if(a < 0){
            res = -M_PI / 2;
            // std::cout << "CORRECT: " << atan2(a, b) << std::endl;
            // std::cout << "ATAN RES: " << -M_PI / 2 << std::endl;
        }else{
            res = 0;
            std::cout << "UNDEFINED\n";
        }
    }else{
        if(a >= 0){
            res = ABYmath::aby_atan(p, input) + M_PI;
        }else{
            res = ABYmath::aby_atan(p, input) - M_PI;
        }
    }

	return res;
}

float ABYmath::aby_sinh(pack p, float x) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    float two = 2, negx = -x;
    uint32_t* twofptr = (uint32_t*) &two;
    uint32_t* xfptr = (uint32_t*) &x;
    uint32_t* negxfptr = (uint32_t*) &negx;
    share* s_x = circ->PutINGate(xfptr, 32, SERVER);
    share* s_x_neg = circ->PutINGate(negxfptr, 32, SERVER);
    share* s_two = circ->PutINGate(twofptr, 32, CLIENT);

    share* s_exp_pos = circ->PutFPGate(s_x, EXP);
    s_exp_pos->set_bitlength(32);
    share* s_exp_neg = circ->PutFPGate(s_x_neg, EXP);
    s_exp_neg->set_bitlength(32);
    share* s_add = circ->PutFPGate(s_exp_pos, s_exp_neg, SUB);
    share* s_div = circ->PutFPGate(s_add, s_two, DIV);

    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_div, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint32_t* sinh_val = (uint32_t*) s_out->get_clear_value_ptr();
    float res = *((float*) sinh_val);

	delete party;
    
#ifdef DEBUG_MPCMATH
    std::cout << "Testing SINH() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << sinh(x) << std::endl;
    std::cout << "SINH RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_cosh(pack p, float x) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    float two = 2, negx = -x;
    uint32_t* twofptr = (uint32_t*) &two;
    uint32_t* xfptr = (uint32_t*) &x;
    uint32_t* negxfptr = (uint32_t*) &negx;
    share* s_x = circ->PutINGate(xfptr, 32, SERVER);
    share* s_x_neg = circ->PutINGate(negxfptr, 32, SERVER);
    share* s_two = circ->PutINGate(twofptr, 32, CLIENT);

    share* s_exp_pos = circ->PutFPGate(s_x, EXP);
    s_exp_pos->set_bitlength(32);
    share* s_exp_neg = circ->PutFPGate(s_x_neg, EXP);
    s_exp_neg->set_bitlength(32);
    share* s_add = circ->PutFPGate(s_exp_pos, s_exp_neg, ADD);
    share* s_div = circ->PutFPGate(s_add, s_two, DIV);

    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_div, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint32_t* cosh_val = (uint32_t*) s_out->get_clear_value_ptr();
    float res = *((float*) cosh_val);

	delete party;

#ifdef DEBUG_MPCMATH
    std::cout << "Testing COSH() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << cosh(x) << std::endl;
    std::cout << "COSH RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_tanh(pack p, float x) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    float two = 2, one = 1;
    uint32_t* twofptr = (uint32_t*) &two;
    uint32_t* onefptr = (uint32_t*) &one;
    uint32_t* xfptr = (uint32_t*) &x;
    share* s_x = circ->PutINGate(xfptr, 32, SERVER);
    share* s_one = circ->PutINGate(onefptr, 32, SERVER);
    share* s_two = circ->PutINGate(twofptr, 32, CLIENT);

    share* s_2x = circ->PutFPGate(s_two, s_x, MUL);
    share* s_exp = circ->PutFPGate(s_2x, EXP);
    s_exp->set_bitlength(32);
    share* s_sub = circ->PutFPGate(s_exp, s_one, SUB);
    share* s_add = circ->PutFPGate(s_exp, s_one, ADD);
    share* s_div = circ->PutFPGate(s_sub, s_add, DIV);

    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_div, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint32_t* tanh_val = (uint32_t*) s_out->get_clear_value_ptr();
    float res = *((float*) tanh_val);

	delete party;

#ifdef DEBUG_MPCMATH
    std::cout << "Testing TANH() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << tanh(x) << std::endl;
    std::cout << "TANH RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_ldexp(pack p, double x, double exp) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    double two = 2;
    uint64_t* xfptr = (uint64_t*) &x, *expfptr = (uint64_t*) &exp;
    uint64_t* twofptr = (uint64_t*) &two;
    share* s_x = circ->PutINGate(xfptr, 64, SERVER);
    share* s_exp = circ->PutINGate(expfptr, 64, CLIENT);
    share* s_two = circ->PutINGate(twofptr, 64, SERVER);
    
    // 2^3 = e^(3ln(2))
	share* log2_res = circ->PutFPGate(s_two, LOG2);
    log2_res->set_bitlength(64);
	share* mul_res = circ->PutFPGate(s_exp, log2_res, MUL);
	share* exp2_res = circ->PutFPGate(mul_res, EXP2);
    exp2_res->set_bitlength(64);
	share* ldexp_res = circ->PutFPGate(s_x, exp2_res, MUL);
    
    // PutOUTGate
    share* s_out = circ->PutOUTGate(ldexp_res, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint64_t* ldexp_val = (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) ldexp_val);

	delete party;
    
#ifdef DEBUG_MPCMATH
    std::cout << "Testing LDEXP() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << ldexp(x, exp) << std::endl;
    std::cout << "LDEXP RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_cabs(pack p, double a, double b) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    uint64_t* afptr = (uint64_t*) &a, *bfptr = (uint64_t*) &b;
    share* s_ain = circ->PutINGate(afptr, 64, SERVER);
    share* s_bin = circ->PutINGate(bfptr, 64, CLIENT);

    share* s_asqr = circ->PutFPGate(s_ain, s_ain, MUL);
    share* s_bsqr = circ->PutFPGate(s_bin, s_bin, MUL);
    share* s_add = circ->PutFPGate(s_asqr, s_bsqr, ADD);
    share* s_sqrt = circ->PutFPGate(s_add, SQRT);

    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_sqrt, ALL);

    party->ExecCircuit();

    uint64_t* cabs_val = (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) cabs_val);

	delete party;

#ifdef DEBUG_MPCMATH
    std::cout << "Testing CABS() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    // double complex c = 3.01 + 4.02*  I;
    // std::cout << "CORRECT: " << cabs(c) << std::endl;
    std::cout << "CABS RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_fmod(pack p, double a, double b) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // judge for special case
    if(b == 0){
        std::cout << "Testing FMOD() in " << get_sharing_name(sharing)
        << " sharing: " << std::endl;
        std::cout << "CORRECT: " << fmod(a, b) << std::endl;
        std::cout << "FMOD RES: 0" << std::endl;        
        return 0;
    }

    // PutINGate
    double q = (double) (int64_t) (a / b);
    double zero = 0, one = 1;
    uint64_t* qiptr = (uint64_t*) &q;
    uint64_t* zerofptr = (uint64_t*) &zero, *onefptr = (uint64_t*) &one;
    uint64_t* afptr = (uint64_t*) &a, *bfptr = (uint64_t*) &b;
    share* s_a = circ->PutINGate(afptr, 64, SERVER);
    share* s_b = circ->PutINGate(bfptr, 64, CLIENT);
    share* s_zero = circ->PutINGate(zerofptr, 64, SERVER);
    share* s_one = circ->PutINGate(onefptr, 64, CLIENT);
    share* s_q_intpart = circ->PutINGate(qiptr, 64, SERVER);

    share* s_mul = circ->PutFPGate(s_q_intpart, s_b, MUL);
    share* s_sub = circ->PutFPGate(s_a, s_mul, SUB);
    
    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_sub, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint64_t* _val = (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) _val);

	delete party;

#ifdef DEBUG_MPCMATH
    std::cout << "Testing FMOD() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << fmod(a, b) << std::endl;
    std::cout << "FMOD RES: " << res << std::endl;
#endif

	return res;
}

double ABYmath::aby_modf(pack p, double a, double* intptr) {
    // get parameters
    e_role role = p.role;
    std::string address = p.address;
    uint16_t port = p.port;
    seclvl seclvl = p.seclevel;
    uint64_t bitlen = p.bitlen, nvals = p.nvals, nthreads = p.nthreads;
    e_mt_gen_alg mt_alg = p.mt_alg;
    e_sharing sharing = p.sharing;

	// build circuit
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();  

    // PutINGate
    double intpart = (double) (int64_t) a;
    uint64_t* intpartptr = (uint64_t*) &intpart;
    uint64_t* afptr = (uint64_t*) &a;
    share* s_a = circ->PutINGate(afptr, 64, SERVER);
    share* s_intpart = circ->PutINGate(intpartptr, 64, CLIENT);

    share* s_sub = circ->PutFPGate(s_a, s_intpart, SUB);
    
    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_sub, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint64_t* _val = (uint64_t*) s_out->get_clear_value_ptr();
    double res = *((double*) _val);
    *intptr = intpart;

	delete party;

#ifdef DEBUG_MPCMATH
    std::cout << "Testing MODF() in " << get_sharing_name(sharing)
            << " sharing: " << std::endl;
    std::cout << "CORRECT: " << modf(a, intptr) << std::endl;
    std::cout << "MODF RES: " << res << std::endl;
#endif

	return res;
}