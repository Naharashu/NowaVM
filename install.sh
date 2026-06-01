source /etc/os-release
if [ "$ID" = "ubuntu" || "$ID" = "debian" ]; then
	apt install git asmjit clang make
elif [ "$ID" = "arch" ]; then
	yay -S git asmjit clang make
elif [ "$ID" = "fedora" ]; then
	dnf install git asmjit clang make
else
	echo "Unknown linux distro, stopping..."
	exit 1
fi

echo "Building..."
make clean && make -j2 && make linker
echo "Done!"