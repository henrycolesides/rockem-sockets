CC = gcc
CFLAGS = -g #-Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement -Werror -Wno-return-local-addr -Wunsafe-loop-optimizations -Wuninitialized -Werror

RS = rockem_server
RC = rockem_client
H = rockem_hdr.h

all: $(RS) $(RC)

$(RS): $(RS).o
	$(CC) $(CFLAGS) -o $@ $<

$(RC): $(RC).o
	$(CC) $(CFLAGS) -o $@ $<

$(RS).o: $(RS).c $(H)
	$(CC) $(CFLAGS) -c $<

$(RC).o: $(RC).c $(H)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TC) \#*

tar:
	tar -cvf hsides_rockem_sockets.tar.gz $(RS).c $(RC).c $(H) Makefile

git add:
	git add $(RS).c $(RC).c rockem_hdr.h Makefile
