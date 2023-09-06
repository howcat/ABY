#include "MPC_math.h"

share* ABYmath::aby_pow(ABYParty* party, BooleanCircuit* circ, share* s_base, share* s_exp){
    // 2^3 = e^(3ln(2))
    share* s_log2 = circ->PutFPGate(s_base, LOG2);
    s_log2->set_bitlength(64);
	share* s_mul = circ->PutFPGate(s_exp, s_log2, MUL);
	share* s_exp2 = circ->PutFPGate(s_mul, EXP2);
    s_exp2->set_bitlength(64);

	return s_exp2;
}

share* ABYmath::aby_ceil(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double toint = 1 / DBL_EPSILON;
    double zero = 0, one = 1;
    uint64_t* toifptr = (uint64_t*) &toint;
    uint64_t* zerofptr = (uint64_t*) &zero;
    uint64_t* onefptr = (uint64_t*) &one;
    share* s_toi = circ->PutCONSGate(*toifptr, 64);
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_one = circ->PutCONSGate(*onefptr, 64);

    if(s_in == s_zero) return s_zero;
    share* s_cmp_1 = circ->PutFPGate(s_zero, s_in, CMP);
    share* s_x = circ->PutFPGate(s_in, s_toi, SUB);
    s_x = circ->PutFPGate(s_x, s_toi, ADD);
    s_x = circ->PutFPGate(s_x, s_in, SUB);
    share* s_y = circ->PutFPGate(s_in, s_toi, ADD);
    s_y = circ->PutFPGate(s_y, s_toi, SUB);
    s_y = circ->PutFPGate(s_y, s_in, SUB);
    share* s_mux_1 = circ->PutMUXGate(s_x, s_y, s_cmp_1);
    share* s_cmp_2 = circ->PutFPGate(s_zero, s_mux_1, CMP);
    share* s_intpart = circ->PutFPGate(s_in, s_mux_1, ADD);
    s_x = circ->PutFPGate(s_intpart, s_one, ADD);
    s_y = circ->PutFPGate(s_intpart, s_zero, ADD);
    share* s_ans = circ->PutMUXGate(s_x, s_y, s_cmp_2);

	return s_ans;
}

share* ABYmath::aby_floor(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double toint = 1 / DBL_EPSILON;
    double zero = 0, one = 1;
    uint64_t* toifptr = (uint64_t*) &toint;
    uint64_t* zerofptr = (uint64_t*) &zero;
    uint64_t* onefptr = (uint64_t*) &one;
    share* s_toi = circ->PutCONSGate(*toifptr, 64);
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_one = circ->PutCONSGate(*onefptr, 64);

    if(s_in == s_zero) return s_zero;
    share* s_cmp_1 = circ->PutFPGate(s_zero, s_in, CMP);
    share* s_x = circ->PutFPGate(s_in, s_toi, SUB);
    s_x = circ->PutFPGate(s_x, s_toi, ADD);
    s_x = circ->PutFPGate(s_x, s_in, SUB);
    share* s_y = circ->PutFPGate(s_in, s_toi, ADD);
    s_y = circ->PutFPGate(s_y, s_toi, SUB);
    s_y = circ->PutFPGate(s_y, s_in, SUB);
    share* s_mux_1 = circ->PutMUXGate(s_x, s_y, s_cmp_1);
    share* s_cmp_2 = circ->PutFPGate(s_mux_1, s_zero, CMP);
    share* s_intpart = circ->PutFPGate(s_in, s_mux_1, ADD);
    s_x = circ->PutFPGate(s_intpart, s_one, SUB);
    s_y = circ->PutFPGate(s_intpart, s_zero, ADD);
    share* s_ans = circ->PutMUXGate(s_x, s_y, s_cmp_2);

	return s_ans;
}
share* ABYmath::aby_abs(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double zero = 0;
    uint64_t* zerofptr = (uint64_t*) &zero;
    share* s_zero = circ->PutSharedINGate(zerofptr, 64);

    share* s_cmp = circ->PutFPGate(s_in, s_zero, CMP);    
    share* s_neg = circ->PutFPGate(s_zero, s_in, SUB);
    share* s_mux = circ->PutMUXGate(s_in, s_neg, s_cmp);

	return s_mux;
}

share* ABYmath::aby_exp(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double one = 1.0;
    uint64_t* onefptr = (uint64_t*)& one;
    share* s_one = circ->PutCONSGate(*onefptr, 64);
    share* s_tyl = s_one;
    share* s_pow = s_one;
    share* s_fac = s_one;
    share* s_temp = s_one;

    uint64_t i = 2;
    for(int i = 1; i < 20; i++){
        // input^i
        s_pow = circ->PutFPGate(s_pow, s_in, MUL);

        // i!
        s_fac = circ->PutFPGate(s_fac, s_temp, MUL);
        s_temp = circ->PutFPGate(s_temp, s_one, ADD);

        // pow / fac
        share* s_i = circ->PutFPGate(s_pow, s_fac, DIV);

        s_tyl = circ->PutFPGate(s_tyl, s_i, ADD);
    }

	return s_tyl;
}

share* ABYmath::aby_sqrt(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    share* s_sqrt = circ->PutFPGate(s_in, SQRT);

	return s_sqrt;
}

share* ABYmath::aby_sin(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double one = 1.0;
    uint64_t* onefptr = (uint64_t*)& one;
    share* s_one = circ->PutCONSGate(*onefptr, 64);
    share* s_tyl = s_in;
    share* s_pow = s_in;
    share* s_fac = s_one;
    share* s_temp = s_one;

    // taylor series
    for(int i = 3; i < 17; i += 2) {
        // input^i
        s_pow = circ->PutFPGate(s_pow, s_in, MUL);
        s_pow = circ->PutFPGate(s_pow, s_in, MUL);

        // i!
        s_temp = circ->PutFPGate(s_temp, s_one, ADD);
        s_fac = circ->PutFPGate(s_fac, s_temp, MUL);
        s_temp = circ->PutFPGate(s_temp, s_one, ADD);
        s_fac = circ->PutFPGate(s_fac, s_temp, MUL);

        // pow / fac
        share* s_i = circ->PutFPGate(s_pow, s_fac, DIV);
        
        // -1
        if((i-1) / 2 % 2 == 0) {
            s_tyl = circ->PutFPGate(s_tyl, s_i, ADD);
        }else{
            s_tyl = circ->PutFPGate(s_tyl, s_i, SUB);
        }
    }

	return s_tyl;
}

share* ABYmath::aby_cos(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double zero = 0, one = 1.0;
    uint64_t* zerofptr = (uint64_t*)& zero;
    uint64_t* onefptr = (uint64_t*)& one;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_one = circ->PutCONSGate(*onefptr, 64);
    share* s_tyl = s_one;
    share* s_pow = s_one;
    share* s_fac = s_one;
    share* s_temp = s_zero;

    // taylor series
    for(int i = 2; i < 16; i += 2) {
        // input ^ i
        s_pow = circ->PutFPGate(s_pow, s_in, MUL);
        s_pow = circ->PutFPGate(s_pow, s_in, MUL);

        // i!
        s_temp = circ->PutFPGate(s_temp, s_one, ADD);
        s_fac = circ->PutFPGate(s_fac, s_temp, MUL);
        s_temp = circ->PutFPGate(s_temp, s_one, ADD);
        s_fac = circ->PutFPGate(s_fac, s_temp, MUL);

        // pow / fac
        share* s_i = circ->PutFPGate(s_pow, s_fac, DIV);
        
        // -1
        if(i / 2 % 2 == 0) {
            s_tyl = circ->PutFPGate(s_tyl, s_i, ADD);
        }else{
            s_tyl = circ->PutFPGate(s_tyl, s_i, SUB);
        }
    }

	return s_tyl;
}

share* ABYmath::aby_tan(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    share* s_sin = aby_sin(party, circ, s_in);
    share* s_cos = aby_cos(party, circ, s_in);
    share* s_div = circ->PutFPGate(s_sin, s_cos, DIV);

	return s_div;
}
/*
share* ABYmath::aby_asin(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // judge range
    // if(input > 1 || input < -1){
    //     double nanValue = std::numeric_limits<double>::quiet_NaN();
    //     return nanValue;
    // }

    // mark for negative number
    bool neg = false;
    double neg_one = -1.0;
    uint64_t* neg_onefptr = (uint64_t*)& neg_one;
    share* s_zero = circ->PutCONSGate((uint64_t)0, 64);
    share* s_neg_one = circ->PutCONSGate(*neg_onefptr, 64);
    share* s_cmp_neg = circ->PutFPGate(s_zero, s_in, CMP);
    // if(input < 0){
    //     input = -input;
    neg = true;

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

	return (mid = (neg) ? (-mid) : mid);
}
/*
share* ABYmath::aby_acos(ABYParty* party, BooleanCircuit* circ, float input) {
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

share* ABYmath::aby_atan(ABYParty* party, BooleanCircuit* circ, float input) {
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

share* ABYmath::aby_atan2(ABYParty* party, BooleanCircuit* circ, float a, float b) {
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
*//*
share* ABYmath::aby_sinh(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double neg_one = -1.0, two = 2.0;
    uint64_t* neg_onefptr = (uint64_t*)& neg_one;
    uint64_t* twofptr = (uint64_t*)& two;

    share* s_in_out = circ->PutSharedOUTGate(s_in);
    
    uint64_t r_exp = aby_exp(party, circ, s_in);
    uint64_t in = (uint64_t) s_in_out->get_clear_value<uint64_t>();
    party->Reset();
    s_in = circ->PutSharedINGate(in, 64);
    share* s_neg_one = circ->PutCONSGate(*neg_onefptr, 64);
    share* s_in_neg = circ->PutFPGate(s_in, s_neg_one, MUL);
    s_in_out = circ->PutSharedOUTGate(s_in);
    uint64_t r_exp_neg = aby_exp(party, circ, s_in_neg);
    party->Reset();

    share* s_exp_pos = circ->PutSharedINGate(r_exp, 64);
    share* s_exp_neg = circ->PutSharedINGate(r_exp_neg, 64);
    share* s_sub = circ->PutFPGate(s_exp_pos, s_exp_neg, SUB);
    share* s_two = circ->PutCONSGate(*twofptr, 64);
    share* s_div = circ->PutFPGate(s_sub, s_two, DIV);

    // PutOUTGate
    share* s_out = circ->PutSharedOUTGate(s_div);

    // execute
    party->ExecCircuit();

    // get output
    uint64_t res = (uint64_t) s_out->get_clear_value<uint64_t>();

	return res;
}

share* ABYmath::aby_cosh(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double neg_one = -1.0, two = 2.0;
    uint64_t* neg_onefptr = (uint64_t*)& neg_one;
    uint64_t* twofptr = (uint64_t*)& two;

    share* s_in_out = circ->PutSharedOUTGate(s_in);
    
    uint64_t r_exp = aby_exp(party, circ, s_in);
    uint64_t in = (uint64_t) s_in_out->get_clear_value<uint64_t>();
    party->Reset();
    s_in = circ->PutSharedINGate(in, 64);
    share* s_neg_one = circ->PutCONSGate(*neg_onefptr, 64);
    share* s_in_neg = circ->PutFPGate(s_in, s_neg_one, MUL);
    s_in_out = circ->PutSharedOUTGate(s_in);
    uint64_t r_exp_neg = aby_exp(party, circ, s_in_neg);
    party->Reset();

    share* s_exp_pos = circ->PutSharedINGate(r_exp, 64);
    share* s_exp_neg = circ->PutSharedINGate(r_exp_neg, 64);
    share* s_add = circ->PutFPGate(s_exp_pos, s_exp_neg, ADD);
    share* s_two = circ->PutCONSGate(*twofptr, 64);
    share* s_div = circ->PutFPGate(s_add, s_two, DIV);

    // PutOUTGate
    share* s_out = circ->PutSharedOUTGate(s_div);

    // execute
    party->ExecCircuit();

    // get output
    uint64_t res = (uint64_t) s_out->get_clear_value<uint64_t>();

	return res;
}

share* ABYmath::aby_tanh(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    share* s_in_out = circ->PutSharedOUTGate(s_in);
    
    uint64_t r_sinh = aby_sinh(party, circ, s_in);
    uint64_t in = (uint64_t) s_in_out->get_clear_value<uint64_t>();
    party->Reset();
    s_in = circ->PutSharedINGate(in, 64);
    uint64_t r_cosh = aby_cosh(party, circ, s_in);
    party->Reset();

    share* s_sinh = circ->PutSharedINGate(r_sinh, 64);
    share* s_cosh = circ->PutSharedINGate(r_cosh, 64);
    share* s_div = circ->PutFPGate(s_sinh, s_cosh, DIV);

    // PutOUTGate
    share* s_out = circ->PutOUTGate(s_div, ALL);

    // execute
    party->ExecCircuit();

    // get output
    uint64_t res = (uint64_t) s_out->get_clear_value<uint64_t>();

    party->Reset();

	return res;
}
*/
share* ABYmath::aby_ldexp(ABYParty* party, BooleanCircuit* circ, share* s_x, share* s_exp) {
    // PutINGate
    double two = 2.0;
    uint64_t* twofptr = (uint64_t*)& two;
    share* s_two = circ->PutCONSGate(*twofptr, 64);
    
    // 2^3 = e^(3ln(2))
	share* log2_res = circ->PutFPGate(s_two, LOG2);
    log2_res->set_bitlength(64);
	share* mul_res = circ->PutFPGate(s_exp, log2_res, MUL);
	share* exp2_res = circ->PutFPGate(mul_res, EXP2);
    exp2_res->set_bitlength(64);
	share* ldexp_res = circ->PutFPGate(s_x, exp2_res, MUL);

	return ldexp_res;
}

share* ABYmath::aby_cabs(ABYParty* party, BooleanCircuit* circ, share* s_ain, share* s_bin) {
    share* s_asqr = circ->PutFPGate(s_ain, s_ain, MUL);
    share* s_bsqr = circ->PutFPGate(s_bin, s_bin, MUL);
    share* s_add = circ->PutFPGate(s_asqr, s_bsqr, ADD);
    share* s_sqrt = circ->PutFPGate(s_add, SQRT);

	return s_sqrt;
}

share* ABYmath::aby_fmod(ABYParty* party, BooleanCircuit* circ, share* s_in_a, share* s_in_b) {
    // PutINGate
    double zero = 0, one = 1;
    uint64_t* zerofptr = (uint64_t*) &zero, *onefptr = (uint64_t*) &one;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_one = circ->PutCONSGate(*onefptr, 64);
    
    // judge for special case
    if(s_in_b == s_zero) return s_zero;

    share* s_cmp = circ->PutFPGate(s_zero, s_in_a, CMP);
    share* s_div = circ->PutFPGate(s_in_a, s_in_b, DIV);
    share* s_ceil = aby_ceil(party, circ, s_div);
    share* s_floor = aby_floor(party, circ, s_div);
    share* s_mux = circ->PutMUXGate(s_floor, s_ceil, s_cmp);

    share* s_mul = circ->PutFPGate(s_mux, s_in_b, MUL);
    share* s_sub = circ->PutFPGate(s_in_a, s_mul, SUB);

	return s_sub;
}

share* ABYmath::aby_modf(ABYParty* party, BooleanCircuit* circ, share* s_in, share* intptr) {
    // PutINGate
    double zero = 0;
    uint64_t* zerofptr = (uint64_t*) &zero;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);

    share* s_cmp = circ->PutFPGate(s_zero, s_in, CMP);
    share* s_ceil = aby_ceil(party, circ, s_in);
    share* s_floor = aby_floor(party, circ, s_in);
    share* s_mux_1 = circ->PutMUXGate(s_ceil, s_floor, s_cmp);
    share* s_sub = circ->PutFPGate(s_in, s_mux_1, SUB);

	return s_sub;
}
