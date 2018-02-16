/*************************************************************
 * Name: Abhishek Kumar Srivastava
 * Student ID: 200203011
 * Unity ID: asrivas3
 * Description: CSC-541 Assignment 2
 * **********************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char* order;
char* stud_file;

FILE* stud;		//to open student records file
FILE* pindex;	//to open primary key index file
FILE* alist;	//to open available list file

typedef struct
{
	int key;
	long off;
} index_S;

typedef struct
{
	int siz;
	long off;
} avail_S;

index_S pkey[10000];	//array to store index file values
avail_S avail[10000];	//array to store availability list values
int idxlen=0,alen=0;	//counter to store lengths of pkey and avail array

void escape();
void find(int);
void delete(int);
void add(int,char*);
int ascend(const void*, const void*);
int descend(const void*, const void*);
int idxsort(const void*, const void*);

//Comparator to sort Availability List in Ascending Order
int ascend(const void* a, const void* b)
{
	avail_S* first = (avail_S*) a;
	avail_S* second = (avail_S*) b;
	
	if(first->siz < second->siz)
		return -1;
		
	if(first->siz > second->siz)
		return 1;
		
	if(first->siz == second->siz)
		return 0;
}

//Comparator to sort Availability List in Descending Order
int descend(const void* a, const void* b)
{
	avail_S* first = (avail_S*) a;
	avail_S* second = (avail_S*) b;
	
	if(first->siz > second->siz)
		return -1;
		
	if(first->siz < second->siz)
		return 1;
		
	if(first->siz == second->siz)
		return 0;
}

//Comparator to sort Primary Key Index according to key value
int idxsort(const void* a, const void* b)
{
	index_S* first = (index_S*) a;
	index_S* second = (index_S*) b;
	
	if(first->key < second->key)
		return -1;
		
	if(first->key > second->key)
		return 1;
		
	if(first->key == second->key)
		return 0;
}

//Function for "end" command
void escape()
{
	//printf("DEBUG: Entered escape()\n");
	int i;
	fseek(pindex, 0, SEEK_SET);
	fwrite(pkey, sizeof(index_S), idxlen, pindex);
	fclose(pindex);
	
	fseek(alist, 0, SEEK_SET);
	fwrite(avail, sizeof(index_S), alen, alist);
	fclose(alist);
	
	fclose(stud);
	
	printf("Index:\n");
	for(i=0;i<idxlen;i++)
		printf( "key=%d: offset=%ld\n", pkey[i].key, pkey[i].off );
	
	printf("Availability:\n");
	int hole_size=0;
	for(i=0;i<alen;i++)
	{
		printf( "size=%d: offset=%ld\n", avail[i].siz, avail[i].off );
		hole_size+=avail[i].siz;
	}
	printf( "Number of holes: %d\n", alen );
	printf( "Hole space: %d\n", hole_size );
	
	exit(0);
}

//function for "find" command
void find(int key)
{
	//printf("DEBUG: Entered find: %d\n",idxlen);
	int i=0,j=idxlen-1,mid=0,rsize=0;
	long offset=-1;
	char* record;
	
	//Binary Search
	while(i<=j)
	{
		//printf("i=%d j=%d\n",i,j);
		mid=(i+j)/2;
		
		if(key == pkey[mid].key)
		{
			offset=pkey[mid].off;
			break;
		}
		
		if(key < pkey[mid].key)
			j=mid-1;
		
		if(key > pkey[mid].key)
			i=mid+1;
	}
	
	//printf("DEBUG: Offset=%ld\n",offset);
	if(offset != -1)
	{
		fseek(stud,offset,SEEK_SET);
		fread(&rsize,sizeof(int),1,stud);
		record=(char*)malloc(rsize+1);
		fread(record,1,rsize,stud);
		record[rsize]='\0';
		printf("%s\n",record);
		free(record);
	}
	else
	{
		printf("No record with SID=%d exists\n",key);
	}
	//printf("DEBUG: Exited find\n");
}

//function for "del" command
void delete(int key)
{
	int i=0,j=idxlen-1,mid=0,rsize=0,idx=-1;
	long offset;
	char* record;
	
	//Binary Search for searching key from index
	while(i<=j)
	{
		mid=(i+j)/2;
		if(key == pkey[mid].key)
		{
			idx=mid;
			break;
		}
		
		if(key < pkey[mid].key)
			j=mid-1;
		
		if(key > pkey[mid].key)
			i=mid+1;
	}
	
	if(idx != -1)
	{
		offset=pkey[idx].off;
		fseek(stud,offset,SEEK_SET);
		fread(&rsize,1,sizeof(int),stud);
		
		//Adding hole in the Availability List
		avail[alen].siz = rsize + sizeof(int);
		avail[alen].off = offset;
		alen++;
		
		if(!strcmp(order,"--best-fit"))
			qsort(avail,alen,sizeof(avail_S),ascend);
		if(!strcmp(order,"--worst-fit"))
			qsort(avail,alen,sizeof(avail_S),descend);
		
		//Removing Key Index from Primary Key Index
		for(i=idx;i<idxlen;i++)
		{
			pkey[i].key = pkey[i+1].key;
			pkey[i].off = pkey[i+1].off;
		}
		idxlen--;
	}
	else
	{
		printf("No record with SID=%d exists\n",key);
	}
}

//function for "add" command
void add(int key, char* rec)
{
	//printf("DEBUG: Entered add\n");
	int i=0,len,remlen,j=idxlen-1,mid,idx=-1,loc=-1;
	long offset,newoffset;
	len = strlen(rec);
	
	//Binary Search for key in the index
	while(i<=j)
	{
		mid=(i+j)/2;
		if(key == pkey[mid].key)
		{
			idx=mid;
			break;
		}
		
		if(key < pkey[mid].key)
			j=mid-1;
		
		if(key > pkey[mid].key)
			i=mid+1;
	}
	
	if(idx != -1)
	{
		printf("Record with SID=%d exists\n",key);
		return;
	}
	
	//Searching for appropriate hole in availability list
	for(i=0;i<alen;i++)
	{
		if(avail[i].siz >= len + sizeof(int))
		{
				loc=i;
				break;
		}
	}
	
	if(loc == -1)
	{
		fseek(stud,0,SEEK_END);
		offset=ftell(stud);
	}
	else
	{
		fseek(stud,avail[loc].off,SEEK_SET);
		offset = avail[loc].off;
	}
	
	//writing size of record and size of record in the file
	fwrite(&len,1,sizeof(int),stud);
	fwrite(rec,len,1,stud);
	
	newoffset = offset+len + sizeof(int);
	remlen = avail[loc].siz - len - sizeof(int);
	
	//Updating Availability List
	if(remlen == 0)
	{
		for(j=i;j<alen;j++)
			avail[j]= avail[j+1];
		alen--;
	}
	if(remlen > 1)
	{
		avail[loc].off = newoffset;
		avail[loc].siz = remlen;
	}
	
	if(!strcmp(order,"--best-fit"))
			qsort(avail,alen,sizeof(avail_S),ascend);
	if(!strcmp(order,"--worst-fit"))
			qsort(avail,alen,sizeof(avail_S),descend);
	
	//printf("key:%d offset:%ld\n",key,offset);
	
	//Updating Primary Key Index
	pkey[idxlen].off = offset;
	pkey[idxlen].key = key;
	idxlen++;
	qsort(pkey,idxlen,sizeof(index_S),idxsort);
	//printf("DEBUG: Exited add\n");
}

int main(int argc, char* argv[])
{
	int i,count=0,key;
	char cmd[200];
	char* temp;
	if(argc != 3)
	{
		printf("Please execute the program as assn_2 <avail_list_order> <studentfile_name>\n");
		exit(-1);
	}
	
	order = argv[1];
	stud_file = argv[2];
	
	//printf("%d\n",(strcmp(order,"--first-fit")) && (strcmp(order,"--best-fit")) && (strcmp(order,"--worst-fit")));
	
	if((strcmp(order,"--first-fit")) && (strcmp(order,"--best-fit")) && (strcmp(order,"--worst-fit")))
	{
		printf("Please provide correct availability list order !!!\n");
		exit(-1);
	}
	
	if((stud = fopen(stud_file,"r+b")) == NULL)
	{
		stud = fopen( stud_file, "w+b" );
		pindex = fopen("index.idx","w+b");
		alist = fopen("available.idx","w+b");
	}
	else
	{
		pindex=fopen("index.idx","r+b");
		fseek(pindex,0,SEEK_SET);
		if(!pindex)
		{
			printf("index file can not be opened");
			exit(-1);
		}
		fseek(pindex,0,SEEK_END);
		idxlen=ftell(pindex);
		idxlen /= sizeof(index_S);
		fseek(pindex,0,SEEK_SET);
		for(i=0;i<idxlen;i++)
		{
			fseek(pindex,i*sizeof(index_S),SEEK_SET);
			fread(&pkey[i],sizeof(index_S),1,pindex);
		}
		
		alist=fopen("available.idx","r+b");
		if(!alist)
		{
			printf("availability file can not be opened");
			exit(-1);
		}
		fseek(alist,0,SEEK_END);
		alen=ftell(alist);
		alen /= sizeof(avail_S);
		fseek(alist,0,SEEK_SET);
		for(i=0;i<idxlen;i++)
		{
			fseek(alist,i*sizeof(avail_S),SEEK_SET);
			fread(&avail[i],sizeof(avail_S),1,alist);
		}
	}
	
	while(-1)
	{
		key=0;
		fgets(cmd,200,stdin);
		temp = strtok(cmd," \n");
		//printf("%s\n",temp);
		if(!strcmp(temp,"add"))
		{
			temp=strtok(NULL," \n");
			key=atoi(temp);
			temp=strtok(NULL," \n");
			add(key,temp);
		}
		else if(!strcmp(temp,"del"))
		{
			temp=strtok(NULL," \n");
			key=atoi(temp);
			delete(key);
		}
		else if(!strcmp(temp,"find"))
		{
			temp=strtok(NULL," \n");
			key=atoi(temp);
			find(key);
		}
		else if(!strcmp(temp,"end"))
		{
			escape();
		}
		else
		{
			printf("Please Enter correct command\n");
		}
		
	}
	return 0;
	
}
