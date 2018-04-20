#!/bin/bash

#wget https://developer.nvidia.com/compute/cuda/8.0/Prod2/local_installers/cuda_8.0.61_375.26_linux-run -O cuda_8.0.61_375.26_linux.run
printf "q" | sudo ./cuda_8.0.61_375.26_linux.run

wget https://developer.nvidia.com/compute/cuda/8.0/Prod2/patches/2/cuda_8.0.61.2_linux-run -O cuda_8.0.61.2_linux.run
sudo sh cuda_8.0.61.2_linux.run

export PATH=/usr/local/cuda-8.0/bin${PATH:+:${PATH}}
export LD_LIBRARY_PATH=/usr/local/cuda-8.0/lib64\${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}

rm cuda_8.0.61_375.26_linux.run
rm cuda_8.0.61.2_linux.run


