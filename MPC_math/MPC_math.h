#ifndef __MPCMATH_H_
#define __MPCMATH_H_

#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <math.h>
#include <cassert>
#include "../../abycore/aby/abyparty.h"
#include "../../abycore/circuit/arithmeticcircuits.h"
#include "../../abycore/circuit/booleancircuits.h"
#include "../../abycore/circuit/circuit.h"
#include "../../abycore/circuit/share.h"
#include "../../abycore/sharing/sharing.h"

class ABYHelper {

public:

    int64_t aby_pow(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double base, double exp);

    int64_t aby_ceil(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double input);

    int64_t aby_floor(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double input);

    int64_t aby_abs(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double input);

    int64_t aby_exp(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, float input);

    int64_t aby_sqrt(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double input);

    int64_t aby_sin(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, float input);

    int64_t aby_cos(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, float input);
        
    int64_t aby_tan(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, float input);
        
    int64_t aby_cabs(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double a_in, double b_in);
        
    int64_t aby_ldexp(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
        uint64_t nvals, uint64_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, double a_in, double b_in);

private:
    

};

#endif // __MPCMATH_H_