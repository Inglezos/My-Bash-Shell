#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h
#include <sys/wait.h>
#include <sys/types.h>

char *trim(char *str);
char *args_tokens(char *str);
void execute_tokens(char **tokens);


int main(int argc, char **argv)
{
	if (argc > 2)
	{
		printf("Parakalw eisagete mexri 1 orismata...\n");
		exit(1);
	}

	if (argc == 2)
	{
		printf("\n\n\n\n\n\n				***** BATCH MODE *****\n\n\n\n\n\n");		
		char pathfile[512];
		strcpy(pathfile,argv[1]);
		printf("Pathfile is : %s \n",pathfile);

		// Start of input file check

		// Check input file extension
	
		char *check_txt;

		check_txt=strstr(pathfile,".txt");
		if ( check_txt )		// if check_txt != NULL
		{
			check_txt=strstr(check_txt,"txt");
		}

		if ( ! check_txt  )		// if check_txt == NULL
		{
			printf("Input file with no extension. Assuming it is a .txt file... \n");
			strcat(pathfile,".txt");
		}
		else
		{		
			printf("Input file is .txt \n");
		}

		// Check if input file is valid
		if ( access( pathfile, F_OK ) != -1)
		{	
			printf("Input file %s exists! \n",pathfile);
		}
		else
		{
			printf("Input file %s doesn't exist! \nPlease enter a valid .txt file... \n",pathfile);
			exit (1);
		}


		// End of input file check

	
		FILE	*counter_fp;
		FILE *fp;
		if ( (counter_fp = fopen (pathfile, "r+")) == NULL)
		{
			printf("Error! Can't open input file! \n");
			exit (1);
		}
		int lines=0;
		int quit_counter=0;
		char counter_line[513];
		while (fgets(counter_line,sizeof(counter_line),counter_fp))
		{
			lines++;
		}
		fclose(counter_fp);

		if ( (fp = fopen (pathfile, "r+")) == NULL)
		{
			printf("Error! Can't open input file! \n");
			exit (1);
		}
		char line[513];
		char **tokens;
		int *line_tokens_number;
		int line_number=0;
		int tokens_counter=0;
		int quit_flag=1;
		int empty_file_flag = 1;
		int MAX_TOKENS = lines*170;
		tokens=(char **)malloc(MAX_TOKENS*sizeof(char *));
		line_tokens_number = (int *)malloc(MAX_TOKENS*sizeof(int));
		char *line_token;
		line_token = (char *)malloc(513*sizeof(char));
		for (int i=0;i<MAX_TOKENS;i++)
		{
			tokens[i] = (char *)malloc(513*sizeof(char));
			line_tokens_number[i] = 0;
		}

		while ( (fgets(line,sizeof(line),fp)) && (quit_flag) )
		{			
			empty_file_flag = 0;			
			if ( sizeof(line) > 513 )
			{
				printf(" Line characters exceeds 512. Please modify input file contents. \n Exiting... \n");
				exit (1);
			}

			line_token = strtok(line,";");
			while ( line_token != NULL )
			{				
				if (( strcmp(line_token,"" )) && ( strcmp(line_token,"\n" )) && ( strcmp(line_token,"\v" )) && ( strcmp(line_token,"\0" )) && ( strlen(line_token) > 1 ) && ( strcmp(trim(line_token),"\n") ))
				{				
					if ( ! strcmp(trim(line_token),"quit") )		// Elegxos gia quit kai ean auto vrethei
					{										// vriskoume ola ta tote mexri kai ekeinh thn
						quit_flag = 0;							// grammh kai meta stamatame
					}
					else
					{									
						// Prepei strcpy kai oxi isothta pointer giati to line_token katastrefetai kai 							// ksanaorizetai se
						// kathe iteration. Thelw apla na perasw to periexomeno tou line_token 
						// ston pointer tou tokens
						strcpy(tokens[tokens_counter],line_token);
						//printf("line_token: \n%s\n",line_token);
						tokens_counter++;
					}
				}
				line_token = strtok(NULL,";");
			}
			line_tokens_number[line_number] = tokens_counter;
			//printf("line_tokens_number: \n%d\n",line_tokens_number[line_number]);
			line_number++;
		}
		fclose(fp);

		if ( empty_file_flag )
		{
			printf("\n\nYou have to enter at least one command...\n\n");
			exit (1);
		}

		strcpy(tokens[tokens_counter],"sleep 0.1");
		tokens_counter++;

		if ( ! quit_flag )
		{
			strcpy(tokens[tokens_counter],"quit");
			tokens_counter++;
		}


		// Diagrafh kenwn xarakthrwn sthn arxh kai to telos twn tokens

		
		for (int i=0;i<tokens_counter;i++)
		{
			strcpy(tokens[i],trim(tokens[i]));
		}
		//printf("Tokens: %d\n\n",tokens_counter);

		printf("\n\nCommands: %d\n\n",tokens_counter-1);
		for (int i=0;i<tokens_counter;i++)
		{
			if ( strcmp(tokens[i],"sleep 0.1") )
			{		
				printf("%s\n",tokens[i]);
			}
		}


		// Twra tha kaloume gia kathe token thn args_tokens gia na pairnoume ta tokens 
		// me th swsth morfopoihsh ws pros ta orismata pou tuxon exoun kai meta 
		// tha ta spame se epi merous tokens gia na vroume ta orismata, me delim to keno
		// afou pleon ta orismata tha exoun ena akrivws keno metaksy tous

	
		// Spame se tokens tis entoles gia na vroume ta orismata tous, afou prwta valoume anamesa se onoma entolhs
		// kai args ena mono keno, kalwntas thn args_tokens

		for (int i=0;i<tokens_counter;i++)
		{
			strcpy(tokens[i],args_tokens(tokens[i]));
		}

		

		printf("\nTerminal commands (random) execution:\n\n\n\n");

		if ( quit_flag == 0 )
		{
			tokens_counter--;
		}
		

		int i;
		int status=0;		// For serial execution, comment status declaration from main
		for (i=0;i<tokens_counter;i++)
		{
			char **fork_tokens, **copy_tokens;
			int fork_tokens_counter=0;
			fork_tokens = (char **)malloc(tokens_counter*sizeof(char *));
			copy_tokens = (char **)malloc(tokens_counter*sizeof(char *));
			for (int k=0;k<tokens_counter;k++)
			{
				fork_tokens[k] = (char *)malloc(513*sizeof(char));
				copy_tokens[k] = (char *)malloc(513*sizeof(char));
				strcpy(copy_tokens[k],tokens[k]);
			}
			char **next = fork_tokens;
			char *temp_fork_token;
			temp_fork_token = (char *)malloc(513*sizeof(char));
			temp_fork_token = strtok(copy_tokens[i]," ");
			while ( temp_fork_token != NULL )
			{
				*next++;	
		
				strcpy(fork_tokens[fork_tokens_counter],temp_fork_token);
				//printf("%s",fork_tokens[fork_tokens_counter]);
				fork_tokens_counter++;		
				temp_fork_token = strtok(NULL," ");
			}
			*next = NULL;
			//printf("next=: %s\n",fork_tokens[fork_tokens_counter]);
			// printf("Execution of command *** %s ***:\n\n",tokens[i]);	


			// Execution of commands
			execute_tokens(fork_tokens);
		}
		while (wait(&status) > 0 );		// comment this line for serial
		if ( ! strcmp(tokens[i],"quit") )
		{
			exit (0);
		}
		for (int i=0;i<tokens_counter;i++)
		{
			free(tokens[i]);	
		}
		free(line_tokens_number);
		free(line_token);
		free(tokens);
	}
	else if ( argc == 1 )
	{
		printf("\n\n\n\n\n\n				***** INTERACTIVE MODE *****\n\n\n\n\n\n");
		printf("Eisagete tis entoles pou thelete na ektelestoun, diaxwrismenes me ton xarakthra \" ; \" kai pathste enter\n\nMaximum plithos xarakthrwn = 512\n\n\n\n\n\n");

		int quit_flag=1;
		int clear_flag=1;	
		while ( quit_flag )
		{
			printf("\n\n\n\n\n\n\n\n\n\n\ninglezos_8145> ");
			char *line;
			line = (char *)malloc(513*sizeof(char));
			char temp_line[513];
			fgets(line,513,stdin);
		
			// Diavazoume to input twn entolwn kai tis pername se arxeio kai tis diavazoume meta apo ekei 				
			// kai pali, dioti etsi exoume kaluterh eikona ths eisodou kai dioti parousiazotan
			// ena provlima sthn entolh ./[onoma idiou programmatos me orisma]
 
			FILE *temp_fp = fopen("copy_line", "w+");
		     if (temp_fp != NULL)
		     {	
				fputs(line, temp_fp);
				fclose(temp_fp);
		     }
			//printf("\n\n%s\n",line);
			strcpy(temp_line,line);
	
			int tokens_counter=0;
			char *temp_line_token;
			temp_line_token = (char *)malloc(513*sizeof(char));
			temp_line_token = strtok(line,";");
			while ( temp_line_token != NULL )
			{
				tokens_counter++;
				temp_line_token = strtok(NULL,";");
			}

			char **tokens;
			tokens=(char **)malloc((tokens_counter+2)*sizeof(char *));
			for (int i=0;i<tokens_counter+2;i++)
			{
				tokens[i] = (char *)malloc(513*sizeof(char));
			}

			int cur_tokens_counter=0;
			int tokens_number=0;
			int tokens_number_with_quit=0;
			FILE *fp = fopen("copy_line", "r+");
			char *file_line;
			file_line = (char *)malloc(513*sizeof(char));
			fgets(file_line,512,fp);
			fclose(fp);
			char *line_token;
			line_token = (char *)malloc(513*sizeof(char));
			line_token = strtok(file_line,";");
			while ( line_token != NULL )
			{				
				//printf("line_token: \n%s\n",line_token);			
				if (( strcmp(line_token,"" )) && ( strcmp(line_token,"\n" )) && ( strcmp(line_token,"\v" )) && ( strcmp(line_token,"\0" )) && ( strlen(line_token) > 1 ) && ( strcmp(trim(line_token),"\n") ))
				{				
					if ( ! strcmp(trim(line_token),"quit") )		// Elegxos gia quit kai ean auto vrethei
					{										// vriskoume ola ta tote mexri kai ekeinh thn
						quit_flag = 0;							// grammh kai meta stamatame
						tokens_number_with_quit++;					
					}
					else
					{									
						// Prepei strcpy kai oxi isothta pointer giati to line_token katastrefetai kai 							// ksanaorizetai se
						// kathe iteration. Thelw apla na perasw to periexomeno tou line_token 
						// ston pointer tou tokens
						strcpy(tokens[cur_tokens_counter],line_token);
						cur_tokens_counter++;
						tokens_number++;
						tokens_number_with_quit++;
						//printf("line_token: \n%s\n",line_token);
					}
				}
				line_token = strtok(NULL,";");
			}
			printf("\n\n\nNumber of commands: %d\n\n",tokens_number_with_quit);
			/*for (int i=0;i<tokens_number;i++)
			{
				printf("%s\n",tokens[i]);
			}*/

			strcpy(tokens[cur_tokens_counter],"sleep 0.1");
			cur_tokens_counter++;
			tokens_number++;
			tokens_number_with_quit++;

			if ( ! quit_flag )
			{
				strcpy(tokens[cur_tokens_counter],"quit");
				cur_tokens_counter++;
			}
			


			// Diagrafh kenwn xarakthrwn sthn arxh kai to telos twn tokens

		
			for (int i=0;i<tokens_number;i++)
			{
				strcpy(tokens[i],trim(tokens[i]));
			}

			printf("\n\n\nCommands: \n\n");
			for (int i=0;i<tokens_number_with_quit;i++)
			{
				if ( strcmp(tokens[i],"sleep 0.1") )
				{
					printf("%s\n",tokens[i]);
				}
			}


			// Twra tha kaloume gia kathe token thn args_tokens gia na pairnoume ta tokens 
			// me th swsth morfopoihsh ws pros ta orismata pou tuxon exoun kai meta 
			// tha ta spame se epi merous tokens gia na vroume ta orismata, me delim to keno
			// afou pleon ta orismata tha exoun ena akrivws keno metaksy tous

	
			// Spame se tokens tis entoles gia na vroume ta orismata tous, afou prwta valoume anamesa se 
			// onoma entolhs kai args ena mono keno, kalwntas thn args_tokens

			for (int i=0;i<tokens_number;i++)
			{
				strcpy(tokens[i],args_tokens(tokens[i]));
			}

			/*for (int i=0;i<tokens_number_with_quit;i++)
			{
				printf("%s",tokens[i]);
			}*/

			printf("\n\n\nTerminal commands (random) execution:\n\n\n\n");

			if ( quit_flag == 0 )
			{
				tokens_counter--;
			}
			int i;
			int status=0;		// For serial execution, comment status declaration from main
			for (i=0;i<tokens_number;i++)
			{
				char **fork_tokens, **copy_tokens;
				int fork_tokens_counter=0;
				fork_tokens = (char **)malloc(tokens_number_with_quit*sizeof(char *));
				copy_tokens = (char **)malloc(tokens_number_with_quit*sizeof(char *));
				for (int k=0;k<tokens_number_with_quit;k++)
				{
					fork_tokens[k] = (char *)malloc(513*sizeof(char));
					copy_tokens[k] = (char *)malloc(513*sizeof(char));
					strcpy(copy_tokens[k],tokens[k]);
				}
				char **next = fork_tokens;
				char *temp_fork_token;
				temp_fork_token = (char *)malloc(513*sizeof(char));
				temp_fork_token = strtok(copy_tokens[i]," ");
				while ( temp_fork_token != NULL )
				{
					*next++;
					strcpy(fork_tokens[fork_tokens_counter],temp_fork_token);
					//printf("%s",fork_tokens[fork_tokens_counter]);
					fork_tokens_counter++;		
					temp_fork_token = strtok(NULL," ");
				}
				*next = NULL;
				//printf("next=: %s\n",fork_tokens[fork_tokens_counter]);
				// printf("Execution of command *** %s ***:\n\n",tokens[i]);	


				// Execution of commands
				execute_tokens(fork_tokens);


				for (int i=0;i<tokens_number_with_quit;i++)
				{
					free(fork_tokens[i]);
					free(copy_tokens[i]);
				}
				free(fork_tokens);
				free(copy_tokens);
			}
			while (wait(&status) > 0 );		// comment this line for serial
			for (int i=0;i<tokens_number;i++)
			{
				free(tokens[i]);
			}
			free(tokens);
			free(temp_line_token);
			free(file_line);
			free(line_token);
			clear_flag = 0;
		}
	}
	printf("\n\n\n\n\n\n\n\n\n\nProgram is now being terminated...\n\n\n\n\n\n\n\n\n\n\n");
return (0);
}





char *trim(char *str)
{
  char *end;
  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces? This means we have reached \0 of the string
    return "\n";

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}





char *args_tokens(char *str)
{
	char *temp_token;
	char **original_tokens;
	temp_token = (char *)malloc(513*sizeof(char));
	original_tokens = (char **)malloc(170*sizeof(char *));
	for (int i=0;i<170;i++)
	{
		original_tokens[i] = (char *)malloc(513*sizeof(char));
	}

	int temp_tokens_counter=0;
	temp_token = strtok(str," \t");
	while ( temp_token != NULL )
	{
		//printf("%s\n",temp_token);		
		strcpy(original_tokens[temp_tokens_counter],trim(temp_token));
		//printf("%s\n",original_tokens[temp_tokens_counter]);
		temp_tokens_counter++;
		temp_token = strtok(NULL,";");
	}

	int tokens_counter=0;
	char **tokens;
	tokens = (char **)malloc(170*sizeof(char *));
	for (int i=0;i<170;i++)
	{
		tokens[i] = (char *)malloc(513*sizeof(char));
	}

	for (int i=0;i<temp_tokens_counter;i++)
	{
		if ( (strcmp(original_tokens[i]," ")) && (strcmp(original_tokens[i],"\t")) )
		{			
			strcpy(tokens[tokens_counter],original_tokens[i]);
			//printf("%s\n",tokens[tokens_counter]);
			tokens_counter++;
		}
	}
	char *final_str;
	final_str = (char *)malloc(513*sizeof(char));	
	strcpy(final_str,tokens[0]);
	for (int i=1;i<tokens_counter;i++)
	{		
		final_str = strcat(final_str," ");
		final_str = strcat(final_str,tokens[i]);
	}
	
	for (int i=0;i<170;i++)
	{
		free(tokens[i]);
		free(original_tokens[i]);
	}
	free(tokens);
	free(original_tokens);
	free(temp_token);

	return final_str;
}





void execute_tokens(char **tokens)
{
	int pid, status;

	if ((pid = fork()) < 0)
	{
		perror("fork");
		exit(1);
	}
	if (pid == 0) 
	{
		//printf("in fork: %s\n",tokens[0]);
		//printf("in fork: %s\n",tokens[1]);
		//printf("in fork: %s\n",tokens[2]);
		//printf("in fork: %s\n",tokens[3]);
		//printf("in fork: %s\n",tokens[4]);
		execvp(*tokens, tokens);
		perror(*tokens);
		exit(1);
	}	
	//while (wait(&status) != pid );		// For serial execution
}

