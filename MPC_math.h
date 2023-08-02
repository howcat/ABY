// file_name.h

#ifndef FILE_NAME_H
#define FILE_NAME_H

#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <math.h>
#include <cassert>

class ABYHelper {
public:
    int32_t read_test_options(int32_t* argcp, char*** argvp, e_role* role,
        uint32_t* bitlen, uint32_t* nvals, uint32_t* secparam, std::string* address,
        uint16_t* port, int32_t* test_op, double* base, double* exp);

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

private:
    // You can place any private utility functions or variables here if needed.
};

#endif // FILE_NAME_H
