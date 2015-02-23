concat.so : concat.c Makefile
	$(CC) -std=gnu99 -Wall -fPIC -shared -o $@ $< -ldl

# EOF #
