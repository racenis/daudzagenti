default: repl

repl: main.c list.c list.h value.h value.c eval.h eval.c api.h api.c ipc.h ipc.c script.h script.c agent.h agent.c world.h world.c chemical.h chemical.c vector.h vector.c stimulus.h stimulus.c action.h action.c context.h context.c
	gcc vector.c value.c list.c script.c eval.c api.c ipc.c chemical.c agent.h agent.c world.c action.c stimulus.c context.c main.c -o repl.exe -I. -lws2_32 -no-pie -static -static-libstdc++ -static-libgcc
	
library:
	gcc -c list.c -o list.o -I.
	gcc -c value.c -o value.o -I.
	gcc -c eval.c -o eval.o -I.
	gcc -c api.c -o api.o -I.
	gcc -c ipc.c -o ipc.o -I.
	gcc -c script.c -o script.o -I.
	gcc -c agent.c -o agent.o -I.
	gcc -c world.c -o world.o -I.
	gcc -c chemical.c -o chemical.o -I.
	gcc -c vector.c -o vector.o -I.
	gcc -c stimulus.c -o stimulus.o -I.
	gcc -c action.c -o action.o -I.
	gcc -c context.c -o context.o -I.
	ar rcs libgoetia.a list.o value.o eval.o api.o ipc.o script.o agent.o world.o chemical.o vector.o stimulus.o action.o context.o

clean:
	del list.o
	del value.o
	del eval.o
	del api.o
	del ipc.o
	del script.o
	del agent.o
	del world.o
	del chemical.o
	del vector.o
	del stimulus.o
	del action.o
	del context.o