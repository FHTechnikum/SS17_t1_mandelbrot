/* !* PIXELGENERATOR PROCESS
 *
 * \description
 *
 *
 * \author Sebastian Dichler <el16b032@technikum-wien.at> <sedi343@gmail.com>
 *
 * \version Rev.: 01, 23.03.2017 - Created
 *          Rev.: 02, 23.03.2017 - created the main function and parts of its declarations
 *          Rev.: 03, 24.03.2017 - Created the init of pixelgen.c and searched web for a algorithm
 *                                 failure present but can't find it.
 *          Rev.: 04, 25.03.2017 - Changed algorithm now its working
 *          Rev.: 05, 25.03.2017 - Created Timestemps and Output file generation in pixelgen.c to check
 *                                 if algorithm is working -> will remove this after programming the writepic.c files
 *          Rev.: 06, 26.03.2017 - Added zoom and colorb to parameter (change this to given values with 1,2,3 change)
 *          Rev.: 07, 27.03.2017 - Changed file handling for prototype (algorithm check)
 *          Rev.: 08, 27.03.2017 - Trying mutlithreading, is not working currently
 *          Rev.: 09, 28.03.2017 - Removing manual zoom and move to given values with 1,2,3 change
 *          Rev.: 10, 28.03.2017 - Added switch for given move and zoom values, currently not working right (using zoom)
 *          Rev.: 11, 28.03.2017 - Fixed bug with given move and zoom values, its now working fine :) zoom = 1/zoom
 *          Rev.: 12, 28.03.2017 - Changed templates (colors change is still missing)
 *          Rev.: 13, 28.03.2017 - User output fixes
 *          Rev.: 14, 28.03.2017 - Added template
 *          Rev.: 15, 29.03.2017 - Added another timer and removed the omp library
 *          Rev.: 16, 29.03.2017 - Added colors (not well yet), removed b parameter and added maxrange colorsettigns to algorithm
 *
 *
 *
 * \information Algorithm with information of
 *              http://stackoverflow.com/questions/16124127/improvement-to-my-mandelbrot-set-code
 *
 */

#include "myhead.h"

#if TIME
struct timeval timer1, timer2, timer3, timer4;
#endif

int main(int argc, char *argv[])
{
	int width = 800;
	int height = 460;
	int iterations = 5000;
	int x, y;
	
	double pr, pi;
	double newRe, oldRe, newIm, oldIm;
	double zoom, moveX, moveY;
	
	int brightnessr;
	int brightnessg;
	int brightnessb;
	double z;
	double iterations_double;
	int colorr;
	int colorg;
	int colorb;
	
	char widthString[STRINGLENGTH];
	char heightString[STRINGLENGTH];
	char iterationsString[STRINGLENGTH];
	char typeString[STRINGLENGTH];
	
	
#if MAKEPIC
	FILE* pFout = NULL;
#endif
	
	int semaphore1 = 0;
	int semaphore2 = 0;
	int sharedmemid = 0;
	
	PICTURE *picture_Pointer_local = NULL;
	
	SEMUN semaphore1union;
	SEMUN semaphore2union;
	SEMBUF semaphore1buffer;
	SEMBUF semaphore2buffer;
	
	key_t keySemaphore;
	key_t keySharedMem;
	
	int i = 0, k = 0, w = 0;
	int error = 0;
	int opt;
	int type = 0;
	
#if TIME
	double timediff;
#endif
	
	char *pEnd;
	
/*------------------------------------------------------------------*/
/* I N I T                                                          */
/*------------------------------------------------------------------*/

	clear();
	
	while ((opt = getopt (argc, argv, "w:h:i:t:?")) != -1)
	{
		switch (opt)
		{
			case 'w':
				error = clearOptarg(widthString, optarg);
				error = check_number(widthString);
				
				width = strtod(widthString, &pEnd);
			break;
			
			case 'h':
				error = clearOptarg(heightString, optarg);
				error = check_number(heightString);
				
				height = strtod(heightString, &pEnd);
			break;
			
			case 'i':
				error = clearOptarg(iterationsString, optarg);
				error = check_number(iterationsString);
				
				iterations = strtod(iterationsString, &pEnd);
			break;
			
			case 't':
				error = clearOptarg(typeString, optarg);
				error = check_number(typeString);
				
				type = strtod(typeString, &pEnd);
			break;
			
			case '?':
				clearNoHelp();
				helpdesk_2();
				
				exit(EXIT_SUCCESS);
			break;
		}
	}
	
	switch (type)
	{
		case 1:
			moveX = -0.75;
			moveY = 0.1;
			zoom = 0.008;
			
			colorr = 60;
			colorg = -50;
			colorb = -200;
		break;
		
		case 2:
			moveX = -0.7463;
			moveY = 0.1102;
			zoom = 0.005;
			
			colorr = 60;
			colorg = -50;
			colorb = -120;
		break;
		
		case 3:
			moveX = -0.74529;
			moveY = 0.113075;
			zoom = 0.00015;
			
			colorr = 80;
			colorg = 50;
			colorb = 20;
		break;
		
		case 4: /* ---- GOOD TEMPLATE ---- */
			moveX = -0.722;
			moveY = 0.246;
			zoom = 0.019;
			
			colorr = 100;
			colorg = -10;
			colorb = -200;
		break;
		
		case 5: /* ---- CHANGE TEMPLATE / COLOR ALGORITHM ---- */
			moveX = -0.16070135;
			moveY = 1.0375665;
			zoom = 0.00000010;
			
			colorr = -100;
			colorg = -255;
			colorb = -255;
		break;
		
		case 6:
			moveX = -0.0452407411;
			moveY = 0.9868162204352258;
			zoom = 0.00000000013;
			
			colorr = -30;
			colorg = -180;
			colorb = -255;
		break;
		
		case 7: /* ---- CHANGE TEMPLATE / COLOR ALGORITHM ---- */
			moveX = 0.281717921930775;
			moveY = 0.5771052841488505;
			zoom = 0.0000000000000192;
			
			colorr = -80;
			colorg = -140;
			colorb = -140;
		break;
		
		case 8:
			moveX = 0.432539867562512;
			moveY = 0.226118675951765;
			zoom = 0.0000032;
			
			colorr = 30;
			colorg = -50;
			colorb = -200;
		break;
		
		case 9:
			moveX = -1.99999911758838;
			moveY = 0;
			zoom = 0.00000000000128;
			
			colorr = 100;
			colorg = -40;
			colorb = -200;
		break;
		
		case 10:
			moveX = -1.296354375872899;
			moveY = 0.44185155566229;
			zoom = 0.0000000000006;
			
			colorr = 10;
			colorg = -100;
			colorb = -200;
		break;
		
		default:
			moveX = -0.5;
			moveY = 0;
			zoom = 1;
			
			colorr = 30;
			colorg = -80;
			colorb = 100;
		break;
	}
	
/*------------------------------------------------------------------*/
/* E R R O R   H A N D L I N G                                      */
/*------------------------------------------------------------------*/
	
/* ---- IF ONE PARAMETER INPUT FAILED OR IS NOT CORRECT ---- */
	
	if (error == 1)
	{
		perror(BOLD"\nERROR: One or more Parameters are not correct."RESET);
		exit(EXIT_FAILURE);
	}
	
	if (type < 0 || type > 10)
	{
		perror(BOLD"\nERROR: Type value must be between 0 and 8"RESET);
		exit(EXIT_FAILURE);
	}
	
/*------------------------------------------------------------------*/
/* I N I T                                                          */
/*------------------------------------------------------------------*/
	
/* ---- ALLOCATE MEMORY FOR LOCAL MEMORY ---- */
	
	picture_Pointer_local = (struct picture *)malloc(width * height * sizeof(struct picture));
	if (picture_Pointer_local == NULL)
	{
		perror(BOLD"\nERROR: malloc: Couldn't allocate memory."RESET);
		
		free(picture_Pointer_local);
		exit(EXIT_FAILURE);
	}
	
/* ---- GENERATE KEYS FOR SEMAPHORE AND SHARED MEMORY ---- */
	
	keySemaphore = ftok("/etc/hostname", 'b');
	if (keySemaphore == -1)
	{
		perror(BOLD"\nERROR: ftok: can't generate semaphore key"RESET);
		
		free(picture_Pointer_local);
		exit(EXIT_FAILURE);
	}
	
	keySharedMem = ftok("/etc/hostname", 'b');
	if (keySharedMem == -1)
	{
		perror(BOLD"\nERROR: ftok: can't generate shared mem key"RESET);
		
		free(picture_Pointer_local);
		exit (EXIT_FAILURE);
	}
	
/* ---- GENERATE SHARED MEMORY ---- */
	
	sharedmemid = shmget(keySharedMem, (width * height * 3), IPC_CREAT | 0666);
	if (sharedmemid == -1)
	{
		perror(BOLD"\nERROR: shmget: Couldn't generate shared memory."RESET);
		
		free(picture_Pointer_local);
		exit(EXIT_FAILURE);
	}
	
/* ---- GENERATE SEMAPHORE 1 ---- */
	
	semaphore1 = semget(keySemaphore, 1, IPC_CREAT | 0666);
	if (semaphore1 < 0)
	{
		perror(BOLD"\nERROR: semget: Couldn't generate semaphore 1"RESET);
		
		free(picture_Pointer_local);	
		exit(EXIT_FAILURE);
	}
	
/* ---- GENERATE SEMAPHORE 2 ---- */
	
	semaphore2 = semget(keySemaphore, 1, IPC_CREAT | 0666);
	if (semaphore2 < 0)
	{
		perror(BOLD"\nERROR: semget: Couldn't generate semaphore 2"RESET);
		
		free(picture_Pointer_local);	
		exit(EXIT_FAILURE);
	}
	
#if DEBUG
	printf(BOLDRED"Semaphore ID1: %d\n"RESET, semaphore1);
	printf(BOLDRED"Semaphore ID2: %d\n"RESET, semaphore2);
	printf(BOLDRED"Key SharedMem: %d\n"RESET, keySharedMem);
	printf(BOLDRED"Key Semaphore: %d\n"RESET, keySemaphore);
	printf(BOLDRED"Size needed for memory(3): %d\n"RESET, height*width*3);
	printf(BOLDRED"Size needed for memory(sizeof): %d\n"RESET, height*width*sizeof(struct picture));
	
	printf(BOLDRED"\nwidth: %d\n"RESET, width);
	printf(BOLDRED"height: %d\n"RESET, height);
	printf(BOLDRED"iterations: %d\n"RESET, iterations);
	printf(BOLDRED"Type: %d\n", type);
	printf(BOLDRED"-moveX: %.15f -moveY: %.15f -zoom: %.15f\n"RESET, moveX, moveY, zoom);
#endif

/* ---- USER OUTPUT ---- */
	
	printf(BOLD ITALIC"\n*** GENERATING MANDELBROT ***\n"RESET);
	
/*------------------------------------------------------------------*/
/* P R O G R A M M   S T A R T                                      */
/*------------------------------------------------------------------*/
	
/* ---- GENERATE TIME STEMP ---- */
	
#if TIME
	gettimeofday(&timer1, NULL);
#endif
	
/* ---- ALGORITHM CODE FOR COLOR (source in description) ---- */
	
	printf(BOLD"\n* Generating Mandelbrot Pixels...\n"RESET);

	zoom = 1/zoom;
	
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pr = 1.5 * (x - width / 2) / (0.5 * zoom * width) + moveX;
			pi = (y - height / 2) / (0.5 * zoom * height) + moveY;
			
			newRe = newIm = oldRe = oldIm = 0;
			
			for (i = 0; i < iterations; i++)
			{
				oldRe = newRe;
				oldIm = newIm;
				
				newRe = oldRe * oldRe - oldIm * oldIm + pr;
				newIm = 2 * oldRe * oldIm + pi;
				
				if ((newRe * newRe + newIm * newIm) > 4)
				{
					break;
				}
			}
			
/* ---- WRITE BLACK PIXELS ---- */
			
			if (i == iterations)
			{
				(picture_Pointer_local+k)->r = 0;
				(picture_Pointer_local+k)->g = 0;
				(picture_Pointer_local+k)->b = 0;
			}
			
/* ---- WRITE COLORED PIXELS ---- */
			
			else
			{
				z = sqrt(newRe * newRe + newIm * newIm);
				iterations_double = iterations;
				brightnessr = 256 * log2(1.75 + i - log2(log2(z))) / log2(iterations_double) + colorr;
				brightnessg = 256 * log2(1.75 + i - log2(log2(z))) / log2(iterations_double) + colorg;
				brightnessb = 256 * log2(1.75 + i - log2(log2(z))) / log2(iterations_double) + colorb;
				
				(picture_Pointer_local+k)->r = brightnessr;
				(picture_Pointer_local+k)->g = brightnessg;
				(picture_Pointer_local+k)->b = brightnessb;
				
/* ---- COLORS ARE LOWER THAN 0 ---- */
				
				if ((picture_Pointer_local+k)->r < 0)
				{
					(picture_Pointer_local+k)->r = 0;
				}
				if ((picture_Pointer_local+k)->g < 0)
				{
					(picture_Pointer_local+k)->g = 0;
				}
				if ((picture_Pointer_local+k)->b < 0)
				{
					(picture_Pointer_local+k)->b = 0;
				}
				
/* ---- COLORS ARE HIGHER THAN 255 ---- */
				
				if ((picture_Pointer_local+k)->r > 255)
				{
					(picture_Pointer_local+k)->r = 255;
				}
				if ((picture_Pointer_local+k)->g > 255)
				{
					(picture_Pointer_local+k)->g = 255;
				}
				if ((picture_Pointer_local+k)->b > 255)
				{
					(picture_Pointer_local+k)->b = 255;
				}
			}
		
			k++;
		}
	}
	
	printf(BOLD"* Done generating Pixels!\n\n"RESET);
	
/* ---- GENERATE TIME STAMP ---- */
	
#if TIME
	gettimeofday(&timer2, NULL);
#endif
	
/* ---- PRINT EVERY PIXEL IN OUTPUT ---- */
		
#if DEBUG_PIXEL
	for (w = 0; w < height*width; w++)
	{
		printf(BOLDRED ITALIC"Pixel: %010d RGB: %03d "RESET, w+1, (picture_Pointer_local+w)->r);
		printf(BOLDRED ITALIC"%03d "RESET, (picture_Pointer_local+w)->g);
		printf(BOLDRED ITALIC"%03d | "RESET, (picture_Pointer_local+w)->b);
	}
	
	printf("\n");
#endif
	
/* ---- NEEDED FOR MAKEPIC - REMOVE AFTER WRITEPIC WORKS ---- */
/* ---- WRITE OUTPUT FILE ---- */
	
#if MAKEPIC
	printf(BOLD"* Writing file...\n"RESET);
	
/* ---- OPEN OUTPUT-FILE ---- */

#if TIME
	gettimeofday(&timer3, NULL);
#endif
	
	pFout = fopen("out.ppm", "wb");
	if (pFout == NULL)
	{
		perror(BOLD"\nERROR: fopen: couldn't open output file."RESET);
		
		free(picture_Pointer_local);
		exit(EXIT_FAILURE);
	}
	
/* ---- WRITE OUTPUT FILE ---- */
	
	fprintf(pFout, "P3\n");
	fprintf(pFout, "#Mandelbrot Generator by Sebastian Dichler\n");
	fprintf(pFout, "#Used -blue value: %d -iterations: %d -zoom: %f -moveX: %f -moveY: %f\n", colorb, iterations, 1/zoom, moveX, moveY);
	fprintf(pFout, "%u %u\n", width, height);
	fprintf(pFout, "255\n");
	
	for (w = 0; w < height*width; w++)
	{
		fprintf(pFout, "%u %u %u\n", (picture_Pointer_local+w)->r, (picture_Pointer_local+w)->g, (picture_Pointer_local+w)->b);
	}
	
/* ---- CLOSE OUTPUT FILE ---- */
	
	error = fclose(pFout);
	if (error == EOF)
	{
		perror(BOLD"\nERROR: fclose: Can't close Outputfile."RESET);
		free(picture_Pointer_local);
		exit(EXIT_FAILURE);
	}

	
	printf(BOLD"* Done writing file!\n"RESET);
#if TIME
	gettimeofday(&timer4, NULL);
#endif
#endif
	
#if TIME
	timediff = (timer2.tv_sec+timer2.tv_usec*0.000001)-(timer1.tv_sec+timer1.tv_usec*0.000001);
	
	printf(BLACK BACKYELLOW"\nGenerated Mandelbrot values within "BOLDBLACK BACKYELLOW"%f"BLACK BACKYELLOW" secs"RESET"\n\n", timediff);
	
	timediff = (timer4.tv_sec+timer4.tv_usec*0.000001)-(timer3.tv_sec+timer3.tv_usec*0.000001);
	
	printf(BLACK BACKYELLOW"Wrote file within "BOLDBLACK BACKYELLOW"%f"BLACK BACKYELLOW" secs"RESET"\n\n", timediff);
#endif

/* ---- WORSE USE OF CLEARING SEMAPHORES AND SHARED MEMORY ----- */
/* ---- REMOVE THAT ---- */
	
	system("ipcrm -a");
	
	free(picture_Pointer_local);
	exit(EXIT_SUCCESS);
}
