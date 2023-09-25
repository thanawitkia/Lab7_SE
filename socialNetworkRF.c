/**
 * Simple social network
 * This code reads a file that defines vertices and edges
 * and builds a graph using the graph ADT defined in abstractGraph.h.
 * It then allows the user to search for users, display their friends,
 * suggest new friends, and add new friends.
 * Version created 16 September 2021 with deliberate Code Smells,
 * for CPE327 Refactoring Exercise
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "abstractGraph.h"

/* Structure type for user in the social network
 * This will be the data stored at a vertex of the network. 
 */
typedef struct
{
  char username[32];
  char firstname[32];
  char lastname[32];
  char birthday[16];
} USER_T;     

/* List items for the adjacency list.
 * Each one represents an edge leading to an existing vertex
 */
typedef struct _adjVertex
{
    void * pVertex;           /* pointer to the VERTEX_T this 
                               * item refers to.
                               */
    struct _adjVertex* next;  /* next item in the ajacency list */ 
} ADJACENT_T;

/* Vertex structure - repeated from linkedListGraph */
typedef struct _vertex
{
    char * key;               /* key for this vertex */
    void * data;              /* additional data for this vertex */
    int color;                /* used to mark nodes as visited */
    struct _vertex * next;    /* next vertex in the list */
    ADJACENT_T * adjacentHead;    /* pointer to the head of the
		                   * adjacent vertices list
                                   */
    ADJACENT_T * adjacentTail;    /* pointer to the tail of the
			           * adjacent vertices list
                                   */
}  VERTEX_T;

/* declaration of function not in abstractGraph.h, to avoid warnings */
VERTEX_T * findVertexByKey(char* key, VERTEX_T** pPred); 

/* Initializes and builds the network based on
 * the information in the file.
 * Returns 1 if successful, 0 if any error occurred
 */
int buildNetwork(char * filename)
{
  FILE* pIn = NULL;
  char inputline[128];
  char username[32];
  char username2[32];  /* second user in a 'FRIEND' line */
  char firstname[32];
  char lastname[32];
  char birthday[16];
  USER_T * pUser = NULL;   /* used for dynamically allocating users */
  int status = 1; /* used to test for errors */
  /* try to open the file */
  pIn = fopen(filename,"r");
  if (pIn == NULL)
    {
    printf("Error - cannot open file '%s'\n", filename);
    return 0;  
    }
  /* since we can open the file, go ahead and initialize the graph */
  status = initGraph(1000,0); /* this should be an UNDIRECTED graph */
  if (!status)
    {
    printf("Error initializing the graph\n");
    return status;  
    }
  /* Let's read the file and build the network */
  while (fgets(inputline,sizeof(inputline),pIn) != NULL)
    {
      printf("read %s", inputline);
    /* if this is a FRIEND specification, create an edge */  
    if (strncmp(inputline,"FRIEND",strlen("FRIEND")) == 0)
      {
      if (sscanf(inputline,"FRIEND %s %s",username,username2) != 2)
	{
	printf("Invalid format for FRIEND line: '%s'",inputline);  
	}
      else
	{
	status = addEdge(username,username2);
	if (status == 0)
	  printf("At least one user ('%s' and '%s') does not exist\n",
		 username,username2);
	else if (status < 0)
	  printf("'%s' and '%s' are already friends\n",
		 username,username2);
	else
	  printf("'%s' and '%s' are now friends\n", username,username2);
	}
      }
    else /* this is a user (VERTEX) */
      {
      if (sscanf(inputline,"%s %s %s %s",username,firstname,lastname,birthday) != 4)
          printf("Invalid input line, skipping: '%s'",inputline);
      else
          {
	  VERTEX_T * dummy;  
	  VERTEX_T* v = findVertexByKey(username,&dummy);  
	  if (v != NULL)
	    {
	    printf(">> ERROR in data file! User '%s' already exists\n",username);
	    }
	  else
	    {
	    pUser = calloc(1,sizeof(USER_T));
	    if (pUser != NULL)
	      {
	      strcpy(pUser->username,username);  
	      strcpy(pUser->firstname,firstname);  
	      strcpy(pUser->lastname,lastname);  
	      strcpy(pUser->birthday,birthday);
	      status = addVertex(username,pUser);
	      if (status == 0)
		printf(">> Memory allocation error adding user '%s'\n",username);
	      else if (status < 0)
		printf(">> ERROR in data file! User '%s' already exists\n",username);
	      else   
		printf("Successfully added user '%s' to the social network\n",username);
	      }
	    else
	      {
	      printf("Memory allocation error creating a user\n");
	      return 0;
	      }
	    }
	  }	
      }
    }
  fclose(pIn);
  return 1;
}

/* Create a new user and add to the social network.
 *   username     Username for the new user 
 * Returns pointer to the USER_T constructed by this function
 * or NULL if an error.
 */
USER_T * addUser(char* username)
{
  USER_T * newUser = NULL;
  char input[32];
  newUser = calloc(1,sizeof(USER_T));
  if (newUser != NULL)
    {
    printf("\nCreating a new user profile for '%s'\n",username);  
    strcpy(newUser->username,username);   
    printf("\tWhat is your first name? ");
    fgets(input,sizeof(input),stdin);
    sscanf(input,"%s",newUser->firstname);
    printf("\tWhat is your last name? ");
    fgets(input,sizeof(input),stdin);
    sscanf(input,"%s",newUser->lastname);
    printf("\tWhat is your birthday (dd-mm-yyyy)? ");
    fgets(input,sizeof(input),stdin);
    sscanf(input,"%s",newUser->birthday);
    if (addVertex(username,newUser) != 1)
      {
      printf("Error adding new user\n");
      free(newUser);
      newUser = NULL;
      }
    }
  return newUser;
}

/* Create friend relationships with the focus user
 *  focusUser      User for whom we are creating friends
 *                 Assumes this user exists
 */
void addFriends(USER_T * focusUser)
{
  char input[32];
  char friendname[32];
  int status = 1;
  printf("\nAdding friends for user '%s %s (%s)'\n",focusUser->firstname,
	 focusUser->lastname,focusUser->username);  
  while (1)
    {
    printf("\tWho do you want to add (username)? (DONE to stop) ");
    fgets(input,sizeof(input),stdin);
    sscanf(input,"%s",friendname);
    if (strcasecmp(friendname,"DONE") == 0)
      break;
    if (strcmp(friendname,focusUser->username) == 0)
       printf("\t>>You can't add yourself as a friend!\n");
    else
      {
       status = addEdge(focusUser->username,friendname);
       if (status == 0)
          printf("\t>> User '%s' does not exist\n",friendname);
       else if (status < 0)
          printf("\t>> User '%s' is already your friend\n",friendname);
       else
          printf("\t>> User '%s' is now your friend\n",friendname);
      }
    }
}


/* Look through a list of existing friend usernames
 * to make sure the target username does not appear.
 *    targetUsername           Name we're looking for
 *    friendnames              Array of keys
 *    count                    Size of friendnames
 * Returns 1 if found, 0 if not found
 */
int searchExisting(char* targetUsername, char** friendnames, int count)
{
  int i = 0;
  int found = 0;
  for (i = 0; (i < count) && (!found); i++)
    if (strcmp(targetUsername,friendnames[i]) == 0)
      found = 1;
  return found;
}

/* This function will print the names of all the friends
 * of the passed user. It will return the number of friends
 * found (which could be 0).
 * The function is used both for simply printing friends of a user (Case1)
 * and for suggesting new friends (by printing the friends of friends) (Case 2)
 *   pUser                  User whose friends we are printing
 *   pExcludeUser           If non-NULL, we are printing suggestions (Case 2)
 *                          So we will not print the header, and we
 *                          will exclude this username if he or she
 *                          appears in the suggestion list
 *   userFriends            Only specified if pExcludeUser is non-null (Case 2)
 *                          holds the keys for all of pExcludeUser's friends
 *                          so we don't suggest someone with whom the
 *                          user is already connected.
 *   count                  0 if userFriends is NULL, else size of userFriends
 */
int printFriends(USER_T * pUser,  USER_T * pExcludeUser,
		 char** userFriends, int count)
{
  int i = 0;
  int friendCount = 0;
  int adjustedFriendCount = 0;  /* number of friends who were not excluded */
  char** friendKeys = getAdjacentVertices(pUser->username,&friendCount);
  if (friendCount > 0)
    {
    adjustedFriendCount = friendCount;  
    if (pExcludeUser == NULL)  
       printf("Here is a list of your current friends:\n");
    for (i = 0; i < friendCount; i++)
      {
      USER_T * friend = findVertex(friendKeys[i]);
      if (friend == NULL)
	printf("Something is wrong - can't find USER_T for key '%s'\n",
	       friendKeys[i]);
      else if (friend == pExcludeUser) /* exclude me! */
	adjustedFriendCount--;
      else 
	{
	/* exclude people who are already my friends */  
	if (!searchExisting(friendKeys[i],userFriends,count))
	    {
	    printf("\t%s %s (%s) - Birthday %s\n",
	       friend->firstname,friend->lastname,friend->username,
	       friend->birthday);
	    }
	else
	   adjustedFriendCount--;
	}
      }
    }
  else
    {
    if (pExcludeUser == NULL) /* Case 1, when there are 0 adjacents */
	printf("You do not have any friends yet\n");
    }
  if (friendKeys != NULL)
    free(friendKeys);
  return adjustedFriendCount;
}

/* Suggest as new friends people who are friends of your friends.
 *   pUser -- focus user for whom we are suggesting friends.
 */
void suggestFriends(USER_T * pUser)
{
 int i = 0;
 int dummyCount = 0;
 int friendCount = 0;
 char** friendKeys = getAdjacentVertices(pUser->username,&friendCount);
 if (friendCount > 0)
    {
    printf("\n-- Here are some people you might like to know --\n");  
    for (i = 0; i < friendCount; i++)
      {
      USER_T * friend = findVertex(friendKeys[i]);
      if (friend != NULL)
	  {
	  printf("  Friends of %s whom you might like: \n",friend->username);  
	  dummyCount = printFriends(friend,pUser,friendKeys,friendCount);
	  if (dummyCount == 0)
   	     printf("\t>> %s has no friends who aren't already your friends\n",friend->username);
	  }
      else
	  printf("\t\tSomething is very wrong - no user associated with username '%s'?\n", friendKeys[i]);
      }
    }
}

/* main function builds the graph, then enters a loop 
 * doing different operations 
 */
int main(int argc, char* argv[])
{
  int status = 1;
  int friendcount = 0;
  char inputline[32];
  char username[32] ="";
  int option = 0;
  USER_T * myProfile = NULL;
  if (argc < 2)
    {
    printf("Please provide an input file name: ./socialNetwork [file]\n");
    exit(0);
    }
  status = buildNetwork(argv[1]);
  if (!status)
    {
    printf("Fatal error building the social network\n");
    exit(1);
    }
  while (option != 5)
    {
    option = getMenuChoice();
    switch(option)
      {
      case 1:
	printf("\nEnter new username: ");
	fgets(inputline,sizeof(inputline),stdin);
	sscanf(inputline,"%s",username);
	myProfile = findVertex(username);
	if (myProfile == NULL)
	  {
	  myProfile = addUser(username);
	  }
	else
	  {
	  printf("\tSorry, that username is already in use\n");
	  }
	break;
      case 2:
	printf("\nPrint friends for what user? ");
	fgets(inputline,sizeof(inputline),stdin);
	sscanf(inputline,"%s",username);
	myProfile = findVertex(username);
	if (myProfile != NULL)
	  {
	  friendcount = printFriends(myProfile,NULL,NULL,0);
	  }
	else
	  {
	  printf("\tNo user exists with that username\n");
	  }
	break;
      case 3:
	printf("\nSuggest friends for what user? ");
	fgets(inputline,sizeof(inputline),stdin);
	sscanf(inputline,"%s",username);
	myProfile = findVertex(username);
	if (myProfile != NULL)
	  {  
	  friendcount = printFriends(myProfile,NULL,NULL,0);
	  if (friendcount > 0)
	    suggestFriends(myProfile);
	  }
	else
	  {
	  printf("\tNo user exists with that username\n");
	  }
	break;
      case 4:
	printf("\nAdd friends for what user? ");
	fgets(inputline,sizeof(inputline),stdin);
	sscanf(inputline,"%s",username);
	myProfile = findVertex(username);
	if (myProfile != NULL)
	  {
	  addFriends(myProfile);
	  }
	else
	  {
	  printf("\tNo user exists with that username\n");
	  }
	break;
      case 5:
	break;
      }
    }
  printf("\nGoodbye!\n");
}
