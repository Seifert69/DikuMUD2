#include <stdlib.h>
#include <stdio.h>
#include <time.h>


/* Calculate how many ways you can roll <target> with            */
/*    <num_dice>d<dice_size>  dices                              */
/* <target> must be less than total possible                     */
int dice_num(int num_dice, int dice_size, int target)
{
  int i, res;

  if (num_dice == 1)
    return (target <= dice_size);

  for (res=0, i=1; (i <= dice_size) && ((target-i) >= 1); i++)
    res += dice_num(num_dice-1, dice_size, target-i);

  return res;
}



int number(int from, int to)
{
  return ((rand() % (to-from+1)) + from);  
}



int dice(int num, int size)
{
  int sum;

  for (sum=0; num > 0; num--)
    sum += ((rand() % size) + 1);

  return sum;
}



int main(int argc, char *argv[])
{
  int num, size, i, j, accum, rev_accum, res, potens, old_res;

  if (argc != 3)
  {
    printf("Syntax:\n");
    printf("Dices <x> <y>\n");
    printf("Where x is number of dices, and y is the dice size.\n");
    exit(0);
  }

  srand((unsigned int) time(0));
  num = atoi(argv[1]);
  size = atoi(argv[2]);

  for (i=num-1, potens=size; i > 0; i--)
    potens *= size;
  printf("Probability Chart for %dD%d\n\n", num, size);
  printf("Average : %6.1f\n", (float) (num+num*size)/2.0);
  printf("Number of possible rolls : %d\n\n", potens);

  printf("          Probability        Accumulated    Reverse Accumulated\n");
  printf("Roll    Stroke  Percent    Stroke  Percent    Stroke  Percent\n");

  rev_accum = potens;
  old_res = 0;
  for (res=accum=0, j=num*size, i=num; i<=j; i++)
  {
    res = dice_num(num, size, i);
    accum += res;
    rev_accum -= old_res;

    printf(" %3d   %6d   %6.2f%%   %6d   %6.2f%%   %6d   %6.2f%%\n",
      i,
      res,
      (double) ((100.0*res)/potens),
      accum,
      (double) ((100.0*accum)/potens),
      rev_accum,
      (double) ((100.0*rev_accum)/potens)
    );
    old_res = res;
  }

  return 0;
}
