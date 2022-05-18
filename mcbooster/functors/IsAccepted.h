
/*
 * IsAccepted.h
 *
 * Copyright 2016 Antonio Augusto Alves Junior
 *
 * Created on : 29/03/2016
 * Author: Antonio Augusto Alves Junior
 * SPDX-License-Identifier: BSD-3-Clause
 *      
 */


/**\file IsAccepted.h
 * Implements isAccepted.
 */

#ifndef ISACCEPTED_H_
#define ISACCEPTED_H_

#include <mcbooster/Config.h>
#include <mcbooster/GTypes.h>

namespace mcbooster {

struct isAccepted {
    __host__ __device__ inline bool operator()(const int x) { return (x == 1); }
};
}

#endif /* ISACCEPTED_H_ */
