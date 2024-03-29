/*
 * GContainers.h
 *
 * Copyright 2016 Antonio Augusto Alves Junior
 *
 * Created on: Feb 25, 2016
 * Author: Antonio Augusto Alves Junior
 * SPDX-License-Identifier: BSD-3-Clause
 *  
 */

/*! \file GContainers.h
 *  Typedef for useful container classes used in MCBooster.
 *  Containers defined here should be used in users application also.
 */

#ifndef GCONTAINERS_H_
#define GCONTAINERS_H_

#include <mcbooster/GContainersHost.h>

#include <mcbooster/Config.h>
#include <mcbooster/Vector3R.h>
#include <mcbooster/Vector4R.h>
#include <mcbooster/GTypes.h>

#include <vector>

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>


namespace mcbooster {

#if(MCBOOSTER_BACKEND == OMP || MCBOOSTER_BACKEND == CPP)
/*!
 * Generic template typedef for thrust::host_vector. Use it instead of Thrust implementation
 * in order to avoid problems to compile OpenMP based applications using gcc and without a cuda runtime installation.
 */
template<typename T>
using mc_device_vector = thrust::device_vector<T>;

#elif(MCBOOSTER_BACKEND == CUDA)
/*!
 * Generic template typedef for thrust::host_vector. Use it instead of Thrust implementation
 * in order to avoid problems to compile OpenMP based applications using gcc and without a cuda runtime installation.
 */
template<typename T>
using mc_device_vector = thrust::device_vector<T>;

#elif(MCBOOSTER_BACKEND == TBB)
/*!
 * Generic template typedef for thrust::host_vector. Use it instead of Thrust implementation
 * in order to avoid problems to compile OpenMP based applications using gcc and without a cuda runtime installation.
 */
template<typename T>
using mc_device_vector = thrust::device_vector<T>;
#endif


//-----------------------------------------------------------------------
// basic containers on device
typedef mc_device_vector<GBool_t> BoolVector_d;       /*! Typedef for a GBool_t device vector.*/
typedef mc_device_vector<GReal_t> RealVector_d;       /*! Typedef for a GReal_t device vector.*/
typedef mc_device_vector<GComplex_t> ComplexVector_d; /*! Typedef for a GComplex_t device vector.*/
typedef mc_device_vector<Vector4R> Particles_d;       /*! Typedef for a  Vector4R device vector.*/
typedef std::vector<Particles_d *>
    ParticlesSet_d; /*! Typedef for a  STL vector of pointers to device Particles_d vectors.*/
typedef std::vector<RealVector_d *>
    VariableSet_d; /*! Typedef for a STL vector of pointers to device RealVector_d vectors.*/

}
#endif /* GCONTAINERS_H_ */
