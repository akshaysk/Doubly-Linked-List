/*****************************************************************
Definition of the object of the list's element.
*****************************************************************/
#ifndef _MY402LISTRECORD_H_
#define _MY402LISTRECORD_H_

#include "my402list.h" 

#ifndef MAXARRAYSIZE 
#define MAXARRAYSIZE 1024
#endif 

#ifndef REGARRAYSIZE
#define REGARRAYSIZE 24
#endif

#ifndef MAXTEMP
#define MAXTEMP 80
#endif
 
#define SIGN 3
#define TIMESTAMP 2
#define AMOUNT 1
#define DESCRIPTION 0

#define FORMATTED_AMOUNT_SIZE	14
#define FORMATTED_TIME_SIZE	15
#define FORMATTED_BALANCE_SIZE	14
#define TIMESTAMP_SIZE 		10
#define DESCRIPTION_SIZE	24
#define AMOUNT_SIZE		14
typedef struct tagMy402ListRecord
{
	char sign;
	long int timestamp;
	double amount;
	char description[REGARRAYSIZE];
}My402ListRecord;



int CHECK_SIGN(char );
int CHECK_TIMESTAMP(char *);		
int CHECK_DESCRIPTION(char *);		
int CHECK_AMOUNT(char *);	
char * with_commas(char *str);
void printline();
void printheader();
FILE *ParseInput(int, char **);
My402ListRecord *My402CheckFormat(char *);
My402ListElem *My402FindObjectPlace(My402List *,My402ListRecord *);
void CreateSortedList(FILE *, My402List *);
void PrintRecord(My402List *);
	
#endif
