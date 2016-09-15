/*
 * threshold.hpp
 *
 *  Created on: Sep 15, 2012
 *      Author: gustavo
 */

#ifndef IMPL_THRESHOLD_GPU_CUH
#define IMPL_THRESHOLD_GPU_CUH

#include "../util/timer.hpp"
#include "../util/cuda_macros.h"
#include "../math/math_macros.h"

#include <cuda_runtime.h>

namespace impl {
	namespace img {

		namespace gpu {
			template<typename T> __global__ void threshold_binary(int rows, int columns, T * data, int stride, T Th, T min_val, T max_val) {

				int x = blockIdx.x * blockDim.x + threadIdx.x;
				int y = blockIdx.y * blockDim.y + threadIdx.y;

				if (x >= columns || y >= rows) {
					__syncthreads();
					return;
				}

				int idx = x + y * stride;

				if (data[idx] <= Th) data[idx] = min_val;
				else data[idx] = max_val;
			}
		}

		template<typename T> void threshold(int rows, int columns, T * data, int stride, T Th) {

			T * dev_data;
			int N = rows * columns;

			dim3 block_dim(12, 4);
			int gx = __ceil( (float)columns / block_dim.x);
			int gy = __ceil( (float)rows / block_dim.y);
			dim3 grid_dim(gx, gy);

			CUDA_CHECK_RETURN( cudaMalloc((void**) &dev_data, N * sizeof(T)));
			CUDA_CHECK_RETURN( cudaMemcpy(dev_data, data, N * sizeof(T), cudaMemcpyHostToDevice));

			gpu::threshold_binary<<<grid_dim, block_dim>>>(rows, columns, dev_data, stride, Th, (T) 0, (T) 255);

			CUDA_CHECK_RETURN(cudaThreadSynchronize());
			CUDA_CHECK_RETURN( cudaMemcpy(data, dev_data, N * sizeof(T), cudaMemcpyDeviceToHost));
			CUDA_CHECK_RETURN( cudaFree(dev_data));
		}

	}
}

#endif /* IMPL_THRESHOLD_GPU_CUH */
