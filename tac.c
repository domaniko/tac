/* 
  Reverse cat
  
  Purpose: reverse all lines in a text file, i.e. last is first, first is last
  
  - Check if file exists
  - load all lines in memory
  - save all lines to new file, if given as arg, or to screen

  V1.00 - 2019.09.19   Initial working version

*/

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
/* Colour codes for Linux terminal */
#define NORMAL_COLOR  "\x1B[0m"
#define GREEN  "\x1B[32m"
#define BLUE  "\x1B[34m"
#define RED  "\x1B[31m"

/* Size definition of text lines */
#define MAX_LINE_LEN 32766    /* text lines possibly can get big */
#define BUFFER_FOR_LF 2       /* possibly add LF to end of string */


/* main new data type: one element of the linked list, which can point to next */
typedef struct line_list {
    char *textline;
    struct line_list * next;
} line_list_t;                

line_list_t *add_text_line_as_first_list_element(line_list_t *p_current_list,char *line)
/*
  Purpose: Store the content of one text line at the beginning of the linked list
  
  Param:   p_current_list   pointer to current head of list, on which the new line shall be added at the beginning
           line             text line to be added to linked list
           
  Return:  new head of list
*/
{
   line_list_t *p_new_list_entry=malloc(sizeof(line_list_t));
   if (line != NULL)
   {
     p_new_list_entry->textline=malloc(strlen(line)+BUFFER_FOR_LF);
     strncpy(p_new_list_entry->textline,line,MIN(strlen(line)+BUFFER_FOR_LF,MAX_LINE_LEN));
   }
   else
   {
     p_new_list_entry->textline=NULL;
   }
   
   p_new_list_entry->next=p_current_list;
   
   return p_new_list_entry;
}

void add_linefeed_at_last_line(line_list_t *p_list)
/*
  Purpose: Special handling of last line needed, as it may not contain a line feed anymore, but should in the reversed file
  
  Param:   p_list   pointer to current head of list, the first entry is the last read line
*/
{
  if (p_list !=NULL)
  {
    int i;
    int length_of_string=strlen(p_list->textline);

    for ( i=0; i<strlen(p_list->textline); i++)
    {
      if(p_list->textline[i]=='\n')
      {
        break;
      }  
    }
    if (i == length_of_string)
    {  
      strncat(p_list->textline,"\n",BUFFER_FOR_LF);
    }
  }     
}
  
line_list_t *read_text_lines_from_file(char *filename)
/*
  Purpose: read all lines from file and store them in linked list
  
  Param:   filename   name of file to be reverted
           
  Return:  head of linked list
*/
{ 
  line_list_t *p_head=NULL;
  FILE * file;
  char text_line[MAX_LINE_LEN];
 
  file = fopen(filename,"r"); 
  
  if (file != NULL)  /* could possibly omitted as file existence is checked already before */
  {
    while (fgets(text_line, MAX_LINE_LEN-1,file) != NULL) /* read one line from text file until file ends */
    {
      p_head = add_text_line_as_first_list_element(p_head,text_line);
    }
    add_linefeed_at_last_line(p_head); /* special handling needed, in case last line does not have line feed */
  }
  return p_head; /* p_head points now to the reversed list of test lines */
}

int write_reversed_lines(line_list_t *head, char *filename)
/*
  Purpose: write all lines stored in the linked list to a new file 
  
  Param:   filename   name of file to be reverted or NULL, which means writing to standard output
           
  Return:  0 for success, 1 for error (either file cannot be opened or cannot be written)
*/
{
  FILE * file;
  line_list_t *current_list_entry = head;
  int write_error=0;
  
  if (filename != NULL)
  {
    file = fopen(filename,"w"); /* try to open file with file name */
  }
  else
  {
    file = stdout;
  }
  
  if (file != NULL)  
  {  
    while (current_list_entry != NULL)
    {
      line_list_t *old_list_entry=current_list_entry;    
      if (current_list_entry->textline != NULL)
      { 
        if (write_error!=EOF)
        {  
          write_error = fputs(current_list_entry->textline, file);
          /* fputs() returns EOF in case of error */
        }  
        free(current_list_entry->textline);
      }
      current_list_entry=current_list_entry->next;
      free(old_list_entry);
    }  
  }
  else
  {
    return EXIT_FAILURE;
  }
  if (file != stdout)
  {
    fclose(file);
  }  
  return (write_error !=EOF ? EXIT_SUCCESS : EXIT_FAILURE);
}

int reverse_file(int argc, char **argv)
/*
  Purpose: Read the text and write in reversed order, either to file or to stdout (if no second command line argument given)
  
  Param:   command line arg counter and argunments
           
  Return:  0 for success, 1 for error
*/
{
  line_list_t *head;
  
  head = read_text_lines_from_file(argv[1]);
  
  if (head != NULL) /* that would e.g. happen with empty file */
  {
    int write_result=write_reversed_lines(head,((argc>2) ? argv[2] : NULL));
    return(write_result); 
  }
  return(EXIT_FAILURE);
}

int check_command_line_arguments(int argc, char **argv)
/*
  Purpose: basic consistence and error checking of given arguments:
           - Arguments given?
           - Does source file exist?
           - Does destination file not exist?
  
  Param:   command line arg counter and argunments
           
  Return:  EXIT_SUCCESS for success, EXIT_FAILURE for error
*/
{
  if (argc <= 1)
  {
    printf("%s\n", NORMAL_COLOR);
    printf("%sUsage: %s%s <source file> [<destination file>]\n\n",GREEN,RED,argv[0]);
    printf("%s\n", NORMAL_COLOR);
    return(EXIT_FAILURE);
  }
  
  printf("%s\n", NORMAL_COLOR);
  
  if (argc > 1)
  {
    FILE *file = fopen(argv[1],"r");
    if (file == NULL)
    {
      printf("%s\n", NORMAL_COLOR);
      printf("%s%s: %s source file %s%s%s does not exist \n\n",GREEN,argv[0],RED,GREEN,argv[1],RED);
      printf("%s\n", NORMAL_COLOR);
      return(EXIT_FAILURE);
    }
    else 
    {
      fclose(file);
    }  
    if (argc > 1)    
    {
      file = fopen(argv[2],"r");
      if (file!=NULL)
      {
        fclose(file);
        printf("%s\n", NORMAL_COLOR);
        printf("%s%s: %s destination file %s%s%s already exists and would be overwritten \n\n",GREEN,argv[0],RED,GREEN,argv[2],RED);
        printf("%s\n", NORMAL_COLOR);
        return(EXIT_FAILURE);
      }        
    }  
  }    
  return(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
  int failure = check_command_line_arguments(argc,argv);
  
  if (!failure)
  {
    failure = reverse_file(argc,argv);
  }

  return failure;
}
