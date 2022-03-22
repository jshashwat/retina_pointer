## Retina Pipeline Implementation
Simple data pipeline with add, sub and mult kernel. Central router is designed
to dynamically order the pipelines. K2K communication is implemented using
bidirectional streams and host2K uses memory map for communication.

###  DESIGN FILES
Accelerator binary files will be compiled to the xclbin directory. The xclbin
directory is required by the Makefile and its contents will be filled during 
compilation. A listing of all the files in this example is shown below

```
src/data.h (input data structure)
src/host.cpp (test case and host side implementation)
src/krnl_stream_arb.cpp (pkt router kernel)
src/krnl_stream_vadd.cpp (add kernel)
src/krnl_stream_vsub.cpp (sub kernel)
src/krnl_stream_vmult.cpp (mult kernel)
krnl_stream_vadd_vmult.ini (stream connections)
```

###  COMMANDS To Run the Code
```
make all TARGET=sw_emu/hw_emu/hw DEVICE=<path-to-device>
```

```
XCL_EMULATION_MODE=sw_emu/hw_emu ./vadd_stream <path-to-xclbin-file>
```

```
Device path in samsung server (colo76)

/opt/dsa/U.2_Shell_v0.90_2019.2.1_Lounge_Release/
U.2_Shell_v0.90_2019.2.1_ubuntu16_04/xilinx_samsung_u2x4_201920_3/
xilinx_samsung_u2x4_201920_3.xpfm
```

