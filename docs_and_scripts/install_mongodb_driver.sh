#!/usr/bin/sh

## Requires root permissions

echo "Download packages"
sudo apt-get install -y libmongoc-dev
sudo apt-get install -y libbson-dev
sudo ldconfig

echo "Download wanted version and go in"
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.5.1/mongo-cxx-driver-r3.5.1.tar.gz
tar -xzf mongo-cxx-driver-r3.5.1.tar.gz
cd mongo-cxx-driver-r3.5.1/build

echo "Configure the driver"
cmake ..                                \
    -DCMAKE_BUILD_TYPE=Release          \
    -DCMAKE_INSTALL_PREFIX=/usr/local

echo "Build and install the driver"
sudo cmake --build .
sudo cmake --build . --target install

# Can uninstall here with
# sudo cmake --build . --target uninstall

echo "Delete the downloaded repository"
cd ../..
sudo rm -r mongo-cxx-driver-r3.5.1
sudo rm mongo-cxx-driver-r3.5.1.tar.gz
