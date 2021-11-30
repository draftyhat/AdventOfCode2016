/* compile for keypad part 1: gcc -Wall -o 2016_02 2016_02.c */
/* compile for keypad part 2: gcc -DKEYPAD1 -Wall -o 2016_02 2016_02.c */
/* with debugging: gcc -DDEBUG -DKEYPAD1 -Wall -o 2016_02 2016_02.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#ifdef DEBUG
#define DBGPRINT(...) printf(__VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#ifdef KEYPAD1
#define KEYPAD_WIDTH 5
#define KEYPAD_HEIGHT 5
#else
#define KEYPAD_WIDTH 3
#define KEYPAD_HEIGHT 3
#endif

struct keypad_t {
    /* width of the keypad */
    int width;
    /* height of the keypad */
    int height;
    /* keypad[x][y] gives the element index on the keypad
     * not cartesian coordinates; 0,0 is top left, and
     * dereference goes y, x */
    char keypad[KEYPAD_HEIGHT][KEYPAD_WIDTH];
};

/** @brief Add 1 to a number, but keep it within a bounded range
 *  @param addend (IN) original number 
 *  @param max (IN) maximum 
 *  @return addend + 1, or addend if addend == max
 */
int add_one_bounded(int addend, int max)
{
    if(addend >= max - 1)
        return addend;
    return addend + 1;
}

/** @brief Subtract 1 from a number, but keep it >= 0
 *  @param minuend (IN) original number 
 *  @return minuend - 1, or minuend if minuend == 0
 */
int subtract_one_bounded(int minuend)
{
    if(minuend <= 0)
        return minuend;
    return minuend - 1;
}

/** @brief Make a move on the keypad
 *  @param next_x (IN) returned x position after move
 *  @param next_y (IN) returned y position after move
 *  @param keypad (IN) keypad representation
 *  @param current_x (IN) x position before move
 *  @param current_y (IN) y position before move
 *  @param move (IN) move character (ULDR)
 *  @return 0 if you made a move, -1 for error
 */
int move(
    int * next_x,
    int * next_y,
    struct keypad_t * keypad,
    int current_x,
    int current_y,
    char move)
{
    int retval = 0;

    /* translate the move to the move vector
        {  0,  1 },  * UP *
        { -1,  0 },  * LEFT *
        {  0, -1 },  * DOWN *
        {  1,  0 }   * RIGHT *
    */

    /* attempt to make the indicated move */
    DBGPRINT("--- making move %c from %d,%d (%c)\n", move, current_x,
            current_y, keypad->keypad[current_x][current_y]);
    switch(move) {
        case 'U':
            *next_x = current_x;
            *next_y = subtract_one_bounded(current_y);
            break;
        case 'L':
            *next_x = subtract_one_bounded(current_x);
            *next_y = current_y;
            break;
        case 'D':
            *next_x = current_x;
            *next_y = add_one_bounded(current_y, keypad->height);
            break;
        case 'R':
            *next_x = add_one_bounded(current_x, keypad->width);
            *next_y = current_y;
            break;
        default:
            printf("ERROR! I don't understand move %c\n", move);
            retval = -1;
            break;
    }
    printf("--- calculated move to %d,%d %c\n", *next_x, *next_y, keypad->keypad[*next_y][*next_x]);

    /* check for empty spots on the grid; if we landed on one, don't do the
     * move */
    if('\0' == keypad->keypad[*next_y][*next_x])
    {
        *next_x = current_x;
        *next_y = current_y;
    }
    printf("--- move landed on %c\n", keypad->keypad[*next_y][*next_x]);
    return retval;
}

/** Given a set of instructions, find the key
 *  @param instructions (IN) slab of NULL-terminated sequences of instructions
 *  @param key_size (IN) Number of sets of instructions
 *  @return 0 upon success, -1 else */
int find_answer(char * instructions, int key_size)
{
    struct keypad_t keypad = {
        KEYPAD_WIDTH, KEYPAD_HEIGHT,
#ifdef KEYPAD1
        { "\000\0001\000\000", "\000234\000", "56789", "\000ABC\000", "\000\000D\000\000" }
#else
        { "123","456","789" }
#endif
    };
    const char * instruction_pointer;
    int answer_index;
    char * answer;
    int x, y;

    DBGPRINT("--ninstructions: %d\n", key_size);
    /* allocate array for answers */
    answer = (char *) malloc(key_size + 1);
    if(!answer)
    {
        printf("ERROR allocating %u bytes for answer\n", key_size + 1);
        return -1;
    }

    instruction_pointer = instructions;
    /* for each set of instructions (so, for each character in the answer) */
    for(answer_index = 0;
            answer_index < key_size;
            answer_index++)
    {
        DBGPRINT("-- starting to find %d\n", answer_index);
        /* start in the center of the keypad */
        x = keypad.width / 2;
        y = keypad.height / 2;

        /* for each instruction in this set */
        for(;
                *instruction_pointer;
                instruction_pointer++)
        {
            /* make the next move */
            move(&x, &y, &keypad, x, y, *instruction_pointer);
        }
        instruction_pointer++;

        /* all done making moves. Record this answer. */
        answer[answer_index] = keypad.keypad[y][x];
        DBGPRINT("-- %d: %c\n", answer_index, answer[answer_index]);
    }

    /* print the answer */
    answer[answer_index] = '\0';
    printf("Answer: %s\n", answer);
    free(answer);

    return 0;
}

int main(int argc, char ** argv)
{
    /* assume stdin gets lines of instructions */
    char * instructions = NULL;
    int ninstructions = 0;
    ssize_t instructions_size = 0;

    /*  cheat: assume instructions are no more than 4096 bytes */
#define MAX_INSTRUCTIONS_SIZE 4096
    instructions = (char *) malloc(MAX_INSTRUCTIONS_SIZE);
    if(!instructions)
    {
        printf("ERROR allocating %d bytes for instructions!\n",
                MAX_INSTRUCTIONS_SIZE);
        return -1;
    }

    instructions_size = read(0, instructions, MAX_INSTRUCTIONS_SIZE - 1);
    if(-1 == instructions_size)
    {
        perror("ERROR reading instructions:");
        free(instructions);
        return -1;
    }
    if(instructions_size >= MAX_INSTRUCTIONS_SIZE - 1)
    {
        printf("ERROR: instructions more than max length (%d)!\n",
                MAX_INSTRUCTIONS_SIZE);
        free(instructions);
        return -1;
    }
    instructions[instructions_size] = '\0';

    /* replace newlines with '\0' */
    for(int i=0; i < instructions_size; i++)
    {
        if('\n' == instructions[i])
        {
            instructions[i] = '\0';
            ninstructions++;
        }
    }

    find_answer(instructions, ninstructions);

    free(instructions);
}

