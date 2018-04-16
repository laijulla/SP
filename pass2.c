#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

struct symboltable
{
	char Symbol[8];
	int Address;
	int Size;
}ST[20];

struct intermediatecode
{
	int LC;
	char opcode1[8], mcode1[8];
	char opcode2[8], mcode2[8];
	char opcode3[8], mcode3[8];
}IC[30];

struct littab
{
	char literal[7];
	int address;	
}LIT[20];

struct pooltable
{
	int literal_no;
}PT[10];

int iIC=0;
char s1[8],s2[8],s3[8],s4[8],s5[8],s6[8],s7[8];

void init()
{
	strcmp(s1,"");
	strcmp(s2,"");
	strcmp(s3,"");
	strcmp(s4,"");
	strcmp(s5,"");
	strcmp(s6,"");
	strcmp(s7,"");
}

void mcode()
{
	int i;
	printf("\nMachine Code");
	for(i=0;i<iIC;i++)
	{
		switch(IC[i].opcode1[0])
		{
			case 'D':
				if(strcmp(IC[i].mcode1,"01")==0)
				{
					printf("\n%d) + 00 0 00%s",IC[i].LC,IC[i].mcode2);
				}
				break;
			case 'A':
				break;
			case 'I':
				if(strcmp(IC[i].mcode1,"00")==0)
					printf("\n%d) + 00 0 000",IC[i].LC);
				else
				{
					printf("\n%d) + %s",IC[i].LC,IC[i].mcode1);
					if(strcmp(IC[i].opcode2,"S")!=0)
						printf(" %d",atoi(IC[i].mcode2));
					else
						printf("0 %d",ST[atoi(IC[i].mcode2)].Address);
						
					if(strcmp(IC[i].opcode3,"L")==0)
						printf(" %d",LIT[atoi(IC[i].mcode3)].address);
					else if(strcmp(IC[i].opcode3,"S")==0)
						printf(" %d", ST[atoi(IC[i].mcode3)].Address);
				}
				break;
		}
	}	
}
void main()
{
	int i,j,count;
	char nextline[80];
	FILE *fp;
	fp = fopen("literal.txt","r");
	j=0;
	printf("\nLiteral Table\n");
	while(fscanf(fp,"%s%s",s1,s2)!=-1)
	{
		strcpy(LIT[j].literal,s1);
		LIT[j].address=atoi(s2);
		printf("%s %d\n",LIT[j].literal,LIT[j].address);
		j++;
	}
	fclose(fp);
	
	fp = fopen("pooltable.txt","r");
	j=0;
	printf("\nPool Table\n");
	while(fscanf(fp,"%s",s1)!=-1)
	{
		PT[j].literal_no=atoi(s1);
		printf("%d\n",PT[j].literal_no);
		j++;
	}
	fclose(fp);
	
	fp = fopen("symbol.txt","r");
	j=0;
	printf("\nSymbol Table\n");
	while(fscanf(fp,"%s%s%s",s1,s2,s3)!=-1)
	{
		strcpy(ST[j].Symbol,s1);
		ST[j].Address=atoi(s2);
		ST[j].Size=atoi(s3);
		printf("%s %d %d\n",ST[j].Symbol,ST[j].Address,ST[j].Size);
		j++;
	}
	fclose(fp);
	
	fp=fopen("ic.txt","r");
	j=0;
	printf("\nIntermediate Code\n");
	while(!feof(fp))
	{
		i=0;
		nextline[i]=fgetc(fp);
		while(nextline[i]!='\n' && nextline[i]!=EOF)
		{
			if(!isalnum(nextline[i]))
			{
				nextline[i]=' ';
			}
			i++;
			nextline[i]=fgetc(fp);
		}
		nextline[i]='\0';
		init();
		count=sscanf(nextline,"%s%s%s%s%s%s%s",s1,s2,s3,s4,s5,s6,s7);
		if(count==-1)
			continue;
		IC[j].LC=atoi(s1);
		strcpy(IC[j].opcode1,s2);
		strcpy(IC[j].mcode1,s3);
		strcpy(IC[j].opcode2,s4);
		strcpy(IC[j].mcode2,s5);
		strcpy(IC[j].opcode3,s6);
		strcpy(IC[j].mcode3,s7);	
		printf("%d %s %s %s %s %s %s\n",IC[j].LC,IC[j].opcode1,IC[j].mcode1,IC[j].opcode2,IC[j].mcode2,IC[j].opcode3,IC[j].mcode3);
		j++;
	}
	iIC=j;
	fclose(fp);
	mcode();
	printf("\n");
}
