semantic: semantic.o synax.o lexer.o 
	gcc -o $@ $^
.c.o:
	gcc -c $<
clean:
	rm -rf *.o synax