#include <stdio.h>
#include <stdio.h>
#include <string.h>

/*
-Para compilar el archivo:  gcc pc2.c -o hamming
-Para correr la aplicacion: ./hamming encode --input 2
*/

/*int code()*/


	//0x3E465C

int main(int argc, char *argv[]) {

	int num;
	sscanf(argv[3], "%x", &num);

	printf("0x%x %i\n", num, num);
	printf("%s", ~001);

	//3E = 111110
	//printf("\n%b", num);


	return 0;
}
