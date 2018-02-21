#!/bin/bash
set -e

#instala a biblioteca libxbee3
git clone https://github.com/attie/libxbee3.git
cd libxbee3
make configure
make
sudo make install
cd ..
rm -rf libxbee3
echo "libxbee3 instalado com sucesso"
