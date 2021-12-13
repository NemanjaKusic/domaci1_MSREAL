#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	FILE* fp;
	char* izbor;
	size_t num_of_bytes = 1;
	int broj;
	char karakter;
	
	while(1)
	{

		printf("1: Pročitaj trenutno stanje stringa\n");
		printf("2: Upiši novi string\n");
		printf("3: Konkataniraj string na trenutni\n");
		printf("4: Izbriši čitav string\n");
		printf("5: Izbriši vodeće i prateće space karaktere\n");
		printf("6: Izbriši izraz iz stringa\n");
		printf("7: Izbriši poslednjih n karaktera iz stringa\n");
		printf("Q: Za izlaz iz aplikacije\n");

		fp = fopen ("/dev/stred", "r");
		if(fp==NULL) 
		{
			puts("Problem pri otvaranju /dev/stred");
			return -1;
		}

		izbor = (char *)malloc(num_of_bytes+1);
		/* do{
			getline(&izbor, &num_of_bytes, fp);
			sscanf(izbor, "%c", &karakter);
		}while(karakter == NULL); */

		getline(&izbor, &num_of_bytes, fp);
		sscanf(izbor, "%d", &broj);

		if(fclose(fp))
		{
			puts("Problem pri zatvaranju /dev/stred");
			return -1;
		}
			
		free(izbor);

		switch (broj)
		{
			case '1':
				break;
			case '2':
				break;
			case '3':
				break;
			case '4':
				break;
			case '5':
				break;
			case '6':
				break;
			case '7':
				break;
			case 'Q':
				printf("Izlaz iz aplikacije");
				return 0;

				break;
			default:
				break;
		}

		
	}

	return 0;
}
