#!/bin/sh

echo "Install all dependencies"

sudo apt-get install libconfig++-dev

echo "Install Postgresql libs and runtimes"
sudo sh ./install_postgresql.sh

echo "Install mongodb driver"
echo "Warning : installation of mongodb server can be tedious : see 'install_mongodb.md' for help."
sudo sh ./install_mongodb_driver.sh"