
compiler = gcc -c -std=c++20 -I../../tram-sdk/src/ -I../../tram-sdk/libraries/ -Ilibraries -I../library
compilerlibsloc = -L../../tram-sdk/ -L../../tram-sdk/libraries/binaries/win64/ -Llibraries -L../library
compilerlibs = -ltramsdk -lglfw3 -lgdi32 -lopengl32 -lOpenAl32 -lBulletDynamics -lBulletCollision -lLinearMath -lgoetia -lws2_32 -static-libstdc++ -static-libgcc

default: main.o
	g++ -std=c++20 -o client main.o $(compilerlibsloc) $(compilerlibs)

	
	
clean:
	del main.o


main.o: src/main.cpp
	$(compiler) src/main.cpp -o main.o
