Hi! Welcome to my (Ishmael Perez's) TinyPL/0 compiler and executor.

To compile the program, enter the following in the compiler:

	gcc -o hw3tc hw3tc.c lexer.c parser.c virtualmachine.c header.h


After you've done that, you execute the program with:

	./hw3tc <inputfile> <optionalargs>

where:
	<inputfile> is a .txt file of PL0 code, like the samplein.txt
		file I placed in there for you, and
	<optionalargs> are up to 3 arguments you can type to make the
		program print its generated files. These are
			-l : print generated lexeme list
			-a : print generated assembly code
			-v : print the virtual machine stack trace

For example, to print the lexeme list from samplein.txt, you'd type
	./hw3tc samplein.txt -l

If nothing is placed for the arguments, the program prints only the
input file and its generated output file.

Have fun!
