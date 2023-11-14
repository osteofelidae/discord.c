all: test.c discord.c
	gcc -lcurl -Wall test.c discord.c