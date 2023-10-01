#include "MPC_math.h"

share* ABYmath::aby_test(ABYParty* party, BooleanCircuit* circ, share* s_in, share* b){
    double e = 1.2345678, f = 1.2345678;
    uint64_t* efptr = (uint64_t*)& e;
    uint64_t* ffptr = (uint64_t*)& f;
    share* s_e = circ->PutCONSGate(*efptr, 64);
    share* s_f = circ->PutCONSGate(*ffptr, 64);
    double zero = 0;
    uint64_t* zerofptr = (uint64_t*) &zero;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_sub = circ->PutFPGate(s_e, s_f, SUB);

    for(int i = 0; i < 64; i++){
        std::cout << i << ": " << s_zero->get_wire_id(i) << ", ";
        // s_zero->set_wire_id(i, s_e->get_wire_id(i));
        // std::cout << s_sub->get_wire_id(i) << ", ";
        std::cout << s_sub->get_wire_id(i) << "\n";
    }
    // share* s_log2 = circ->PutFPGate(s_e, LOG2);
    // s_log2->set_bitlength(64);
    // share* s_mul = circ->PutFPGate(s_in, s_log2, MUL);
    // share* s_exp2 = circ->PutFPGate(s_mul, EXP2);
    // s_exp2->set_bitlength(64);

	return s_sub;
    
}

share* ABYmath::aby_pow(ABYParty* party, BooleanCircuit* circ, share* s_base, share* s_exp){
    // 2^3 = e^(3ln(2))
    share* s_log2 = circ->PutFPGate(s_base, LOG2);
    s_log2->set_bitlength(64);
	share* s_mul = circ->PutFPGate(s_exp, s_log2, MUL);
	share* s_exp2 = circ->PutFPGate(s_mul, EXP2);
    s_exp2->set_bitlength(64);

    double zero = 0, nan_d = std::nan("1");
    uint64_t* zerofptr = (uint64_t*) &zero;
    uint64_t* nanfptr = (uint64_t*) &nan_d;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_nan = circ->PutCONSGate(*nanfptr, 64);
    share* s_cmp_1 = circ->PutFPGate(s_zero, s_base, CMP);
    share* s_exp_abs = aby_abs(party, circ, s_exp);
    share* s_exp_abs_int = aby_floor(party, circ, s_exp_abs);
    share* s_cmp_2 = circ->PutFPGate(s_exp_abs, s_exp_abs_int, CMP);
    share* s_and = circ->PutANDGate(s_cmp_1, s_cmp_2);
    share* s_res = circ->PutMUXGate(s_nan, s_exp2, s_and);

	return s_res;
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
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);

    share* s_cmp = circ->PutFPGate(s_in, s_zero, CMP);    
    share* s_neg = circ->PutFPGate(s_zero, s_in, SUB);
    share* s_mux = circ->PutMUXGate(s_in, s_neg, s_cmp);

	return s_mux;
}

share* ABYmath::aby_exp(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    // 2^(in * log2 e)
    // 2^(log2 e^in)
    // e^in
    double e = M_Ef64;
    uint64_t* efptr = (uint64_t*)& e;
    share* s_e = circ->PutCONSGate(*efptr, 64);

    // std::cout << s_e->get_wire_id(0) << "\n";
    // std::cout << s_e->get_wire_id(1) << "\n";
    // std::cout << s_e->get_wire_id(2) << "\n";
    // std::cout << s_e->get_wire_id(3) << "\n";
    
    share* s_log2 = circ->PutFPGate(s_e, LOG2);
    s_log2->set_bitlength(64);
    share* s_mul = circ->PutFPGate(s_in, s_log2, MUL);
    share* s_exp2 = circ->PutFPGate(s_mul, EXP2);
    s_exp2->set_bitlength(64);

	return s_exp2;
}

share* ABYmath::aby_sqrt(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double zero = 0, nan_d = std::nan("1");
    uint64_t* zerofptr = (uint64_t*) &zero;
    uint64_t* nanfptr = (uint64_t*) &nan_d;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_nan = circ->PutCONSGate(*nanfptr, 64);
    
    share* s_cmp = circ->PutFPGate(s_zero, s_in, CMP);    
    share* s_sqrt = circ->PutFPGate(s_in, SQRT);
    s_sqrt = circ->PutMUXGate(s_nan, s_sqrt, s_cmp);

	return s_sqrt;
}

share* ABYmath::aby_sin_32(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    share* s_sin = circ->PutFPGate(s_in, SIN, 32, 1, no_status);
    // s_sin->set_bitlength(32);
	return s_sin;
}

share* ABYmath::aby_sin(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double one = 1.0, round = 2 * M_PIf64;
    uint64_t* onefptr = (uint64_t*)& one;
    uint64_t* roundfptr = (uint64_t*)& round;
    share* s_one = circ->PutCONSGate(*onefptr, 64);
    share* s_r = circ->PutCONSGate(*roundfptr, 64);
    share* s_fmod = aby_fmod(party, circ, s_in, s_r);
    share* s_tyl = s_fmod;
    share* s_pow = s_fmod;
    share* s_fac = s_one;
    share* s_temp = s_one;

    // taylor series
    for(int i = 3; i <= 35; i += 2) {
        // input^i
        s_pow = circ->PutFPGate(s_pow, s_fmod, MUL);
        s_pow = circ->PutFPGate(s_pow, s_fmod, MUL);

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
    double zero = 0, one = 1.0, round = 2 * M_PIf64;
    uint64_t* zerofptr = (uint64_t*)& zero;
    uint64_t* onefptr = (uint64_t*)& one;
    uint64_t* roundfptr = (uint64_t*)& round;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_one = circ->PutCONSGate(*onefptr, 64);
    share* s_r = circ->PutCONSGate(*roundfptr, 64);
    share* s_fmod = aby_fmod(party, circ, s_in, s_r);
    share* s_tyl = s_one;
    share* s_pow = s_one;
    share* s_fac = s_one;
    share* s_temp = s_zero;

    // taylor series
    for(int i = 2; i <= 34; i += 2) {
        // input ^ i
        s_pow = circ->PutFPGate(s_pow, s_fmod, MUL);
        s_pow = circ->PutFPGate(s_pow, s_fmod, MUL);

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
    // PutINGate
    double round = 2 * M_PIf64;
    uint64_t* roundfptr = (uint64_t*)& round;
    share* s_r = circ->PutCONSGate(*roundfptr, 64);
    share* s_fmod = aby_fmod(party, circ, s_in, s_r);    
    share* s_sin = aby_sin(party, circ, s_fmod);
    share* s_cos = aby_cos(party, circ, s_fmod); 
    share* s_div = circ->PutFPGate(s_sin, s_cos, DIV);

    return s_div;
}

share* ABYmath::aby_asin(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // judge range
    // if(input > 1 || input < -1){
    //     double nanValue = std::numeric_limits<double>::quiet_NaN();
    //     return nanValue;
    // }

    // mark for negative number
    double zero = 0, negone = -1;
    uint64_t* zerofptr = (uint64_t*)& zero;
    uint64_t* negonefptr = (uint64_t*)& negone;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_negone = circ->PutCONSGate(*negonefptr, 64);
    share* s_cmp_neg = circ->PutFPGate(s_zero, s_in, CMP);
    share* s_neg_in = circ->PutFPGate(s_in, s_negone, MUL);
    s_in = circ->PutMUXGate(s_neg_in, s_in, s_cmp_neg);

    // prepare
    double low = 0.0;
    double high = M_PI_2f64;
    double one = 1, two = 2, pi = M_PIf64;
    int cnt = 0, spec_1 = 0, spec_2 = 0;
    uint64_t* highfptr = (uint64_t*) &high;
    uint64_t* lowfptr = (uint64_t*) &low;
    uint64_t* onefptr = (uint64_t*) &one;
    uint64_t* twofptr = (uint64_t*) &two;
    uint64_t* pifptr = (uint64_t*) &pi;

    // PutINGate
    share* s_two = circ->PutCONSGate(*twofptr, 64);
    share* s_one = circ->PutCONSGate(*onefptr, 64);
    share* s_high = circ->PutCONSGate(*highfptr, 64);
    share* s_low = circ->PutCONSGate(*lowfptr, 64);
    share* s_pi = circ->PutCONSGate(*pifptr, 64);

    // sin number
    double sin_1_4 = sin(M_PI_2 / 2);
    uint64_t* fptr_1_4 = (uint64_t*) &sin_1_4;
    share* s_sin_1_4 = circ->PutCONSGate(*fptr_1_4, 64);
    double sin_1_8 = sin(M_PI_2 / 4);
    uint64_t* fptr_1_8 = (uint64_t*) &sin_1_8;
    share* s_sin_1_8 = circ->PutCONSGate(*fptr_1_8, 64);
    double sin_3_8 = sin(3 * M_PI_2 / 4);
    uint64_t* fptr_3_8 = (uint64_t*) &sin_3_8;
    share* s_sin_3_8 = circ->PutCONSGate(*fptr_3_8, 64);
    double sin_1_16 = sin(1 * M_PI_2 / 8);
    uint64_t* fptr_1_16 = (uint64_t*) &sin_1_16;
    share* s_sin_1_16 = circ->PutCONSGate(*fptr_1_16, 64);
    double sin_3_16 = sin(3 * M_PI_2 / 8);
    uint64_t* fptr_3_16 = (uint64_t*) &sin_3_16;
    share* s_sin_3_16 = circ->PutCONSGate(*fptr_3_16, 64);
    double sin_5_16 = sin(5 * M_PI_2 / 8);
    uint64_t* fptr_5_16 = (uint64_t*) &sin_5_16;
    share* s_sin_5_16 = circ->PutCONSGate(*fptr_5_16, 64);
    double sin_7_16 = sin(7 * M_PI_2 / 8);
    uint64_t* fptr_7_16 = (uint64_t*) &sin_7_16;
    share* s_sin_7_16 = circ->PutCONSGate(*fptr_7_16, 64);
    
    // compare
    share* s_cmp_1_4 = circ->PutFPGate(s_in, s_sin_1_4, CMP);
    share* s_cmp_1_8 = circ->PutFPGate(s_in, s_sin_1_8, CMP);
    share* s_cmp_3_8 = circ->PutFPGate(s_in, s_sin_3_8, CMP);
    share* s_cmp_1_16 = circ->PutFPGate(s_in, s_sin_1_16, CMP);
    share* s_cmp_3_16 = circ->PutFPGate(s_in, s_sin_3_16, CMP);
    share* s_cmp_5_16 = circ->PutFPGate(s_in, s_sin_5_16, CMP);
    share* s_cmp_7_16 = circ->PutFPGate(s_in, s_sin_7_16, CMP);

    s_low = circ->PutMUXGate(s_sin_1_4, s_low, s_cmp_1_4);
    s_high = circ->PutMUXGate(s_high, s_sin_1_4, s_cmp_1_4);
    s_low = circ->PutMUXGate(s_sin_1_8, s_low, s_cmp_1_8);
    s_high = circ->PutMUXGate(s_high, s_sin_1_8, s_cmp_1_8);
    s_low = circ->PutMUXGate(s_sin_3_8, s_low, s_cmp_3_8);
    s_high = circ->PutMUXGate(s_high, s_sin_3_8, s_cmp_3_8);
    s_low = circ->PutMUXGate(s_sin_1_16, s_low, s_cmp_1_16);
    s_high = circ->PutMUXGate(s_high, s_sin_1_16, s_cmp_1_16);
    s_low = circ->PutMUXGate(s_sin_3_16, s_low, s_cmp_3_16);
    s_high = circ->PutMUXGate(s_high, s_sin_3_16, s_cmp_3_16);
    s_low = circ->PutMUXGate(s_sin_5_16, s_low, s_cmp_5_16);
    s_high = circ->PutMUXGate(s_high, s_sin_5_16, s_cmp_5_16);
    s_low = circ->PutMUXGate(s_sin_7_16, s_low, s_cmp_7_16);
    s_high = circ->PutMUXGate(s_high, s_sin_7_16, s_cmp_7_16);

    // in == 0
    share* s_spec_1 = circ->PutFPGate(s_in, s_zero, CMP);
    share* s_spec_2 = circ->PutFPGate(s_zero, s_in, CMP);
    share* s_spec = circ->PutORGate(s_spec_1, s_spec_2);

    share* s_res;
    share* s_add;
    share* s_sin;
    share* s_cmp;

    // cmp_loop only show 1 or 0 for condition
    // max iteration is 15
    while (cnt <= 10) {
        // mid = (low + high) / 2
        s_add = circ->PutFPGate(s_low, s_high, ADD);
        s_res = circ->PutFPGate(s_add, s_two, DIV);

        // s_cmp = sin(mid) > x
        s_sin = aby_sin(party, circ, s_res);
        s_cmp = circ->PutFPGate(s_sin, s_in, CMP);

        s_high = circ->PutMUXGate(s_res, s_high, s_cmp);
        s_low = circ->PutMUXGate(s_low, s_res, s_cmp);

        // prevent double point problem
        // while(mid > high || mid < low){
        //     mid = (low + high) / 2;
        //     cmp = -1;
        // }
        std::cout << cnt << "\n";

        cnt++;
    }


    // judge for special case
    share* s_neg_res = circ->PutFPGate(s_res, s_negone, MUL);
    s_res = circ->PutMUXGate(s_neg_res, s_res, s_cmp_neg);
    s_res = circ->PutMUXGate(s_res, s_zero, s_spec);

    return s_res;
}

share* ABYmath::aby_acos(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double pi = M_PI, two = 2.0;
    uint64_t* pifptr = (uint64_t*) &pi;
    uint64_t* twofptr = (uint64_t*)& two;
    share* s_pi = circ->PutCONSGate(*pifptr, 64);
    share* s_two = circ->PutCONSGate(*twofptr, 64);
    share* s_div = circ->PutFPGate(s_pi, s_two, DIV);
    share* s_asin = aby_asin(party, circ, s_in);
    share* s_sub = circ->PutFPGate(s_div, s_asin, SUB);

	return s_sub;
}

share* ABYmath::aby_atan(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double one = 1.0;
    uint64_t* onefptr = (uint64_t*)& one;
    share* s_one = circ->PutCONSGate(*onefptr, 64);
    share* s_mul = circ->PutFPGate(s_in, s_in, MUL);
    share* s_add = circ->PutFPGate(s_mul, s_one, ADD);
    share* s_sqrt = circ->PutFPGate(s_add, SQRT);
    share* s_div = circ->PutFPGate(s_in, s_sqrt, DIV);
    share* s_asin = aby_asin(party, circ, s_div);

	return s_asin;
}

// share* ABYmath::aby_atan2(ABYParty* party, BooleanCircuit* circ, share* s_in_a, share* s_in_b) {
//     // double input = a/b;
//     // double res = 0;
//     share* s_div = circ->PutFPGate(s_in_a, s_in_b, DIV);
//     share* res;

//     if(b > 0){
//         res = ABYmath::aby_atan(party, circ, s_div);
//     }else if(b == 0){
//         if(a > 0){
//             res = M_PI / 2;
//             // std::cout << "CORRECT: " << atan2(a, b) << std::endl;
//             // std::cout << "ATAN RES: " << M_PI / 2 << std::endl;
//         }else if(a < 0){
//             res = -M_PI / 2;
//             // std::cout << "CORRECT: " << atan2(a, b) << std::endl;
//             // std::cout << "ATAN RES: " << -M_PI / 2 << std::endl;
//         }else{
//             res = 0;
//             std::cout << "UNDEFINED\n";
//         }
//     }else{
//         if(a >= 0){
//             res = ABYmath::aby_atan(p, input) + M_PI;
//         }else{
//             res = ABYmath::aby_atan(p, input) - M_PI;
//         }
//     }

// 	return res;
// }

share* ABYmath::aby_sinh(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double zero = 0.0, two = 2.0;
    uint64_t* zerofptr = (uint64_t*)& zero;
    uint64_t* twofptr = (uint64_t*)& two;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_two = circ->PutCONSGate(*twofptr, 64);
    
    share* s_exp = aby_exp(party, circ, s_in);
    share* s_in_neg = circ->PutFPGate(s_zero, s_in, SUB);
    share* s_exp_neg = aby_exp(party, circ, s_in_neg);
    share* s_sub = circ->PutFPGate(s_exp, s_exp_neg, SUB);
    share* s_div = circ->PutFPGate(s_sub, s_two, DIV);

	return s_div;
}

share* ABYmath::aby_cosh(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    // PutINGate
    double zero = 0.0, two = 2.0;
    uint64_t* zerofptr = (uint64_t*)& zero;
    uint64_t* twofptr = (uint64_t*)& two;
    
    share* s_exp = aby_exp(party, circ, s_in);
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_in_neg = circ->PutFPGate(s_zero, s_in, SUB);
    share* s_exp_neg = aby_exp(party, circ, s_in_neg);
    share* s_add = circ->PutFPGate(s_exp, s_exp_neg, ADD);
    share* s_two = circ->PutCONSGate(*twofptr, 64);
    share* s_div = circ->PutFPGate(s_add, s_two, DIV);

	return s_div;
}

share* ABYmath::aby_tanh(ABYParty* party, BooleanCircuit* circ, share* s_in) {
    share* s_sinh = aby_sinh(party, circ, s_in);
    share* s_cosh = aby_cosh(party, circ, s_in);
    share* s_div = circ->PutFPGate(s_sinh, s_cosh, DIV);

	return s_div;
}

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
    double zero = 0, one = 1, nan_d = std::nan("1");
    uint64_t* zerofptr = (uint64_t*) &zero, *onefptr = (uint64_t*) &one;
    uint64_t* nanfptr = (uint64_t*) &nan_d;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);
    share* s_one = circ->PutCONSGate(*onefptr, 64);
    share* s_nan = circ->PutCONSGate(*nanfptr, 64);

    share* s_cmp_a = circ->PutFPGate(s_zero, s_in_a, CMP);
    share* s_cmp_b = circ->PutFPGate(s_zero, s_in_b, CMP);
    share* s_cmp = circ->PutXORGate(s_cmp_a, s_cmp_b);

    share* s_cmp_b_ = circ->PutFPGate(s_in_b, s_zero, CMP);
    share* s_cmp_1 = circ->PutORGate(s_cmp_b, s_cmp_b_);

    share* s_div = circ->PutFPGate(s_in_a, s_in_b, DIV);
    share* s_ceil = aby_ceil(party, circ, s_div);
    share* s_floor = aby_floor(party, circ, s_div);
    share* s_mux = circ->PutMUXGate(s_ceil, s_floor, s_cmp);

    share* s_mul = circ->PutFPGate(s_mux, s_in_b, MUL);
    share* s_sub = circ->PutFPGate(s_in_a, s_mul, SUB);
    s_sub = circ->PutMUXGate(s_sub, s_nan, s_cmp_1);

	return s_sub;
}

share* ABYmath::aby_modf(ABYParty* party, BooleanCircuit* circ, share* s_in, share** s_intptr) {
    // PutINGate
    double zero = 0;
    uint64_t* zerofptr = (uint64_t*) &zero;
    share* s_zero = circ->PutCONSGate(*zerofptr, 64);

    share* s_cmp = circ->PutFPGate(s_zero, s_in, CMP);
    share* s_ceil = aby_ceil(party, circ, s_in);
    share* s_floor = aby_floor(party, circ, s_in);
    share* s_mux_1 = circ->PutMUXGate(s_ceil, s_floor, s_cmp);
    share* s_sub = circ->PutFPGate(s_in, s_mux_1, SUB);
    *s_intptr = s_mux_1;

	return s_sub;
}
