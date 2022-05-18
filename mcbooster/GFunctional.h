/*
 * GFunctional.h
 *
 * Created on: Feb 25, 2016
 * Author: Antonio Augusto Alves Junior
 * SPDX-License-Identifier: BSD-3-Clause
 *      
 */

/** \file GFunctional.h
 * Implements the template functors IFunction and IFunctionArray
 */
#ifndef GFUNCTIONAL_H_
#define GFUNCTIONAL_H_

#include <mcbooster/Config.h>
#include <mcbooster/GTypes.h>
#include <mcbooster/Vector3R.h>
#include <mcbooster/Vector4R.h>

namespace mcbooster {
/** \struct  IFunction
 *  IFunction is the base class for arbitrary functions return any type suported by the framwork.
 */
template<typename RESULT>
struct IFunction {
    __host__ __device__ virtual RESULT operator()(const GInt_t n, Vector4R **particles) = 0;
};

/** \struct  IFunction
 *  IFunctionArray is the base class for arbitrary functions used to evaluate at once an array of variables.
 */
struct IFunctionArray {
    GInt_t dim{0};
    IFunctionArray() = default;

    __host__ __device__ virtual void operator()(const GInt_t np, Vector4R **particles, GReal_t *variables) = 0;
};
}

#endif /* GFUNCTIONAL_H_ */
