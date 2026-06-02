if [ "$PREFIX" = "/data/data/com.termux/files/usr" ]; then
	echo "Running in Termux..."
	chmod +x ./termux.sh
	exec ./termux.sh
fi
source /etc/os-release
if [ "$ID" = "ubuntu" ] || [ "$ID" = "debian" ]; then
	sudo apt -y install git asmjit clang make
elif [ "$ID" = "arch" ]; then
	yay -S git asmjit clang make
elif [ "$ID" = "fedora" ]; then
	sudo dnf -y install git asmjit clang make
else
	echo "Unknown linux distro, stopping..."
	exit 1
fi

echo "Building..."
make clean && make -j2 && make linker
echo "Done!"
