// https://devblogs.nvidia.com/even-easier-introduction-cuda
// https://devblogs.nvidia.com/unified-memory-cuda-beginners

#include <iostream>
#include <math.h>


// cuda kernel
__global__
void add(size_t num_elements, const float* x, float* result)
{
  size_t index = blockIdx.x * blockDim.x + threadIdx.x;
  size_t stride = blockDim.x * gridDim.x;
  for (size_t i = index; i < num_elements; i += stride)
  {
    result[i] += x[i];
  }
}


int main()
{
  const size_t num_elements = 1 << 20;  // 1M elements
  float* x;
  float* y;
  cudaMallocManaged(&x, num_elements * sizeof(float));
  cudaMallocManaged(&y, num_elements * sizeof(float));

  for (size_t i = 0; i < num_elements; i++)
  {
    x[i] = 1.0f;
    y[i] = 2.0f;
  }

  // prefetch data to GPU
  int device = -1;
  cudaGetDevice(&device);
  cudaMemPrefetchAsync(x, num_elements * sizeof(float), device, NULL);
  cudaMemPrefetchAsync(y, num_elements * sizeof(float), device, NULL);

  size_t block_size = 256;
  size_t num_blocks = (num_elements + block_size - 1) / block_size;
  // run kernel on GPU
  add<<<num_blocks, block_size>>>(num_elements, x, y);
  // wait for GPU to finish
  cudaDeviceSynchronize();

  float max_error = 0.0f;
  for (size_t i = 0; i < num_elements; i++)
  {
    max_error = fmax(max_error, fabs(y[i] - 3.0f));
  }
  std::cout << "max error: " << max_error << std::endl;

  cudaFree(x);
  cudaFree(y);
}
