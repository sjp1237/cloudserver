ALL:main
main:main.cc FileUtil.cpp bundle.o
	g++ -o $@ $^  -lpthread -ljsoncpp  -std=c++11 -lstdc++fs -g 

clean:
	rm -f main 
