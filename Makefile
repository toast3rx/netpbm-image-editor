build:
	gcc -ggdb3 -Wall -Wextra image_editor.c -o image_editor
run:
	./image_editor
clean:
	rm -f image_editor
