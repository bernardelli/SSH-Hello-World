NVCC := nvcc
CXX  := g++ 
INCD := -I"/scratch-local/usr/include" -I"/usr/local/cuda-7.5/include"

#LIBS := -L"/usr/local/cuda-7.5/lib64" -L"/scratch-local/usr/lib64"

CPP_SOURCES := main.cpp
CU_SOURCES  := convolution.cu \
               convolution_shared.cu \
               cubefilling.cu \
               little_cuda_functions.cu \
               slicing.cu

NVCCFLAGS   := -gencode=arch=compute_20,code=\"sm_20,compute_20\"

HEADERS     := $(wildcard *.h) include_file.h

CPP_OBJS    := $(patsubst %.cpp, %.o, $(CPP_SOURCES))
CU_OBJS     := $(patsubst %.cu, %.cu_o, $(CU_SOURCES))

%.cu_o: %.cu
	$(NVCC) -w $(NVCCFLAGS) -c $(INCD) $(LIBS) -o $@ $<

%.o: %.cpp
	$(CXX) -g -lcudart `pkg-config --libs opencv` `pkg-config --cflags opencv` -c $(CXXFLAGS) $(INCD) $(LIBS) -o $@ $<

hello: $(CPP_OBJS) $(CU_OBJS)
	$(CXX) -o hello $(CU_OBJS) $(CPP_OBJS) $(LDFLAGS) $(INCD) $(LIBS)  `pkg-config --libs opencv` `pkg-config --cflags opencv`

clean:
	rm -f hello *.o *.cu_o
