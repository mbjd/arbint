test: test.c arbint.c helper-functions.c
	gcc -o run-tests test.c arbint.c helper-functions.c operators.c
