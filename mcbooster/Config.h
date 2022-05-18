/*
 * Config.h
 *
 *  Created on: Feb 24, 2016
 *  Author: Antonio Augusto Alves Junior
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <mcbooster/MCBooster.h>
#include <iostream>

#define CUDA 1
#define OMP 2
#define TBB 3
#define CPP 4

#if(__cplusplus < 201103L) && !defined(_MSV_VER)
#error "This library needs a C++11 compliant compiler"
#endif

#ifndef MCBOOSTER_BACKEND
#define MCBOOSTER_BACKEND CUDA
#endif

#if MCBOOSTER_BACKEND != CUDA && MCBOOSTER_BACKEND != OMP && MCBOOSTER_BACKEND != TBB && MCBOOSTER_BACKEND != CPP

#error "MCBooster: Backend not supported. MCBOOSTER_BACKEND = CUDA, OMP, TBB, or CPP"

#endif

#if MCBOOSTER_BACKEND == CUDA
#define CUDA_API_PER_THREAD_DEFAULT_STREAM
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#elif MCBOOSTER_BACKEND != CPP
#include <omp.h>
#endif

#include <thrust/detail/config/host_device.h>

#endif /* CONFIG_H_ */
