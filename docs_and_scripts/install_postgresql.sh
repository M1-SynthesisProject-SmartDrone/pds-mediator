#!/usr/bin/sh

# Go in pds-global repository, "docs/tutos" folder to have infos on postgresql setup

sudo apt-get install postgresql postgresql-contrib postgresql-server-dev-13 -y

echo "=     Install libpqxx     ="
git clone https://github.com/jtv/libpqxx.git
cd libpqxx
./configure
sudo mkdir build && cd build
sudo cmake ..
sudo make
sudo make install
cd ../..
sudo rm -r libpqxx