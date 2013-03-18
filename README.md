scalc
=====

Calculate expression via gsoap

Example build:

CXXFLAGS="-ggdb -Wall -Werror" cmake -D CMAKE_INSTALL_PREFIX=/var/www/localhost .
make
make test
sudo make install
sudo make package
sudo make package_source

