echo -n "Is asmjit installed... " 
if [ -e $HOME/.local/include/asmjit ]; then
	echo "yes"
else
	echo "no"
	echo "Starting building asmjit"
	if [ ! -e ./asmjit ]; then
		git clone https://github.com/asmjit/asmjit.git -q
		cd asmjit
	fi
	mkdir -p build
	cd build
	echo "Running CMAKE"
	cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/.local --log-level=ERROR
	echo -n "Compiling asmjit..."
	make -j$(nproc) > /dev/null
	echo "Done"
	echo -n "Installing... " 
	make install > /dev/null
	echo "Done"
	echo "Cleaning up"
	cd .. && cd ..
	rm -rf asmjit
fi


echo "Building nowavm..."
clang++ -O1 -L~/.local/lib -I~/.local/include -lasmjit  src/*.cpp src/assembly/*.cpp -o nowavm -pipe -std=c++20
echo "Building nwld..."
clang++ -O2 -L~/.local/lib -I~/.local/include -lasmjit  src/linker/linker.cpp -o nwld -pipe -std=c++20
echo "Done."
