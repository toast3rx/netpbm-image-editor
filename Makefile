build:
	gcc -ggdb3 -Wall -Wextra  image_editor.c utils.c \
	       image_utils.c commands.c -o image_editor -lm
run:
	./image_editor
clean:
	rm -f image_editor
