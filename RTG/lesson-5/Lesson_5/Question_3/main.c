/*
The implementation of Shell:
1) The programm runs and gets as an input shell commands and runs them.
2) The program can show the history of the commands.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The definidion for input commands linked list node
typedef struct Node
{
	int		     number; 
	char* 		 value ;
	struct Node* next  ;
}node;

//Globals - the head and the tail of the linked list of the input commands
node* head = NULL;
node* tail = NULL;
int seqNumber = 0;

//Prototypes - few functions for linked list operations

void  Add	(char* value);
node* InitNode  (char* value);
void  PrintNode (node* ptr  );
void  PrintList (	    );
/*********************************************************************/
int main()
{
	char read_buffer[80] = {0};
	char* username = getenv("USER");
	char* hostname = getenv("HOSTNAME");
	int command_number;

	while(1)
	{
		memset (read_buffer, 0, 80);
		printf("[%s@%s ~]# ", username, hostname); 	//the beggining of the shell line
		fgets(read_buffer, 80, stdin);			//gets the input command into the buffer
		if (0 == strcmp(read_buffer, "exit\n"))		//if "exit" - finish running the program
			break;
		if (0 == strcmp(read_buffer, "\n"))		//when 'enter' is pressed without any command - just make new line
			continue;
		if (0 == strcmp(read_buffer, "history\n"))	//when "history" command - display all the previous commands
			PrintList();
		else
			system(read_buffer);			//run the input command
		Add(read_buffer);				// at the end, add the input command to the list of input commands (for history)
	}
	return 0;
}
/*********************************************************************/
void Add(char* value)
{
	node* new;
	new = InitNode(value);

	if (new == NULL)
		return;

	if( head == NULL )  
	{
       		head = new; 
		tail = new;
		return;       
	}
	
	tail->next = new;
	tail = tail->next;     
}
/*********************************************************************/
node* InitNode(char* value)
{
   node* ptr;
   ptr = (node*) calloc( 1, sizeof(node) );
   if( ptr == NULL )                       
       return (node *) NULL;        	
   else 
   {                                  
	ptr->number = seqNumber++;
	ptr->value = strdup( value );     
	ptr->next = NULL;     
       return ptr;                         
   }
}
/*********************************************************************/
void PrintNode(node* ptr)
{
	printf("%d %s", ptr->number, ptr->value);
}
/*********************************************************************/
void PrintList()
{
	node* currPtr = head;
	while (currPtr != NULL)
	{
		PrintNode(currPtr);
		currPtr = currPtr->next;
	}
}
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
