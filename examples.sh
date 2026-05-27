if [ ! -f ./nowavm ]; then
	echo "NowaVM is not builded, starting build process..."
	sleep 1
	make -j2
	if [ "$0" != 0 ]; then
		echo "Build error, exiting..."
		exit 1
	fi
	echo "Build succesful..."
	sleep 1
fi

echo "1. Math example (you will get register dump, it should be r0 = 10, r1 = 47 and r2 = 15)"
sleep 3
./nowavm examples/docs/math.asm -e -r -interpret

echo ""
sleep 3 

echo "2. Label (you will get 'jumped to address 9', etc)"
sleep 3
./nowavm examples/docs/labels.asm -e -r -interpret -V

echo ""
sleep 3

echo "3. functions (you will get 'calling address 9', etc)"
sleep 3
./nowavm examples/docs/funcs.asm -e -r -interpret -V

echo ""
sleep 3 

echo "4. preprocessor (you will get error that say 'im glad to hear that you are happy :)')"
sleep 3
./nowavm examples/docs/preprocessor.asm -e

rm -f examples/docs/*.sym
