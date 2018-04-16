#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
struct MOTtable
{
	char Mnemonic[6];
	int  Class;
	char Opcode[3];
};

struct symboltable
{
	char Symbol[8];
	int  Address;
	int  Size;
}ST[20];

struct intermediatecode
{
	int LC;
	int Code1,Type1;
	int Code2,Type2;
	int Code3,Type3;
}IC[30];

struct Littable
{
	char literal[7];
	int address;
}LIT[10];

struct pooltable
{
	int literal_no;
}PT[10];

static struct MOTtable MOT[28]=
{
	{"STOP",1,"00"},{"ADD",1,"01"},{"SUB",1,"02"},
	{"MULT",1,"03"},{"MOVER",1,"04"},{"MOVEM",1,"05"},
	{"COMP",1,"06"},{"BC",1,"07"},{"DIV",1,"08"},
	{"READ",1,"09"},{"PRINT",1,"10"},
	{"START",3,"01"},{"END",3,"02"},{"ORIGIN",3,"03"},
	{"EQU",3,"04"},{"LTORG",3,"05"},
	{"DS",2,"01"},{"DC",2,"02"},
	{"AREG",4,"01"},{"BREG",4,"02"},{"CREG",4,"03"},
	{"EQ",5,"01"},{"LT",5,"02"},{"GT",5,"03"},{"LE",5,"04"},
	{"GE",5,"05"},{"NE",5,"06"},{"ANY",5,"07"}
};

int nMOT=28; //Number of entries in MOT
int LC=0;    //Location counter
int iST=0;   //Index of next entry in Symbol Table
int iIC=0;   //Index of next entry in intermediate code table
int littab_ptr=0;
int pooltab_ptr=0;

int searchST(char symbol[])
{
	int i;
	for(i=0;i<iST;i++)
		if(strcmp(ST[i].Symbol,symbol)==0)
			return(i);
	return(-1);
}

int searchMOT(char symbol[])
{
	int i;
	for(i=0;i<nMOT;i++)
		if(strcmp(MOT[i].Mnemonic,symbol)==0)
			return(i);
	return(-1);
}

int insertST( char symbol[],int address,int size)
{
       strcpy(ST[iST].Symbol,symbol);
       ST[iST].Address=address;
       ST[iST].Size=size;
       iST++;
       return(iST-1);
}

int isliteral(char symbol[])
{
	if(strchr(symbol,'=')==NULL)
		return 0;
	return 1;
}
void imperative();  //Handle an executable statement
void declaration(); //Handle a declaration statement
void directive();  //Handle an assembler directive
void print_symbol(); //Display symbol table
void print_opcode();  //Display opcode table
void intermediate();  //Display intermediate code
void mcode();           //Generate machine code
char s1[8],s2[8],s3[8],label[8];
void DC();   //Handle declaration statement DC
void DS();   //Handle declaration statement DS
void START();  //Handle START directive
void LTORG();
void ORIGIN();
void EQU();
void print_lit_pool();
int tokencount;  //total number of words in a statement

void main()
{
	char file1[40],nextline[80];
	int len,i,j,temp,errortype;
	FILE *ptr1;
	printf("\nenter Source file name:");
	scanf("%s",file1);
	ptr1=fopen(file1,"r");
	PT[0].literal_no=0;
	while(!feof(ptr1))
	{
	//Read a line of assembly program and remove special characters
		i=0;
		nextline[i]=fgetc(ptr1);
		while(nextline[i]!='\n'&& nextline[i]!=EOF )
		{
			if(!isalnum(nextline[i]) && nextline[i]!='=')
				nextline[i]=' ';
			else
				nextline[i]=toupper(nextline[i]);
			i++;
			nextline[i]=fgetc(ptr1);
		}
		nextline[i]='\0';
		//printf("nextline[]=%s\n",nextline);

		sscanf(nextline,"%s",s1); //read from the nextline in s1
		if(strcmp(s1,"END")==0) //if the nextline is an END statement
		{
			LTORG();
			break;
		}
		//if the nextline contains a label
		if(searchMOT(s1)==-1)
		{
			if(searchST(s1)==-1)
				insertST(s1,LC,0);
		//separate opcode and operands
			tokencount=sscanf(nextline,"%s%s%s%s",label,s1,s2,s3);
			tokencount--;
		}
		else
		//separate opcode and operands
			tokencount=sscanf(nextline,"%s%s%s",s1,s2,s3);
		if(tokencount==0)//blank line
			continue;//goto the beginning of the loop
		i=searchMOT(s1);
		if(i==-1)
		{
			printf("\nWrong Opcode .... %s",s1);
			continue;
		}
		switch (MOT[i].Class)
		{
			case 1: imperative();break;
			case 2: declaration();break;
			case 3: directive();break;
			default: printf("\nWrong opcode ...%s",s1);
			break;
		}
	}
	print_opcode();
	print_symbol();
	print_lit_pool();
	intermediate();
	printf("\n");
	//mcode();  
}

void imperative()
{
	int index;
	index=searchMOT(s1);
	IC[iIC].Type1=IC[iIC].Type2=IC[iIC].Type3=0; //intialize
	IC[iIC].LC=LC;
	IC[iIC].Code1=index;
	IC[iIC].Type1=MOT[index].Class;
	LC=LC+1;
	if(tokencount>1)
	{
		index=searchMOT(s2);
		if(index != -1)
		{
			IC[iIC].Code2=index;
			IC[iIC].Type2=MOT[index].Class;
		}
		else
		{   	//It is a variable
			index=searchST(s2);
			if(index==-1)
				index=insertST(s2,0,0);
			IC[iIC].Code2=index;
			IC[iIC].Type2=7; //VALUE 7 IS FOR VARIABLES
		}
	}
	
	if(tokencount>2)
	{
		index=searchST(s3);	// -1 for symbol not defined and literal
		if(isliteral(s3)==0)	//so first check literal	
		{
			if(index==-1)		//symbol undefined
				index=insertST(s3,0,0);
			IC[iIC].Code3=index;
			IC[iIC].Type3=7	; //VALUE 7 IS FOR VARIABLES
		}
		else
		{
			IC[iIC].Code3=littab_ptr;
			IC[iIC].Type3=8;
			strcpy(LIT[littab_ptr].literal,s3);
			littab_ptr++;
			//if literal
		}
	}
	iIC++;
}

void declaration()
{
	if(strcmp(s1,"DC")==0)
	{
		DC();
		return;
	}
	if(strcmp(s1,"DS")==0)
		DS();
}

void directive()
{
	if(strcmp(s1,"START")==0)
	{
		START();
		return;
	}
	else if(strcmp(s1,"LTORG")==0)
	{
		LTORG();
		return ;
	}
	else if(strcmp(s1,"EQU")==0)
	{
		EQU();
		return ;
	}
	else
	{
		ORIGIN();
	}
 }


void DC()
{
	int index;
	index=searchMOT(s1);
	IC[iIC].Type1=IC[iIC].Type2=IC[iIC].Type3=0; //intialize
	IC[iIC].LC=LC;
	IC[iIC].Code1=index;
	IC[iIC].Type1=MOT[index].Class;
	IC[iIC].Type2=6;        //6 IS TYPE FOR CONSTANTS
	IC[iIC].Code2=atoi(s2);
	index=searchST(label);
	if(index==-1)
		index=insertST(label,0,0);
	ST[index].Address=LC;
	ST[index].Size=1;
	LC=LC+1;
	iIC++;
}

void DS()
{
	int index;
	index=searchMOT(s1);
	IC[iIC].Type1=IC[iIC].Type2=IC[iIC].Type3=0; //intialize
	IC[iIC].LC=LC;
	IC[iIC].Code1=index;
	IC[iIC].Type1=MOT[index].Class;
	IC[iIC].Type2=6;        //6 IS TYPE FOR CONSTANTS
	IC[iIC].Code2=atoi(s2);
	index=searchST(label);
	if(index==-1)
		index=insertST(label,0,0);
	ST[index].Address=LC;
	ST[index].Size=atoi(s2);
	LC=LC+atoi(s2);
	iIC++;
}


void START()
{
	int index;
	index=searchMOT(s1);
	IC[iIC].Type1=IC[iIC].Type2=IC[iIC].Type3=0; //intialize
	IC[iIC].LC=LC;
	IC[iIC].Code1=index;
	IC[iIC].Type1=MOT[index].Class;
	IC[iIC].Type2=6;        //6 IS TYPE FOR CONSTANTS
	IC[iIC].Code2=atoi(s2);
	LC=atoi(s2);
	iIC++;
}

void intermediate()
{
	int i;
	char decode[9][3]={" ","IS","DL","AD","RG","CC","C","S","L"};
	printf("\nIntermediate Code :");
	for(i=0;i<iIC;i++)
	{
		printf("\n%3d)   (%s,%2s)",IC[i].LC,decode[IC[i].Type1],MOT[IC[i].Code1].Opcode);
		if(IC[i].Type2!=0)
		{
			if(IC[i].Type2<6)
				printf(" (%s,%2s)",decode[IC[i].Type2],MOT[IC[i].Code2].Opcode);
			else
				printf("  (%s,%2d)",decode[IC[i].Type2],IC[i].Code2);
		}
		if(IC[i].Type3!=0)
			printf("  (%s,%2d)",decode[IC[i].Type3],IC[i].Code3);
	}
}
void print_symbol()
{
	int i;
	printf("\n*******symbol table ***********\n");
	for(i=0;i<iST;i++)
		printf("%10s  %3d   %3d\n",ST[i].Symbol,ST[i].Address,ST[i].Size);
}

void print_opcode()
{
	int i;
	printf("\n***********opcode table *************\n");
	for(i=0;i<nMOT;i++)
		if(MOT[i].Class==1)
			printf("%6s   %2s\n",MOT[i].Mnemonic,MOT[i].Opcode);
}

void print_lit_pool()
{
	int i;
	printf("\n********** LITTAB **************\n");
	for(i=0;i<littab_ptr;i++)
		printf("%s   %d\n",LIT[i].literal,LIT[i].address);
	printf("\n********** POOLTAB **************\n");
	for(i=0;i<pooltab_ptr;i++)
		printf("%d\n",PT[i].literal_no);
}
  
void EQU()
{
	int index=searchST(label),index1;
	if(index==-1)
		index=insertST(label,0,0);
	if(isdigit(s2[0])==0)
	{
		index1=searchST(s2);
		ST[index].Address=ST[index1].Address;
	}
	else
		ST[index].Address=atoi(s2);
	ST[index].Size=1;
}
  
void ORIGIN()
{
	LC=atoi(s2);
}
  
void LTORG()
{
	int i;
	for(i=PT[pooltab_ptr].literal_no;i<littab_ptr;i++)
	{
		LIT[i].address=LC++;
	}
	pooltab_ptr++;
	PT[pooltab_ptr].literal_no=littab_ptr; 
}
