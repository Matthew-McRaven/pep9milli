#Install python dependencies
sudo apt-get -y install python-pip

#Add python to path
export PATH=$PATH:~/.local/bin

#Install LLVM6.0 dependencies
sudo apt-get -y install build-essential curl libcap-dev git cmake libncurses5-dev python-minimal python-pip unzip libtcmalloc-minimal4 libgoogle-perftools-dev

#Install LLVM6.0
sudo apt-get -y install clang-6.0 llvm-6.0 llvm-6.0-dev llvm-6.0-tools
 
#Build STP 
sudo apt-get -y install cmake bison flex libboost-all-dev python perl zlib1g-dev minisat
git clone https://github.com/stp/stp.git
(
	cd stp
	git checkout tags/2.3.3
	mkdir build
	cd build
	cmake ..
	make -j4
	sudo make install
)
#Clone google test code
(
	curl -OL https://github.com/google/googletest/archive/release-1.7.0.zip
	unzip release-1.7.0.zip
	rm release-1.7.0.zip
)
#Build klee-uclibc
git clone https://github.com/klee/klee-uclibc.git  
(
	cd klee-uclibc
	git checkout tags/klee_uclibc_v1.2
	./configure --make-llvm-lib --with-llvm-config /usr/bin/llvm-config-6.0
	make -j4
	sudo make install
)

#Install hidden Klee dependencies
sudo apt-get -y install libsqlite3-dev
pip install tabulate
pip install lit

#Build Klee
git clone https://github.com/klee/klee.git
(
	cd klee
	git checkout tags/v2.0
	mkdir build
	cd build
	cmake \
	-DENABLE_SOLVER_STP=ON \
	-DENABLE_POSIX_RUNTIME=ON \
  	-DENABLE_KLEE_UCLIBC=ON \
	-DENABLE_UNIT_TESTS=ON \
	-DGTEST_SRC_DIR=../../googletest-release-1.7.0 \
	-DKLEE_UCLIBC_PATH=../../klee-uclibc \
	-DLLVM_CONFIG_BINARY=/usr/bin/llvm-config-6.0 \
	-DLLVMCC=/usr/bin/clang-6.0 \
	-DENABLE_SYST-DLLVMCXX=/usr/bin/clang++-6.0 \
	..
	make -j4
	make check
	sudo make install
)

# Install tools to visualize runs.
sudo apt -y install kcachegrind
