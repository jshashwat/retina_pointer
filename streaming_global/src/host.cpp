#include "data.h"
// This extension file is required for stream APIs
#include "CL/cl_ext_xilinx.h"
// This file is required for OpenCL C++ wrapper APIs
#include "xcl2.hpp"

#define CONST_ADD 2
#define CONST_SUB 3
#define CONST_MUL 4
#define SIZE      4
#define IN_NUM 5
auto constexpr c_test_size = 8 * 1024 * 1024; // 8 MB data

int reset(int *d, int *sw_result, int *out) {
	  
  d[0] = OP_ADD;
  d[1] = OP_SUB;
  d[2] = OP_MUL;
  d[3] = IN_NUM;
  *out = 0;
//  *sw_result = (((d->num + CONST_ADD) - CONST_SUB) * CONST);
	*sw_result = 0;
  return 0;
}

bool verify(int *sw_results, int *out) {
  bool match = true;

  std::cout << "Verifying the Results ..." << std::endl;
    if (*sw_results != *out) {
      match = false;
    }
  std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
  return match;
}

int main(int argc, char **argv) {
	unsigned int size = c_test_size;

  if (xcl::is_hw_emulation()) {
    size = 4096; // 4KB for HW emulation
  } else if (xcl::is_emulation()) {
    size = 2 * 1024 * 1024; // 4MB for sw emulation
  }

  int *d;
  int *sw_res; 	
  int *out;	
  int __dummy;
  size_t buff_size = sizeof(*d) * SIZE;

  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
    return EXIT_FAILURE;
  }

  d = new int[4];
  sw_res = new int; 	
  out = new int;
  if (!d || sw_res || out) {
	std::cout << "memory alloc failed" << d << "\t" << sw_res << "\t" << out <<std::endl;
  }

  // OpenCL Host Code Begins.
  cl_int err;

  // OpenCL objects
  cl::Device device;
  cl::Context context;
  cl::CommandQueue q;
  cl::Program program;
  cl::Kernel krnl_arb;
  cl::Kernel krnl_vadd;
  cl::Kernel krnl_vmult;
  cl::Kernel krnl_vsub;

  auto binaryFile = argv[1];

  // get_xil_devices() is a utility API which will find the xilinx
  // platforms and will return list of devices connected to Xilinx platform
  auto devices = xcl::get_xil_devices();

  // read_binary_file() is a utility API which will load the binaryFile
  // and will return the pointer to file buffer.
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  int valid_device = 0;
  for (unsigned int i = 0; i < devices.size(); i++) {
    device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err,
              q = cl::CommandQueue(context, device,
                                   CL_QUEUE_PROFILING_ENABLE |
                                       CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                                   &err));
    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      // Creating Kernel
      OCL_CHECK(err, krnl_arb = cl::Kernel(program, "krnl_stream_arb", &err));
      OCL_CHECK(err, krnl_vadd = cl::Kernel(program, "krnl_stream_vadd", &err));
      OCL_CHECK(err,
                krnl_vsub = cl::Kernel(program, "krnl_stream_vsub", &err));
      OCL_CHECK(err,
                krnl_vmult = cl::Kernel(program, "krnl_stream_vmult", &err));
      valid_device++;
      break; // we break because we found a valid device
    }
  }
  if (valid_device == 0) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  std::cout << "Stream Addition, Subtraction and Multiplication" /*<< d->num*/ << std::endl;

  reset(d, sw_res, out);

  // Running the kernel

  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  OCL_CHECK(err, cl::Buffer buffer_in1(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                       buff_size, d, &err));
  OCL_CHECK(err, cl::Buffer buffer_output(
                     context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                     sizeof(int), out, &err));

  // Setting Kernel Arguments

  OCL_CHECK(err, err = krnl_arb.setArg(0, buffer_in1));
  OCL_CHECK(err, err = krnl_vadd.setArg(1, __dummy));
  OCL_CHECK(err, err = krnl_vsub.setArg(1, __dummy));
  OCL_CHECK(err, err = krnl_vmult.setArg(1, buffer_output));

  // Copy input data to device global memory
  OCL_CHECK(
      err, err = q.enqueueMigrateMemObjects({buffer_in1}, 0 /* 0 means from host*/));

  // Launch the Kernel
  OCL_CHECK(err, err = q.enqueueTask(krnl_arb));
  OCL_CHECK(err, err = q.enqueueTask(krnl_vadd));
  OCL_CHECK(err, err = q.enqueueTask(krnl_vsub));
  OCL_CHECK(err, err = q.enqueueTask(krnl_vmult));
  q.finish();

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
  q.finish();
  // OpenCL Host Code Ends

  // Compare the device results with software results
  bool match = verify(sw_res, out);

  return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
