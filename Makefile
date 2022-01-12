build:
	gcc -ggdb3 -Wall -Wextra  image_editor.c utils.c \
	       image_utils.c command_check.c  commands.c -o image_editor -lm

pack:
	zip 311CA_LateaMihaiAlexandru_Tema3.zip \
		image_editor.c utils.c image_utils.c command_check.c \
		commands.c Makefile
run:
	./image_editor
clean:
	rm -f image_editor
