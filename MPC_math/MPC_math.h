#ifndef __MPCMATH_H_
#define __MPCMATH_H_
#define DEBUG_MPCMATH

#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <cmath>
#include <cassert>
#include <iomanip>
#include <limits>
#include <cfloat>
#include "../../abycore/aby/abyparty.h"
#include "../../abycore/circuit/arithmeticcircuits.h"
#include "../../abycore/circuit/booleancircuits.h"
#include "../../abycore/circuit/circuit.h"
#include "../../abycore/circuit/share.h"
#include "../../abycore/sharing/sharing.h"

class ABYmath {
public:

    share* aby_test(ABYParty* party, BooleanCircuit* circ, share* a, share* b);

    share* aby_pow(ABYParty* party, BooleanCircuit* circ, share* base, share* exp);

    share* aby_ceil(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_floor(ABYParty* party, BooleanCircuit* circ, share* input);
    
    share* aby_abs(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_exp(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_sqrt(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_sin_32(ABYParty* party, BooleanCircuit* circ, share* input);
    share* aby_sin_64(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_cos(ABYParty* party, BooleanCircuit* circ, share* input);
        
    share* aby_tan(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_asin(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_acos(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_atan(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_atan2(ABYParty* party, BooleanCircuit* circ, share* y, share* x);

    share* aby_sinh(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_cosh(ABYParty* party, BooleanCircuit* circ, share* input);

    share* aby_tanh(ABYParty* party, BooleanCircuit* circ, share* input);
        
    share* aby_ldexp(ABYParty* party, BooleanCircuit* circ, share* x, share* exponent);
        
    share* aby_cabs(ABYParty* party, BooleanCircuit* circ, share* complex_real, share* complex_imag);
    
    share* aby_fmod(ABYParty* party, BooleanCircuit* circ, share* x, share* y);
    
    share* aby_modf(ABYParty* party, BooleanCircuit* circ, share* input, share** intptr);

private:
    
};

#endif // __MPCMATH_H_