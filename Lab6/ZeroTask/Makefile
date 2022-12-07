all:

c:
	gcc -o child child.c SHMFunc.c -lpthread
	gcc -o parent parent.c SHMFunc.c -lpthread

d:
	rm -rf child parent

r:
	./parent
