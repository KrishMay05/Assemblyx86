#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
/*
 * Prints out histogram with asm lines
 */
unsigned short * g_histogram_address;
static size_t g_code_origin;
static long g_histogram_size;
static char * g_commandName;

extern int __ehdr_start; // Start Text
extern int __data_start; // End Text

void start_histogram()
{

    // get information to print histogram to file
    g_code_origin = (size_t)&__ehdr_start;
    size_t code_end = (size_t)&__data_start;
    g_histogram_size = (long)code_end - g_code_origin;

    char exe[1024];
    int ret;

    ret = readlink("/proc/self/exe",exe,sizeof(exe)-1);
    if (ret ==-1) {
       perror("readlink");
       exit(1);
    }
    exe[ret] = 0;
    g_commandName =  strdup(exe);

    // init needed variables
    unsigned int scale = 65536;

    // get memory for histogram
    unsigned short * histogram = (unsigned short *) malloc(g_histogram_size * sizeof(unsigned short));
    memset(histogram, 0, g_histogram_size * sizeof(unsigned short));

    // save histogram address to global var
    g_histogram_address = histogram;

    // start profiling
    profil(histogram, g_histogram_size * sizeof(unsigned short), g_code_origin, scale);
}

void print_histogram()
{
    // get histogram from where we stashed it
    unsigned short * histogram;
    histogram = g_histogram_address;

    // we should exit now
    if (!histogram)
    {
        return;
    }
 
    // get name of command used to call
    char filename[1024];
    snprintf(filename, 1024 - 1, "%s.hist", g_commandName);

    FILE * outfile = fopen(filename, "w");

    if (outfile == (FILE *)NULL)
    {
        assert(0);
    }
    else
    {
        // iterates through histogram
        for (int i = 0; i < g_histogram_size; i++)
        {
            // get output if non-zero
            if (histogram[i])
            {
                fprintf(outfile, "0x%lx\t%4dms\n", 2 * i + g_code_origin - 2, histogram[i] * 10);
            }
        }
        // close out file
        fclose(outfile);
    }

    // free histogram memory
    free(histogram);
    histogram = NULL;
}

