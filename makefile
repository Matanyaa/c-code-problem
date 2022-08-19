CREATOR = gcc -Wall -ansi -pedantic
P = ./as_files

my_assembler: my_assembler.o pre_process.o first_pass.o second_pass.o util.o list.o err.o data_image.o syntax_analysis.o include.h
	$(CREATOR) -g my_assembler.o pre_process.o first_pass.o second_pass.o util.o list.o err.o data_image.o syntax_analysis.o -o my_assembler

my_assembler.o: my_assembler.c include.h
	$(CREATOR) -c -g my_assembler.c

pre_process.o: pre_process.c include.h
	$(CREATOR) -c -g pre_process.c

first_pass.o: first_pass.c include.h
	$(CREATOR) -c -g first_pass.c

second_pass.o: second_pass.c include.h
	$(CREATOR) -c -g second_pass.c

util.o: util.c include.h util.h
	$(CREATOR) -c -g util.c

list.o: list.c include.h list.h
	$(CREATOR) -c -g list.c

err.o: err.c include.h err.h
	$(CREATOR) -c -g err.c

data_image.o: data_image.c include.h data_image.h
	$(CREATOR) -c -g data_image.c

syntax_analysis.o: syntax_analysis.c include.h
	$(CREATOR) -c -g syntax_analysis.c

test: 
	./my_assembler ./as_files/test1 ./as_files/full_test


