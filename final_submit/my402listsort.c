/**************************************************************
Sort logic implementation for doubly list circular linkedlist
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <sys/stat.h>
#include <unistd.h>

#include "my402listrecord.h"


int CHECK_SIGN(char sign)					/*  Check correctness in the sign field of the record  */		
{						
	if(sign !='-' && sign!='+'){	
		fprintf(stderr,"\nError:Transaction record sign value is not well formatted\n");				
		return FALSE;	
	}		
	return TRUE;				
}


int CHECK_TIMESTAMP(char *temp)					/*  Check correctness of the timestamp field of the record  */
{	
	int len=strlen(temp);		
		
	if(len != TIMESTAMP_SIZE){
		fprintf(stderr,"\nError:Transaction record Timestamp size is not well formatted\n");				
		return FALSE;
	}				
	if(atoi(temp) > 0 && atoi(temp) < time(NULL))	
		return TRUE;				
	else{						
		fprintf(stderr,"\nError:Transaction record timestamp value is not well formatted\n");				
		return FALSE;				
	}
}

int CHECK_DESCRIPTION(char *temp)				/*  Check correctness in the description field of the record  */	
{	
	char *t1,*t2;
	int i=0,len=strlen(temp);
	
	char *temp2=(char *)malloc(MAXTEMP*sizeof(char));
	
	t1 = temp;
	t2 = temp2;


	while(*t1 == ' ')
		t1++;

	while(*t1 != '\0' && i<REGARRAYSIZE)
	{
		*t2++=*t1++;
		i++;
	}
	*t2='\0';
	len = strlen(temp2);
	strncpy(temp,temp2,len);	
	temp[len]='\0';

	return TRUE;	
}

int CHECK_AMOUNT(char *temp)					/*  Check correctness in the amount field of the record  */	
{
	int len=strlen(temp);
	if(len == 0){
		fprintf(stderr, "\nError:Transaction record amount is empty\n");
		return FALSE;
	}

	if(len>AMOUNT_SIZE){
		fprintf(stderr,"\nError:Transaction record amount value exceeds maximum length\n");				
		return FALSE;
	}
	if(temp[len-3]!='.'){
		fprintf(stderr,"\nError:Transaction record amount value is not well formatted\n");				
		return FALSE;
	}
	return TRUE;
}

char * with_commas(char *str)					/*  Format the amount/balance field with inserting commas  */
{
	int len=strlen(str), len2=0;
	if(len <= 8)
		return str;

	int i=len-5,j=0,count=3;
	char *temp = (char *)malloc(REGARRAYSIZE*sizeof(char));
	while(i>0)
	{
		if(str[i] == ' '){
			break;
		}
		if(count == 0){
			temp[j]=',';	
			count=4;
		}else	
			temp[j]=str[i--];
		
		j++;
		count--;
	}

	temp[j]='\0';

	len2=strlen(temp);
	j=len2-1;
	i=0;
	while(i<j)
	{
		temp[i]^=temp[j];
		temp[j]^=temp[i];
		temp[i]^=temp[j];	
		i++;j--;
	}

	i=len2;
	j=3;
	while(j>0)
	{	
		temp[i]=str[len-j-1];
		i++;j--;
	}
	temp[i]='\0';
	return temp;	
}

FILE * ParseInput(int argc, char **argv)			/*  Parse commandline input  */
{
	FILE *fp;
	int len=0;
	struct stat s;

	if(argc < 2){
		fprintf(stderr, "\nMalformed command!\nusage: warmup1 sort <test.tfile>\n");
		exit(-1);
	}
	if(strcmp(argv[1],"sort") != 0){
		fprintf(stderr, "\nMalformed command!\nusage: warmup1 sort <test.tfile>\n");
		exit(-1);
	}
	
	if(argc == 2){
		fp = stdin;

	} else if(argc == 3){

				
		if(stat(argv[2],&s)!=0){
			fprintf(stderr,"\nError: Source file %s doesn't exist!\n",argv[2]);
			exit(-1);
		}else if(s.st_mode & S_IFDIR){
			fprintf(stderr,"\nError: %s is a directory\n",argv[2]);
			exit(-1);	
		}
	
		if((len=strlen(argv[2])) > MAXPATHLENGTH){
			fprintf(stderr, "\nError: file name length exceeds max path length!Exiting..\n");	
			exit(-1);
		}


		fp = fopen(argv[2],"r");
		if(!fp){
			fprintf(stderr, "\nError: Could not open the source file %s. Access denied\n",argv[2]);
			exit(-1);
		}			
	
	}else{
		fprintf(stderr, "\nMalformed command!\nusage: warmup1 sort <test.tfile>\n");
		exit(-1);
	}
	
			
	return fp;
}


My402ListRecord * My402CheckFormat(char *line)				/*  Check errors for the scanned input record and create list object */
{
	My402ListRecord *record = NULL;
	int iteration = 4, errno = 0, no_of_fields = 1, len=0;
	char *start_ptr, *tab_ptr, *temp;
	start_ptr = tab_ptr = line;
	record = (My402ListRecord *)malloc(sizeof(My402ListRecord));
	temp = (char *)malloc(MAXTEMP*sizeof(char)); 

	
	do{
	iteration--;		
	start_ptr = tab_ptr;
	tab_ptr = strchr(start_ptr,'\t');
	memset(temp,0,MAXTEMP*sizeof(char));
	
	if(tab_ptr == NULL){
		if(no_of_fields != 4){
			errno = 1;
			break;
		}
	}else if(tab_ptr != NULL)
	{
		*tab_ptr++ = '\0';
		if(no_of_fields == 4){
			errno = 1;
			break;
		}		
	no_of_fields++;
	}
		switch(iteration)
		{
			case SIGN:
				sscanf(start_ptr,"%c",&record->sign);
				if(!CHECK_SIGN(record->sign)){
					free(record);free(temp);
					errno = 1;
					break;
				}	
				break;	
			case TIMESTAMP:
				sscanf(start_ptr,"%s",temp);
				if(!CHECK_TIMESTAMP(temp)){
					free(record);free(temp);
					errno = 1;
					break;
				}
				sscanf(temp,"%ld",&record->timestamp);
				break;
			case AMOUNT:
				sscanf(start_ptr,"%s",temp);
				if(!CHECK_AMOUNT(temp)){
					free(record);free(temp);
					errno = 1;
					break;

				}
				sscanf(temp,"%lf",&record->amount);
				
				break;
			case DESCRIPTION:
				if( ( len=strlen(start_ptr)) > MAXTEMP){
					fprintf(stderr, "\nError: Transaction description field is too big\n");
					free(record);free(temp);
					errno = 1; break;
				}
				if(len == 1){
					fprintf(stderr, "\nError: Transaction description field is empty\n");
					free(record);free(temp);
					errno = 1; break;
				}
				sscanf(start_ptr,"%[^\t\n]",temp);
				if(!CHECK_DESCRIPTION(temp)){
					free(record);free(temp);
					errno = 1;
					break;

				}
				sscanf(temp,"%24[^\t\n]s",record->description);
				break;
			default:
				errno = 1;
				break;
		}

	if(errno == 1)
		break;
	}while(iteration);
	
	if(errno == 1)
		return NULL;

	return record;	
	
}

My402ListElem * My402FindObjectPlace(My402List *list, My402ListRecord *record){		/*  Find the exact position into the linked list 
											where the list object is to be inserted  */

	My402ListElem *elem = My402ListFirst(list);
	if(elem == NULL)
		return &list->anchor;

	My402ListRecord *elem_rec = (My402ListRecord *)(elem->obj);
	while((record->timestamp > elem_rec->timestamp)){
		elem = My402ListNext(list, elem);
		if(elem == NULL)
		break;
		elem_rec = (My402ListRecord *)(elem->obj);
	}

		if(elem == NULL)
			return  &list->anchor;
		
		if(record->timestamp == elem_rec->timestamp){
			fprintf(stderr,"\nTimestamp values being clashed!! Exiting..\n");
			exit(-1);
		}	 
		else
			return elem;	
		 
}	

void printline()					/*  Formatting the output  */
{
	int i=0;
	printf("+");
	for(i=0;i<17;i++)printf("-");
	printf("+");
	for(i=0;i<26;i++)printf("-");
	printf("+");
	for(i=0;i<16;i++)printf("-");
	printf("+");
	for(i=0;i<16;i++)printf("-");
	printf("+\n");
	
		
}
void printheader()					/*  Formatting the output  */
{
	int i;
	printf("|");
	for(i=0;i<7;i++)printf(" ");
	printf("Date");
	for(i=0;i<6;i++)printf(" ");
	printf("|");
	printf(" ");
	printf("Description");
	for(i=0;i<14;i++)printf(" ");
	printf("|");
	for(i=0;i<9;i++)printf(" ");
	printf("Amount");
	printf(" ");
	printf("|");
	for(i=0;i<8;i++)printf(" ");
	printf("Balance");
	printf(" ");
	printf("|\n");
}
void PrintRecord(My402List *list){			/*  Print the output records  */
	My402ListRecord *record;
	double balance=0;
	My402ListElem *elem = NULL;
	char *formatted_time, *timestamp,*formatted_amount,*formatted_balance,*formatted_amount_with_comma,*formatted_balance_with_comma;
	char garbage_string[] = "?,???,???.??";
	
	if(My402ListEmpty(list)){
		fprintf(stderr, "\nError: List could not be created\n");
		exit(-1);
	}
	
	printline();
	printheader();
	printline();

	formatted_time = (char *)malloc(FORMATTED_TIME_SIZE*sizeof(char));
	timestamp = (char *)malloc(TIMESTAMP_SIZE*sizeof(char));
	formatted_amount = (char *)malloc(FORMATTED_AMOUNT_SIZE*sizeof(char));
	formatted_balance = (char *)malloc(FORMATTED_BALANCE_SIZE*sizeof(char));
	formatted_amount_with_comma = (char *)malloc((FORMATTED_AMOUNT_SIZE+4)*sizeof(char));
	formatted_balance_with_comma = (char *)malloc((FORMATTED_BALANCE_SIZE+4)*sizeof(char));
	for(elem = My402ListFirst(list);elem != NULL;elem = My402ListNext(list,elem)){
	record = (My402ListRecord *)elem->obj;

	timestamp = ctime(&record->timestamp);
	snprintf(formatted_time,11,"%s",timestamp);
	snprintf(formatted_time+10,6,"%s",timestamp+19); 

	if(record -> sign == '+'){
		balance = balance + record->amount;
		if(record->amount >= 10000000)
			snprintf(formatted_amount_with_comma,FORMATTED_AMOUNT_SIZE+10," %s ",garbage_string);
		else{
			snprintf(formatted_amount,FORMATTED_AMOUNT_SIZE+4," %'12.2f ",record->amount);
			formatted_amount = with_commas(formatted_amount);
			snprintf(formatted_amount_with_comma,FORMATTED_AMOUNT_SIZE+10," %12s ",formatted_amount);
		}
	}else{
		
		balance = balance - record->amount;
		if(record->amount >= 10000000)
			snprintf(formatted_amount_with_comma,FORMATTED_AMOUNT_SIZE+10,"(%s)",garbage_string);
		else{	
			snprintf(formatted_amount,FORMATTED_AMOUNT_SIZE+4," %'12.2f ",record->amount);
			formatted_amount = with_commas(formatted_amount);
			snprintf(formatted_amount_with_comma,FORMATTED_AMOUNT_SIZE+10,"(%12s)",formatted_amount);
		}
	}	
	
	if(balance > 0){
		if(balance >= 10000000)
			snprintf(formatted_balance_with_comma,FORMATTED_BALANCE_SIZE+10," %s ",garbage_string);
		else{
			snprintf(formatted_balance,FORMATTED_BALANCE_SIZE+4," %'12.2f ",balance);
			formatted_balance = with_commas(formatted_balance);
			snprintf(formatted_balance_with_comma,FORMATTED_BALANCE_SIZE+10," %12s ",formatted_balance);
		}
	}else{
		if(balance <= -10000000)	
			snprintf(formatted_balance_with_comma,FORMATTED_BALANCE_SIZE+10,"(%s)",garbage_string);
		else{
			snprintf(formatted_balance,FORMATTED_BALANCE_SIZE+4," %'12.2f ",-balance);	
			formatted_balance = with_commas(formatted_balance);
			snprintf(formatted_balance_with_comma,FORMATTED_BALANCE_SIZE+10,"(%12s)",formatted_balance);
		}
	}

	printf("| %13s | %-24s | %14s | %14s |\n",formatted_time,record->description,formatted_amount_with_comma,formatted_balance_with_comma);	

	}
	printline();
	
}

void CreateSortedList(FILE *fp, My402List *list){		/* Funtion to create the resultant sorted list of the objects */	
	My402ListElem *next_elem = NULL;
	char line[MAXARRAYSIZE];
	My402ListRecord *record = NULL;
	int len=0;
	memset(line,0,MAXARRAYSIZE);
	while(fgets(line,sizeof(line),fp) != NULL){			/*  Read record line by line from input file  */
		if((len=strlen(line))>MAXARRAYSIZE){
			fprintf(stderr, "\nError:Line is too big (i.e more than 1024 chars)!Exiting..\n");
			exit(-1);
		}	
		if((record = My402CheckFormat(line)) == NULL){		/*   Check the record for its correctness  */
			fprintf(stderr,"\nError: Error in Format of the record! Exiting..\n");
			exit(-1);
		}

		next_elem = My402FindObjectPlace(list, record);		/*  Find its exact location in the list where the object is to be inserted */
		
		My402ListInsertBefore(list, record, next_elem);		/*  Insert the object  */
		
		memset(line,0,MAXARRAYSIZE);
	}
	

}

int main(int argc, char **argv)
{
	FILE *fp = NULL;
	My402List list;
	(void)My402ListInit(&list);
	
	fp = ParseInput(argc,argv);
	CreateSortedList(fp, &list);

	PrintRecord(&list);
	if(fp != stdin)	
		fclose(fp);
	return 0;
}
