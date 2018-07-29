#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// Throughout this file, an unsigned int will be used to represent a set of choices.
// Consider it to be a set of bools as opposed to an int.

// Utility function for printing out binary numbers.
// Useful for debugging.
void printBin(unsigned bin)
{
    unsigned mask = (~0U >> 1) + 1;
    do
    {
        putchar(mask & bin ? '1' : '0');
    }
    while(mask >>= 1);
    putchar('\n');
}

// Read a vote from stdin, stopping at newline.
unsigned readVote(void)
{
    unsigned vote = 0;
    char buf[4];
    short inputnum;
    for(fgets(buf, sizeof buf, stdin); strcmp(buf, "\n") != 0; fgets(buf, sizeof buf, stdin))
    {
        inputnum = atoi(buf);
        vote |= 1 << inputnum;
    }
    return vote;
}

// As per the definition of proportional approval voting
double getScore(unsigned intersectionSize)
{
    double sum = 0;
    for(unsigned i = 1; i <= intersectionSize; i++)
        sum += 1.0/(double)i;
    return sum;
}

void printResult(unsigned permutation)
{
    unsigned pos;
    for(pos = 0; pos < 32; ++pos)
        if((1 << pos) & permutation)
            printf("%u ",pos);
    putchar('\n');
}

bool validateInput(unsigned nomineeCount, unsigned seatCount, unsigned voteCount)
{
    if(nomineeCount > 32)
    {
        perror("Error: maximum of 32 nominees currently supported");
        return false;
    }
    if(seatCount > nomineeCount)
    {
        perror("Error: number of nominees must exceed number of seats");
        return false;
    }
    return true;
}

// According to Knuth in "Generating All Tuples and Permutations", this algorithm 
// can be attributed to Narayana Pandita from 14th century India.
unsigned nextPermutation(unsigned permutation)
{
    unsigned right, left;
    // Find the rightmost '01'
    for(left = 02; !( (left & ~permutation) && ((left >> 1) & permutation) ); left <<= 1);
    // Find the rightmost '1'
    for(right = 01; !(right & permutation); right <<= 1);
    // Swap them
    permutation |= left;
    permutation ^= right;
    // Reverse the order of everything less significant than left
    for(left >>= 1, right = 01; left > right; left >>= 1, right <<= 1)
        if(!!(left & permutation) ^ !!(right & permutation))
            permutation ^= left, permutation ^= right;
    return permutation;
}

// bits to the left of the 'nomineeCount'th bit are zero and ignored
unsigned getLastPermutation(unsigned seatCount, unsigned nomineeCount)
{
    unsigned lastPermutation = 0;
    while(seatCount)
        lastPermutation |= 1 << nomineeCount - seatCount--;
    return lastPermutation;
}

unsigned getFirstPermutation(unsigned seatCount)
{
    unsigned firstPermutation = 0;
    while(seatCount--)
        firstPermutation |= 1 << seatCount;
    return firstPermutation;
}

int main(int argc, char *argv[])
{
    unsigned nomineeCount, seatCount, voteCount;

    scanf("%u %u %u", &nomineeCount, &seatCount, &voteCount);
    if(!validateInput(nomineeCount, seatCount, voteCount))
        return 1;

    getchar(); // clear '\n' from input buffer

    // read votes from stdin
    unsigned *votes = (unsigned *)malloc(voteCount * sizeof(unsigned));
    for(unsigned i = 0; i < voteCount; i++)
        votes[i] = readVote();

    const unsigned firstPermutation = getFirstPermutation(seatCount);
    const unsigned lastPermutation = getLastPermutation(seatCount, nomineeCount);

    unsigned currentPermutation;
    unsigned bestPermutationSoFar = firstPermutation;
    double bestScoreSoFar = 0;


    // Iterate through the seatCount-element subsets of the nomineeCount nominees
    for(currentPermutation = firstPermutation; currentPermutation <= lastPermutation; currentPermutation = nextPermutation(currentPermutation))
    {
        // get score for the currentPermutation
        double currentScore = 0;
        for(unsigned i = 0; i < voteCount; i++)
            currentScore += getScore(__builtin_popcount(votes[i] & currentPermutation));

        if(currentScore > bestScoreSoFar)
        {
            bestPermutationSoFar = currentPermutation;
            bestScoreSoFar = currentScore;
        }
    }

    free(votes);
    
    printResult(bestPermutationSoFar);
    
    return 0;
}
