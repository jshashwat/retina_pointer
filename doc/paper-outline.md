# Retina APIs to support preprocessing

## Retina native APIs

```
- init, put(), get(), scan() [must have]
- delete(), update [optional]
```

### init
- applications must pass the operation id and the order for the preprocessing
- for e.g., op-id = [1, 5, 3]
- kernels 2 and 4 will be skipped and the rest will be processed in the above
  order
- **init** API should have this array and as an arg and the host should store this info
  and pass to the router during its init time
```
	init(arg0, arg1, arg2, int op_id[])
```

### put 
- CPU --> FPGA --> SSD --> CPU
- **put** API should have parameter to turn on/off the preprocessing 

```
	put(arg0, arg1, arg2, bool need_pre_proc)
```
- if enabled, **put** kernel will write to the router\_in\_stream 
- poll on the router\_out\_stream for the compressed key
- compelete write and return to host

### get
- CPU --> FPGA --> SSD --> FPGA --> CPU
- **get** API should have a similar on/off parameter 

```
	get(arg0, arg1, arg2, bool need_pre_proc)
```
- fetch the key from the DDR-cache or SSD
- write to the router\_in\_stream
- poll on router\_out\_stream for the decompressed key
- return the key to host


**all retina kernels should have a private in and out stream to the router
kernel**


## Evaluation Plan 
- Retina vs Kvell vs SILK
- for large key size including key compression and decompression 
- YCSB workloads

- preprocessing (application benchmark)
	- Retina vs CPU (any KV interface)
	- ML stage: fetch --> preprocess --> load
	- fetch data --> decode --> image saturate --> resize --> return 
	- calculate time for preprocess

- **accelerate fetching (Retina KV)**
- **accelerate preprocess (Retina pipeline)**
- **accelerate load (CPU)**

- [silk-source-code](https://github.com/theoanab/SILK-USENIXATC2019)
- [pink-source-code](https://github.com/dgist-datalab/PinK)


### TODO for benchmarking
- Implement the decoding, image saturate, resize logic in FPGA kernel
- Explore how to integrate a user-defined filesystem in tensorflow
- Implement a filesystem API over retina 
- Implement/find tensorflow with ResNet50 model for training benchmark
- Integrate retina filesystem API to tensorflow benchmark
- Implement CPU side compression/decompression for Kvell and Silk


### TODO for paper writing
- Define the problem space (what is the problem that retina tackles?)
- why do we need retina? what are our contributions?
- what is the impact of retina? e.g., ML preprocessing
- How to place our work? should not focus too much on the application side
































### Refs

[AIstore-nvidia-paper](https://arxiv.org/pdf/2001.01858.pdf)
[AIstore-nvidia-git](https://github.com/NVIDIA/aistore)
[AIstore-readme](https://nvidia.github.io/aistore/)
[AIstore-overview](https://nvidia.github.io/aistore/docs/overview)
[Storage-Performance-Basics-for-Deep-Learning-nvidia](https://developer.nvidia.com/blog/storage-performance-basics-for-deep-learning/)
[storage-bench-DL-paper](https://newtraell.cs.uchicago.edu/files/tr_authentic/TR-2021-01.pdf)

### Tensorflow and ResNet 
[Resnet-coding-with-tensorflow-1](https://www.analyticsvidhya.com/blog/2021/08/how-to-code-your-resnet-from-scratch-in-tensorflow/)
[Understand-and-implement-Resnet](https://towardsdatascience.com/understand-and-implement-resnet-50-with-tensorflow-2-0-1190b9b52691)
[Resnet-in-tensorflow-git](https://github.com/wenxinxu/resnet-in-tensorflow)
[Resnet-in-keras](https://towardsdatascience.com/understanding-and-coding-a-resnet-in-keras-446d7ff84d33)
[Detailed-Guide-Understand-Implement-ResNets](https://cv-tricks.com/keras/understand-implement-resnets/)
[pretrained-Resnet50-keras](https://keras.io/api/applications/resnet/#resnet50-function)
[image-classifier-using-Resnet50-Keras](https://medium.com/@venkinarayanan/tutorial-image-classifier-using-resnet50-deep-learning-model-python-flask-in-azure-4c2b129af6d2)

