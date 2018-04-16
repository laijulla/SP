#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<ctype.h>

typedef struct line
{
	char nextline[100];
	int address;
}line;

typedef struct terminal
{
	char term[20];
}terminal;

typedef struct identifier
{
	char iden[20];
}identifier;

typedef struct literal
{
	char lit[20];
}literal;

typedef struct UST
{
	char token[30];
	char class[30];
}UST;


typedef struct MOTtable 
{
 char Mnemonic[30];
}MOTtable;

MOTtable MOT[28]={{"int"},{"void"},{"main"},{"{"},{"}"},{";"},{"="},{"+"},{"++"},{","},{"("},{")"},{"~"}};


line next[50];
terminal TT[50];
identifier ID[20];
literal LT[20];
int TTpt,nMOT=12,IDpt,LTpt;

int searchMOT(char symbol[])
{
	int i;
	for(i=0;i<nMOT;i++)
		if(strcmp(MOT[i].Mnemonic,symbol)==0)
			return(i);
	return(-1);
}

void addtoterminal(int pos)
{
	int flag=0,i;
	char temp[20];
	strcpy(temp,MOT[pos].Mnemonic);
	for(i=0;i<TTpt;i++)
	{
		if(strcmp(TT[i].term,temp)==0)
			flag=1;
	}
	if(!flag)
	strcpy(TT[TTpt++].term,temp);
}

void addtoliteral(char symbol[])
{
	int flag=0,i;
	char temp[20];
	for(i=0;i<LTpt;i++)
	{
		if(strcmp(LT[i].lit,symbol)==0)
			flag=1;
	}
	if(!flag)
	strcpy(LT[LTpt++].lit,symbol);
}

void addtoidentifier(char symbol[])
{
	int flag=0,i;
	char temp[20];
	for(i=0;i<IDpt;i++)
	{
		if(strcmp(ID[i].iden,symbol)==0)
			flag=1;
	}
	if(!flag)
	strcpy(ID[IDpt++].iden,symbol);
}

void printidentifier()
{	int i;
	for(i=0;i<IDpt;i++)
	{
		printf("\n %s",ID[i].iden);
	}
}

void printterminal()
{	int i;
	for(i=0;i<TTpt;i++)
	{
		printf("\n %s",TT[i].term);
	}
}

void printliteral()
{	int i;
	for(i=0;i<LTpt;i++)
	{
		printf("\n %s",LT[i].lit);
	}
}

int main()
{
	UST ust[100];int u=0;
	char file1[40],nextline[80],s1[20];
	char *data;
	int x,len,i,j,temp,errortype,offset;
	FILE *ptr1;
	ptr1=fopen("sample.c","r");
	while(!feof(ptr1))
	{
	//Read a line of assembly program and remove special characters
		i=0;
		nextline[i]=fgetc(ptr1);
		while(nextline[i]!='\n'&& nextline[i]!=EOF )
		{
			i++;
			nextline[i]=fgetc(ptr1);
		}
		nextline[i]='\0';
		data=nextline;
		while(sscanf(data,"%s%n",s1,&offset)==1)
		{
			data +=offset;
			int pos=searchMOT(s1);
			
			if(pos!=-1)
			{
				addtoterminal(pos);
				strcpy(ust[u].token,s1);
				strcpy(ust[u].class,"terminal");
				u++;
				
			}
			else
			{
				if(isalpha(s1[0]))
				{
					addtoidentifier(s1);
					strcpy(ust[u].token,s1);
					strcpy(ust[u].class,"identifier");
					u++;
				}
				else
				{
				 	addtoliteral(s1);
				 	strcpy(ust[u].token,s1);
				 	strcpy(ust[u].class,"literal");
					u++;
				}
			}
		}
	}
	printf("Terminal table");
	printterminal();
	printf("\nIdentifier Table\n");
	printidentifier();
	printf("\nLiteral Table\n");
	printliteral();
	printf("\n\n");
	
	printf("\n\n\nUST\n\n\n");
	for(x=0;x<u;x++)
	{
		printf("%s\t%s\n",ust[x].token,ust[x].class);
	}
}
