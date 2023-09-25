/* Module to control menu for socialNetwork program
 *   Created by Sally Goldin 16 September 2021
 */
#include <stdio.h>
#include <string.h>

/* Present a menu of actions and ask the user which one they
 * want. Keep asking until a valid option is provided.
 * Return the selected option.
 */
int getMenuChoice()
{
  char input[32];
  int choice = 0;
  printf("\nAvailable actions:\n");
  printf("\t1 - Create a new user\n");
  printf("\t2 - Show a user's friends\n");
  printf("\t3 - Suggest new friends\n");
  printf("\t4 - Add new friends\n");
  printf("\t5 - Exit the program\n");
  while ((choice < 1) || (choice > 5))
    {
    printf("What action? ");  
    fgets(input,sizeof(input),stdin);
    sscanf(input,"%d",&choice);
    }
  return choice;
}

