#include "include_file.h"


#include <chrono>
#include <ctime>

#include "slicing.h"
#include "convolution.h"
#include "convolution_shared.h"
#include "little_cuda_functions.h"
#include "cubefilling.h"


int main(int argc, char **argv)
{
	
	/********************************************************************************
	*** initialization of variables                                               ***
	********************************************************************************/
	cv::Mat image, image2;
	int size, kernel_eps_size, kernel_xy_size, image_size;
	float *kernel_eps, *kernel_xy, *dev_cube_wi, *dev_cube_w, *dev_cube_wi_out, 
		*dev_cube_w_out, *dev_kernel_xy, *dev_kernel_eps, *dev_image, *result_image;
	cudaError_t cudaStatus;

	
	
	/********************************************************************************
	*** printing compute capability of each device                                ***
	********************************************************************************/
	checkingDevices();
	
	
	/********************************************************************************
	*** define kernel                                                             ***
	********************************************************************************/
/*	float sigma_xy = 3;
	kernel_xy_size = 7;
	kernel_xy = (float*)malloc(kernel_xy_size*sizeof(float));
	define_kernel(kernel_xy, sigma_xy, kernel_xy_size);
*/
	float sigma_eps = 10;
	kernel_eps_size = 21;
	kernel_eps = (float*)malloc(kernel_eps_size*sizeof(float));
	define_kernel(kernel_eps, sigma_eps, kernel_eps_size);
	
	
	int device = 1;
	cudaDeviceProp deviceProp;
	cudaGetDeviceProperties(&deviceProp, device);
	
	/********************************************************************************
	*** choose which GPU to run on, change this on a multi-GPU system             ***
	********************************************************************************/
	cudaStatus = cudaSetDevice(device);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?\n");
	}

	/********************************************************************************
	*** loading image and display it on desktop                                   ***
	********************************************************************************/
	image2 = cv::imread("lena.bmp", CV_LOAD_IMAGE_GRAYSCALE);   // Read the file


	if (!image2.data) {
		std::cerr << "Could not open or find \"lena.bmp\"." << std::endl;
		return 1;
	}
	
	
	for (float j = 0.1; j <= 1.09; j+=0.1){
			printf("j=%f\n",j);
			cv::resize(image2, image, cv::Size(image2.cols * j, image2.rows * j), 0, 0, CV_INTER_LINEAR);
			image.convertTo(image, CV_32F);
			image_size = image.rows*image.cols;
			size = image_size * 256;
			dim3 dimensions = dim3(image.rows, image.cols, 256);
			
			
		char filename[40];
		sprintf(filename, "image_size_%d__kernel_eps_size_%d.txt", image.rows, kernel_eps_size);
		FILE* output_file = fopen(filename, "w");
		fprintf(output_file,"kernel_xy_size: %d, kernel_eps_size: %d\nelapsed_seconds\tkernel_xy_size\n", kernel_xy_size, kernel_eps_size);
		
	
		

			//copyMakeBorder(image, image, sigma_xy / 2, sigma_xy / 2, sigma_xy / 2, sigma_xy / 2, IPL_BORDER_CONSTANT, 0);
	#ifndef __linux__	
			cv::namedWindow("Original image", cv::WINDOW_AUTOSIZE);
			cv::imshow("Original image", image);
	#endif

	for(int i = 0; i < 50; i+=3){
	//	printf("i:%d\n", i);
		float sigma_xy = i;
		kernel_xy_size = 1+2*i;
	
			

		    	for(int k = 0; k < 10; k++){
				std::chrono::time_point<std::chrono::system_clock> start, end;
				start = std::chrono::system_clock::now();
				kernel_xy = (float*)malloc(kernel_xy_size*sizeof(float));
				define_kernel(kernel_xy, sigma_xy, kernel_xy_size);
	
				/********************************************************************************
				*** allocate the space for cubes on gpu memory                                ***
				********************************************************************************/
				cudaStatus = allocateGpuMemory(&dev_cube_wi, size);
				cudaStatus = allocateGpuMemory(&dev_cube_w, size);
				cudaStatus = allocateGpuMemory(&dev_cube_wi_out, size);
				cudaStatus = allocateGpuMemory(&dev_cube_w_out, size);
				cudaStatus = allocateGpuMemory(&dev_kernel_xy, kernel_xy_size);
				cudaStatus = allocateGpuMemory(&dev_kernel_eps, kernel_eps_size);
				cudaStatus = allocateGpuMemory(&dev_image, image_size);
				    
				if (cudaStatus != cudaSuccess) {
					fprintf(stderr, "cudaMalloc failed!\n");
				}
				    

				    

				    
				/********************************************************************************
				*** copy cubes on gpu memory                                                  ***
				********************************************************************************/
				//cudaStatus = copyToGpuMem(dev_cube_wi,cube_wi, size);
				//cudaStatus = copyToGpuMem(dev_cube_w,cube_w, size);
				cudaStatus = copyToGpuMem(dev_kernel_xy, kernel_xy, kernel_xy_size);
				cudaStatus = copyToGpuMem(dev_kernel_eps, kernel_eps, kernel_eps_size);
				cudaStatus = cudaMemcpy(dev_image, image.ptr(), image_size*sizeof(float), cudaMemcpyHostToDevice);////copyToGpuMem(dev_image,(float*) image.ptr(), size); //only works with raw function!
				if (cudaStatus != cudaSuccess) {
					fprintf(stderr, "cudaMemcpy failed!\n");
				}		    

		
				/********************************************************************************
				*** setting up the cubes and filling them                                     ***
				********************************************************************************/
				//maybe use cudaPitchedPtr for cubes
				callingCubefilling(dev_image, dev_cube_wi, dev_cube_w, dimensions);

				    
				/********************************************************************************
				*** start concolution on gpu                                                  ***
				********************************************************************************/
				callingConvolution_shared(dev_cube_wi_out, dev_cube_w_out, dev_cube_wi, dev_cube_w, dev_kernel_xy, kernel_xy_size, dev_kernel_eps, kernel_eps_size, dimensions, device);
				    
				    
				/********************************************************************************
				*** start slicing on gpu                                                      ***
				********************************************************************************/
				result_image = (float*)malloc(image_size*sizeof(float));
				callingSlicing(result_image, dev_image, dev_cube_wi_out, dev_cube_w_out, dimensions);
				cv::Mat output_imag(image.rows, image.cols, CV_32F, result_image);
				    
				/********************************************************************************
				*** free every malloced space                                                 ***
				********************************************************************************/
				cudaFree(dev_cube_wi_out);
				cudaFree(dev_cube_wi);
				cudaFree(dev_cube_w_out);
				cudaFree(dev_cube_w);
				cudaFree(dev_kernel_xy);
				cudaFree(dev_kernel_eps);
				cudaFree(dev_image);
				free(kernel_xy);
		
				end = std::chrono::system_clock::now();
				double elapsed_seconds = std::chrono::duration<double,std::milli>(end-start).count();

				std::time_t end_time = std::chrono::system_clock::to_time_t(end);
			 	
				if (i) fprintf(output_file,"%f\t%d\r\n",elapsed_seconds, kernel_xy_size);
				
			}
			
		}
		fclose(output_file);
	}
	
	free(kernel_eps);


	/********************************************************************************
	*** show filtered image and save image                                        ***
	********************************************************************************/
#ifndef __linux__	
	cv::namedWindow("Filtered image", cv::WINDOW_AUTOSIZE);

	cv::imshow("Filtered image", output_imag/256);
	free(result_image); //needs to be freed after using output_imag
	cv::imwrite("Result.bmp", output_imag);
#endif	
	
	cv::waitKey(0);

	/********************************************************************************
	*** cudaDeviceReset must be called before exiting in order for profiling and  ***
    *** tracing tools such as Nsight and Visual Profiler to show complete traces. ***
	********************************************************************************/
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!\n");
        return 1;
    }
	
    return 0;
}


void define_kernel(float* output_kernel, float sigma, int size) {
	for (int i = 0; i < size; i++) {
		output_kernel[i] = expf(-0.5*powf((size / 2 - i) / sigma, 2));
	}
}
