/* !* FUNCTIONS FOR MANDELBROT PROGRAM
 *
 * \author Sebastian Dichler <el16b032@technikum-wien.at> <sedi343@gmail.com>
 *
 * \information Some functions from my previous tasks due to
 *              ppm handling
 *
 */
 
#include "myhead.h"

/*------------------------------------------------------------------*/
/* F U N C T I O N   D E F I N I T I O N                            */
/*------------------------------------------------------------------*/

/* ---- CLEAR FUNCTION ---- */

void clear(void)
{
	system("clear");
	helpdesk_1();
}

/* ---- CLEAR FUNCTION WITH NO HELP ---- */

void clearNoHelp(void)
{
	system("clear");
}

/* ---- HELPDESK FUNCTION ---- */

void helpdesk_1(void)
{
	printf("\n");
	printf(BOLD"MANDELBROT @ v1.0\n"RESET);
	printf(BOLD"Created by Sebastian Dichler, 2017\n"RESET);
	printf("Use\""BOLD"-?"RESET"\" for more information.\n\n");
	
#if DEBUG
	printf(BOLDRED ITALIC"*** DEBUG MODE ACTIVE ***\n\n"RESET);
#endif
}

/* ---- HELPDESK FUNCTION ---- */

void helpdesk_2(void)
{
	printf("\n");
	printf(BOLD"MANDELBROT @ v1.0\n"RESET);
	printf(BOLD"Created by Sebastian Dichler, 2017\n\n"RESET);
	printf(BOLD"This is a programm, communicating between 2 programms\n"RESET);
	printf(BOLD"RUN writepic before pixelgen!!!\n"RESET);
	printf(BOLD"A simple programm (pixelgen) calculating a mandelbrot and put pixels\n"RESET);
	printf(BOLD"into a shared memory, and writepic is writing them into a ppm file\n\n"RESET);
	printf(BOLD ITALIC"OPTIONAL PARAMETERS:\n"RESET);
	printf("- "BOLD"[-w]\t"RESET" to change Image width\n");
	printf("- "BOLD"[-h]\t"RESET" to change Image height\n");
	printf("- "BOLD"[-i]\t"RESET" to change Iterations\n");
	printf("- "BOLD"[-t]\t"RESET" to change picture design\n");
	printf(" -"BOLD"[1]\t"RESET"  for 1st template\n");
	printf(" -"BOLD"[2]\t"RESET"  for 2nd template\n");
	printf(" -"BOLD"[3]\t"RESET"  for 3rd template\n");
	printf(" -"BOLD"[4]\t"RESET"  for 4th template\n");
	printf(" -"BOLD"[5]\t"RESET"  for 5th template\n");
	printf(" -"BOLD"[6]\t"RESET"  for 6th template\n");
	printf(" -"BOLD"[7]\t"RESET"  for 7th template\n");
	printf(" -"BOLD"[8]\t"RESET"  for 8th template\n");
	printf("- "BOLD"[-b]\t"RESET" to change color output\n");
	printf("- "BOLD"[-?]\t"RESET" to show this help message\n");
	
#if DEBUG
	printf(BOLDRED ITALIC"*** DEBUG MODE ACTIVE ***\n\n"RESET);
#endif
}

/* ---- ONLY NUMBERS FUNCTION ---- */

int check_number(char *number)
{
	char * pch;
	int i;
	
	pch = strchr(number, '.');
	if (pch != NULL)
	{
		printf(BOLD"\nERROR: No floating-point numbers allowed.\n"RESET);
		return 1;
	}
	
	pch = strchr(number, ',');
	if (pch != NULL)
	{
		printf(BOLD"\nERROR: No floating-point numbers allowed.\n"RESET);
		return 1;
	}
	
	for (i = 0; i < strnlen(number, STRINGLENGTH); i++)
	{
		if (isdigit(number[i]) == 0)
		{
			printf(BOLD"\nERROR: Parameter is not a number.\n"RESET);
			return 1;
		}
	}
	
	return 0;
}

/* ---- FUNCTION TO AVOID STRINGLEAKS IN MAIN FILE ---- */

int clearOptarg(char *string, char *input)
{
	strncpy(string, input, strnlen(input, STRINGLENGTH));
	string[strlen(input)] = '\0';
	
	if (strlen(string) >= STRINGLENGTH)
	{
		printf(BOLD"\nERROR: Parameterinput is too long!\n"RESET);
		return 1;
	}
	else
	{
		return 0;
	}
}
