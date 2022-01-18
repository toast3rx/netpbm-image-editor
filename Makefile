build:
	gcc -ggdb3 -Wall -Wextra  image_editor.c utils.c \
	       image_utils.c command_check.c exceptions.c  commands.c -o image_editor -lm

pack:
	zip -r  311CA_LateaMihaiAlexandru_Tema3.zip \
		image_editor.c utils.c utils.h image_utils.c image_utils.h command_check.c \
		command_check.h commands.c commands.h constants.h structs.h exceptions.h exceptions.c \
	       	image_example Makefile README README.md
run:
	./image_editor
clean:
	rm -f image_editor
