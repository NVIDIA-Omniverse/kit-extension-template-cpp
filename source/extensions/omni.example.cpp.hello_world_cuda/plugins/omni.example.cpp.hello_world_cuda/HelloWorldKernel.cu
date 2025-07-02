#include <cuda_runtime.h>
#include <cstdio>

// A very small CUDA kernel that prints from the device.
__global__ void helloKernel(void)
{
    printf("Hello from CUDA kernel! (block %d, thread %d)\n", blockIdx.x, threadIdx.x);
}

// Launch the kernel and wait for it to complete.
extern "C" bool launchCudaHelloKernel(void)
{
    // Launch one block with four threads.
    helloKernel<<<1, 4>>>();
    cudaError_t err = cudaDeviceSynchronize();
    return err == cudaSuccess;
}