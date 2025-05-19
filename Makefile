all:
	mkdir -p out/
	gcc src/main.c src/assembler.c -o out/c8asm
