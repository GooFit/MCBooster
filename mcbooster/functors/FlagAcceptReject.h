/*
 * FlagAcceptReject.h
 *
 * Copyright 2016 Antonio Augusto Alves Junior
 *
 * Created on : 29/03/2016
 * Author: Antonio Augusto Alves Junior
 * SPDX-License-Identifier: BSD-3-Clause
 *      
 */

/**\file FlagAcceptReject.h
 * Implements FlagAcceptReject.
 */
#ifndef FLAGACCEPTEDREJECTED_H_
#define FLAGACCEPTEDREJECTED_H_

#include <mcbooster/Config.h>
#include <thrust/random.h>
#include <mcbooster/GTypes.h>

namespace mcbooster {
/**\struct FlagAcceptReject
 * Flags generated events as accepted (1) or rejected (0).
 */
struct FlagAcceptReject {
    GReal_t wmax; ///< maximum weight
    GUInt_t seed; // added for GooFit functionality.
    /**
     * FlagAcceptReject constructor. It is initialized with the value of the maximum weight
     * with which event weights will be compared.
     */
    FlagAcceptReject(const GReal_t _wmax)
        : wmax(_wmax)
        , seed(0) {}
    FlagAcceptReject(const GReal_t _wmax, GUInt_t _seed)
        : // Modification for GooFit
        wmax(_wmax)
        , seed(_seed) {}
    /**
     * hash function. Generate hashs to be used in random number generation initialization
     */
    __host__ __device__ inline GUInt_t hash(GUInt_t a) {
        a = (a + 0x7ed55d16) + (a << 12);
        a = (a ^ 0xc761c23c) ^ (a >> 19);
        a = (a + 0x165667b1) + (a << 5);
        a = (a + 0xd3a2646c) ^ (a << 9);
        a = (a + 0xfd7046c5) + (a << 3);
        a = (a ^ 0xb55a4f09) ^ (a >> 16);
        return a;
    }
    /**
     * operator(). Takes the events index and weight and so flag it as accepted and rejected
     *
     */
    __host__ __device__ inline GBool_t operator()(GLong_t idx, GReal_t weight) {
        thrust::random::minstd_rand0 randEng(2863311530);
        thrust::uniform_real_distribution<GReal_t> uniDist(0.0, wmax);
        randEng.discard(idx + seed);
        GReal_t uni = uniDist(randEng);
        // printf("accRej idx %li seed%u  %.5g %.5g %s\n",idx, seed, uni, weight, (uni<weight) ? "true" : "false" );
        GBool_t flag = (uni < weight) ? true : false;
        return flag;
    }
};
}

#endif /* FLAGACCEPTEDREJECTED_H_ */
