#include <cstdio>
#include <cstdlib>
#include <ctime>


int main()
{
	srand(time(NULL));
	int GusseThisNumber = rand();

	int userinput = 0;
	bool FoundNumber = false;

	do {
		scanf_s("%d", &userinput);

		if (userinput == GusseThisNumber)
		{
			FoundNumber = true;
			printf("Found it!");
		}
		else if (userinput > GusseThisNumber)
			printf("to high!");
		else if (userinput < GusseThisNumber)
			printf("to low!");
		printf("%d\n", userinput);
	} while (!FoundNumber);

	getchar();
	getchar();

	return 0;
}
