

all: crunchfuck-5 crunchfuck-4 crunchfuck-5.1

crunchfuck-5.1: crunchfuck-5.c
	gcc -O3 -Wall -Wextra -Winline -DMAXCELLS=1024 -o $@ $^

crunchfuck-5: crunchfuck-5.c
	gcc -O3 -Wall -Wextra -Winline -o $@ $^

crunchfuck-4: crunchfuck-4.c
	gcc -O3 -Wall -Wextra -Winline -o $@ $^

