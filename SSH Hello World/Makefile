NVCC := /opt/cuda/bin/nvcc
CXX  := g++ 
INCD := -I"/scratch-local/usr/include" -I"/opt/cuda/include"

LIBS := -L"/opt/cuda/lib64" -L"/scratch-local/usr/lib"

CFLAGS += -std=c++11 -pg -D_DEBUG -g

export PKG_CONFIG_PATH  := /scratch-local/usr/lib64/pkgconfig/
export PATH             := /opt/cuda/bin:$(PATH)
export LD_LIBRARY_PATH  := /scratch-local/usr/lib/:$(LD_LIBRARY_PATH)

SHELL := env PATH=$(PATH) /bin/bash

#/scratch-local/usr/include/

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
	$(CXX) $(CFLAGS) -lcudart `pkg-config --libs opencv` `pkg-config --cflags opencv` -c $(CXXFLAGS) $(INCD) $(LIBS) -o $@ $<

hello: $(CPP_OBJS) $(CU_OBJS)
	$(CXX) $(CFLAGS) -o hello $(CU_OBJS) $(CPP_OBJS) $(LDFLAGS) $(INCD) $(LIBS)  `pkg-config --libs opencv` `pkg-config --cflags opencv`

clean:
	rm -f hello *.o *.cu_o
