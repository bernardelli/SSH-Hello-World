#include "little_cuda_functions.h"

void checkingDevices()
{
	int deviceCount;
	cudaError_t cudaStatus = cudaGetDeviceCount(&deviceCount);
	
	if (cudaStatus == cudaErrorInsufficientDriver) {
		fprintf(stderr, "cudaGetDeviceCount failed!  Do you have CUDA installed?\n");
		exit(1);
	}
	
	int device;
	for (device = 0; device < deviceCount; ++device) {
		cudaDeviceProp deviceProp;
		cudaGetDeviceProperties(&deviceProp, device);
		
		if(device<100) printf("Device %d with name %s has compute capability %d.%d and concurrentKernels = %d.\n",
			device, deviceProp.name, deviceProp.major, deviceProp.minor, deviceProp.concurrentKernels);
	}
	cudaDeviceReset();

	

	
}

cudaError_t allocateGpuMemory(float**ptr, int size)
{
	cudaError_t cudaStatus = cudaMalloc((float**)ptr, size * sizeof(float));
	return cudaStatus;
}

cudaError_t copyToGpuMem(float *a, float *b, int size)
{
	cudaError_t cudaStatus = cudaMemcpy(a, b, size * sizeof(float), cudaMemcpyHostToDevice);
	return cudaStatus;
}
