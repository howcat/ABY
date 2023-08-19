#ifndef __MPCMATH_H_
#define __MPCMATH_H_
#define DEBUG_MPCMATH

#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <cmath>
#include <cassert>
#include <iomanip>
#include <limits>
#include "../../abycore/aby/abyparty.h"
#include "../../abycore/circuit/arithmeticcircuits.h"
#include "../../abycore/circuit/booleancircuits.h"
#include "../../abycore/circuit/circuit.h"
#include "../../abycore/circuit/share.h"
#include "../../abycore/sharing/sharing.h"

struct parameter_package{
    e_role role;
    std::string address;
    uint16_t port;
    seclvl seclevel;
    uint64_t bitlen;
    uint64_t nvals;
    uint64_t nthreads;
    e_mt_gen_alg mt_alg;
    e_sharing sharing;
};

typedef struct parameter_package pack;

class ABYmath {
public:
    pack read_parameter(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t bitlen, uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing);

    double aby_pow(pack p, double base, double exp, int precision);

    double aby_ceil(pack p, double input);

    double aby_floor(pack p, double input);
    /* finished line */
    double aby_abs(pack p, double input);

    double aby_exp(pack p, float input);

    double aby_sqrt(pack p, double input);

    double aby_sin(pack p, float input);

    double aby_cos(pack p, float input);
        
    double aby_tan(pack p, float input);

    float aby_asin(pack p, float input);

    float aby_acos(pack p, float input);

    float aby_atan(pack p, float input);

    double aby_atan2(pack p, float y, float x);

    float aby_sinh(pack p, float input);

    double aby_cosh(pack p, float input);

    double aby_tanh(pack p, float input);
        
    double aby_ldexp(pack p, double x, double exponent);
        
    double aby_cabs(pack p, double complex_real, double complex_imag);
    
    double aby_fmod(pack p, double x, double y);
    
    double aby_modf(pack p, double input, double *intptr);

private:
    
};

#endif // __MPCMATH_H_