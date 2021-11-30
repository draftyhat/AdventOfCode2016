#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef DEBUG
#define DBGPRINT(...) printf(__VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define ROOM_NAME_MAX 1000  /* room names max 1000 characters */
#define CSUM_LENGTH 5


int is_real(unsigned long * sector, const char * room_name, int room_name_length)
{
    /* will hold frequency counts for our letters. Indexed by the index of the
     * letter in the alphabet; for any char ch, that's (ch - 'a'). */
    struct {
        int occurrences;
        int in_csum;
    } freqs[26];
    int max_not_csum = 0;
    char ch_not_csum = 'z';
    int i;
    char ch, ch2;
#define csum_start_index (room_name_length - CSUM_LENGTH - 1)

    memset(freqs, 0, sizeof(freqs));

    /* parse out checksum, mark is_csum in freqs
     * name format (csum characters 'C'):
     *   xxxxxxxx[CCCCC]
     * so that's the -(CSUM_LENGTH + 1)th character to the -2nd */
    for(i = 0; i < CSUM_LENGTH; i++)
    {
        /* this is our character */
        ch = room_name[csum_start_index + i];
        /* just mark it as "in_csum" */
        freqs[ch - 'a'].in_csum = 1;
    }

    /* now do frequency counts for all letters in the name string
     * while we're processing the input, keep track of the max frequency count
     * of any letter not in the checksum. */
    /* for every non-checksum character: */
    for(i=0; i < csum_start_index - 1; i++)
    {
        ch = room_name[i];
        /* skip '-'s (actually, skip non-alphabetic characters) */
        if(('a' <= ch) && (ch <= 'z'))
        {
            /* increment occurrences for this character */
            freqs[ch - 'a'].occurrences++;
            /* if this is not a checksum character */
            if(!freqs[ch - 'a'].in_csum)
            {
                /* update currently tracked max */
                if(max_not_csum > freqs[ch - 'a'].occurrences)
                {
                    max_not_csum = freqs[ch - 'a'].occurrences;
                    ch_not_csum = ch;
                }
                else if(max_not_csum == freqs[ch - 'a'].occurrences)
                {
                    /* also keep track of the lowest max-occurrence
                     * character */
                    if(ch < ch_not_csum)
                        ch_not_csum = ch;
                }
            }
        }

        /* if this is a digit character, assume the sector name starts here and
         * parse out the integer */
        else if(('0' <= ch) && (ch <= '9'))
        {
            *sector = strtoul(&room_name[i], NULL, 10);
            /* exit loop */
            break;
        }
    }

    /* first checksum validation. Verify that the last character in the
     * checksum either has more occurrences than any non-checksum character, or
     * that it has the same number but is alphabetically before the
     * non-checksum character with the max number of occurrences. */
    ch = room_name[csum_start_index + CSUM_LENGTH - 1];
    if((freqs[ch - 'a'].occurrences > max_not_csum)
             || ((ch < ch_not_csum) &&
                 (freqs[ch - 'a'].occurrences == max_not_csum)))
    {
        DBGPRINT(" Last csum character %c validated\n", ch);
    }
    else
    {
        DBGPRINT(" Last csum character %c has fewer occurrences (%d) than"
               "  %c (%d)\n", ch, freqs[ch - 'a'].occurrences,
               ch_not_csum, max_not_csum);
        return 0;
    }

    /* validate checksum. For each character in the checksum, check that it's
     * either more common than the next character, or that the number of
     * occurrences is equal and it comes alphabetically before the next
     * character. */
    for(i=0; i < CSUM_LENGTH - 1; i++)
    {
        ch = room_name[csum_start_index + i];
        ch2 = room_name[csum_start_index + i + 1];
        if((freqs[ch - 'a'].occurrences > freqs[ch2 - 'a'].occurrences)
                 || ((ch < ch2) &&
                     (freqs[ch - 'a'].occurrences == freqs[ch2 - 'a'].occurrences)))
        {
            /* good */
            DBGPRINT("  validated checksum character [%d] %c, %d occurrences\n",
                    i, ch, freqs[ch - 'a'].occurrences);
        }
        else
        {
            DBGPRINT("  checksum character [%d] %c does not validate;"
                   " %d occurrences, next char %c with %d occurences\n", 
                   i, ch, freqs[ch - 'a'].occurrences, ch2,
                   freqs[ch2 - 'a'].occurrences);
            return 0;
        }
    }

    return 1;
}

int main()
{
    char line[ROOM_NAME_MAX];
    int scanf_ret;
    unsigned long sector_sum = 0, sector;

    /* for each line (room) in stdin */
    scanf_ret = scanf("%s", line);
    while(scanf_ret > 0)
    {
        DBGPRINT("Parsing room %s\n", line);
        if(is_real(&sector, line, strlen(line)))
        {
            DBGPRINT(" Room %s is real!\n", line);
            sector_sum += sector;
        }
        else
        {
            DBGPRINT(" Room %s is not real\n", line);
        }
        scanf_ret = scanf("%s", line);
    }

    printf("Sum of all sectors: %lu\n", sector_sum);

    return 0;
}
