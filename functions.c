/* !* FUNCTIONS FOR MANDELBROT PROGRAM
 *
 * \author Sebastian Dichler <el16b032@technikum-wien.at> <sedi343@gmail.com>
 *
 * \version Rev.: 01, 28.03.2017 - Changed Helpdesk function
 *          Rev.: 02, 04.04.2017 - Changed helpdesk_2 function
 *          Rev.: 03, 06.04.2017 - Added getkey function
 *          Rev.: 04, 08.04.2107 - Added getnewzoom function
 *
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
	
	printf("The pixelgen program is generating the\n"
			"mandelbrot picture and writes the pixels into a local memory.\n"
			"This memory is moved into a global, shared memory.\n\n");
	printf("The writepic program is reading the pixels from the shared memory\n"
			"and writes them into a ppm P3 picture.\n\n");
	printf(BOLD"Please run writepic before pixelgen\n"
	        "and stop it before pixelgen due to data loss!\n\n"RESET);
	printf("The whole process is running in a while loop and zooms out of the standard zoom.\n\n");
			
	printf(BOLD ITALIC"OPTIONAL PARAMETERS:\n"RESET);
	printf("- "BOLD"[-w]\t"RESET" to change Image width\n");
	printf("- "BOLD"[-h]\t"RESET" to change Image height\n");
	printf("- "BOLD"[-i]\t"RESET" to change Iterations\n");
	printf("- "BOLD"[-t]\t"RESET" to change picture design\n");
	printf(" -"BOLD"[0]\t"RESET"  the default template\n");
	printf(" -"BOLD"[1]\t"RESET"  for 1st template\n");
	printf(" -"BOLD"[2]\t"RESET"  for 2nd template\n");
	printf(" -"BOLD"[3]\t"RESET"  for 3rd template\n");
	printf(" -"BOLD"[4]\t"RESET"  for 4th template\n");
	printf(" -"BOLD"[5]\t"RESET"  for 5th template\n");
	printf(" -"BOLD"[6]\t"RESET"  for 6th template\n");
	printf(" -"BOLD"[7]\t"RESET"  for 7th template\n");
	printf(" -"BOLD"[8]\t"RESET"  for 8th template\n");
	printf(" -"BOLD"[9]\t"RESET"  for 9th template\n");
	printf(" -"BOLD"[10]\t"RESET" for 10th template\n");
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

/* ---- NEW ZOOM FUNCTION ---- */

double getnewzoom(double zoom)
{
	if (zoom < 0.00005)
	{
		zoom = zoom + 0.000005;
	}
	else if (zoom < 0.0005)
	{
		zoom = zoom + 0.00005;
	}
	else if (zoom < 0.005)
	{
		zoom = zoom + 0.0005;
	}
	else if (zoom < 0.05)
	{
		zoom = zoom + 0.005;
	}
	else if (zoom < 0.5)
	{
		zoom = zoom + 0.05;
	}
	else
	{
		zoom = zoom + 0.5;
	}
	
	return zoom;
}

/* ---- KEY GENERATOR ---- */

key_t getkey(void)
{
	key_t key = -1;
	
	key = ftok("/etc/hostname", 'b');
	if (key == -1)
	{
		perror(BOLD"\nERROR: ftok: Can't generate Key"RESET);
		exit(EXIT_FAILURE);
	}
	
	return key;
}
