# -*- indent-tabs-mode:t; -*-

NUM_HASHES=10
#ROCKYOU=rockyou3m.txt
ROCKYOU=rockyou100.txt
#ROCKYOU=rockyou1000.txt
#ROCKYOU=rockyou2000.txt

all: hashpass crack

# This rule links hashpass.o and md5.o along with the
# libssl and libcrypto libraries to make the executable.
hashpass: hashpass.o md5.o
	clang hashpass.o md5.o -o hashpass -l crypto

md5.o: md5.c md5.h
	clang -g -c md5.c -Wall

hashpass.o: hashpass.c md5.h
	clang -g -c hashpass.c -Wall

crack: crack.o md5.o
	clang crack.o md5.o -o crack -l crypto

crack.o: crack.c md5.h
	clang -g -c crack.c -Wall

hashes.txt: hashpass $(ROCKYOU)
	shuf -n $(NUM_HASHES) $(ROCKYOU) > passwords.txt
	./hashpass < passwords.txt > hashes.txt

hashes: hashes.txt

# Fetch the rockyou.txt file that contains 3 million entries
rockyou3m.txt:
	wget http://cs.sierracollege.edu/cs46/rockyou3m.txt.gz
	gunzip rockyou3m.txt.gz

clean:
	rm -f *.o hashpass crack hashes.txt passwords.txt

test: crack hashes.txt
	./crack hashes.txt $(ROCKYOU)

check: crack hashes.txt
	valgrind ./crack hashes.txt $(ROCKYOU)
	
check2: crack hashes.txt
	valgrind -v ./crack hashes.txt $(ROCKYOU) --leak-check=full

dbug: crack hashes.txt
	gdb ./crack ${ARGS}