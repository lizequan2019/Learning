

# g++ man.cpp   -fPIC -shared   -o ./mandll/libman.so
# g++ woman.cpp -fPIC -shared   -o ./womandll/libwoman.so


g++ main.cpp -L./mandll -lman -L./womandll -lwoman  -Wl,-rpath=./mandll:./womandll  -o main.out