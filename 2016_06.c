#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define MESSAGE_LENGTH 8


int main(int argc, char ** argv)
{
    char message[MESSAGE_LENGTH + 1];
    int freqs[MESSAGE_LENGTH][26];
    int target_freq;
    char target_char;
    int i,j;

    memset(freqs, 0, sizeof(freqs));

    /* read in each message and log its characters */
    while(1 == scanf("%s", message))
    {
        for(i=0; i < MESSAGE_LENGTH; i++)
        {
            if('a' <= message[i] && message[i] <= 'z')
                freqs[i][message[i] - 'a']++;
        }
    }
    /* for each column, pick out the target character */
    for(i=0; i < MESSAGE_LENGTH; i++)
    {
        target_freq = freqs[i][0];
        target_char = 'a';
        for(j=1; j < 26; j++)
        {
#ifdef PART2
            if(freqs[i][j] < target_freq)
#else
            if(freqs[i][j] > target_freq)
#endif
            {
                target_freq = freqs[i][j];
                target_char = 'a' + j;
            }
        }
        printf("%c", target_char);
    }

    printf("\n");
}
