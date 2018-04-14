/*************************************************************
 * Name: Abhishek Kumar Srivastava
 * Student ID: 200203011
 * Unity ID: asrivas3
 * Description: CSC-541 Assignment 3
 * **********************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<limits.h>
#include<sys/time.h>

void basicMergeSort(char*, char*);
void multiStepMergeSort(char*, char*);
void replacementMergeSort(char*, char*);
int comparator(const void*,const void* b);
void buildMinHeap(int*,int,int);
void heapify(int*,int,int);
void swap(int*,int,int);

int main(int argc,char* argv[])
{
	char* method;
	if(argc != 4)
	{
		printf("Command format is assn_3 <mergesort-method> <index-file> <sorted-index-file>\n");
		exit(-1);
	}
	
	method = argv[1];
	
	struct timeval tm_start,tm_end,tm_diff;
	
	if(!strcmp(method,"--basic"))
		{
			gettimeofday(&tm_start,NULL);
			basicMergeSort(argv[2],argv[3]);
			gettimeofday(&tm_end,NULL);
			timersub(&tm_end,&tm_start,&tm_diff);
			printf( "Time: %ld.%06ld", tm_diff.tv_sec, tm_diff.tv_usec );
		}
	else if(!strcmp(method,"--multistep"))
		{
			gettimeofday(&tm_start,NULL);
			multiStepMergeSort(argv[2],argv[3]);
			gettimeofday(&tm_end,NULL);
			timersub(&tm_end,&tm_start,&tm_diff);
			printf( "Time: %ld.%06ld", tm_diff.tv_sec, tm_diff.tv_usec );
		}
	else if(!strcmp(method,"--replacement"))
		{
			gettimeofday(&tm_start,NULL);
			replacementMergeSort(argv[2],argv[3]);
			gettimeofday(&tm_end,NULL);
			timersub(&tm_end,&tm_start,&tm_diff);
			printf( "Time: %ld.%06ld", tm_diff.tv_sec, tm_diff.tv_usec );
		}
	else
		printf("Please Enter correct Merge Sort Method\n");
	
	return 0;
}

int comparator(const void* a,const void* b)
{
	return (*(int*)a - *(int*)b);
}

void basicMergeSort(char* idxFile,char* sortedIdxFile)
{
	int i=0,rec[1000],len=0,tottempfile,j,tempkey,runkey;
	int position=0,minelement=INT_MAX,count=0;
	int oprec[1000],filenum=0;
	FILE* ipfile,* opfile, * temp, **tempfileptr;
	char seq[5]="",tempfile[50]="";
	
	ipfile = fopen(idxFile,"rb");
	if(!ipfile)
	{
		printf("Unable to open file\n");
		return;
	}
	fseek(ipfile,0,SEEK_END);
	len=ftell(ipfile);
	len /= sizeof(int);
	fseek(ipfile,0,SEEK_SET);
	
	if(len <= 1000)
	{
		sprintf(tempfile,"%s",idxFile);
		sprintf(seq,"%.03d",i);
		strcat(tempfile,".");
		strcat(tempfile,seq);
		
		fread(&rec,sizeof(int),len,ipfile);
		qsort(rec,len,sizeof(int),comparator);
		temp=fopen(tempfile,"wb");
		fwrite(rec,sizeof(int),len,temp);
		opfile=fopen(sortedIdxFile,"wb");
		fwrite(rec,sizeof(int),len,opfile);
		fclose(temp);
		fclose(ipfile);
	}
	else
	{
		tottempfile = len/1000;
		
		if(tottempfile*1000 != len)
			tottempfile++;
		
		for(i=0;i<tottempfile;i++)
		{
			sprintf(tempfile,"%s",idxFile);
			sprintf(seq,"%.03d",i);
			strcat(tempfile,".");
			strcat(tempfile,seq);
			
			for(j=0;j<1000;j++)
			{
				fseek(ipfile,((i*1000)+j)*sizeof(int),SEEK_SET);
				if(fread(&tempkey,sizeof(int),1,ipfile) == 1 && j<1000)
				{
					rec[j]=tempkey;
					count++;
				}
				else
					break;
					
				fseek(ipfile,0,SEEK_END);
			}
			
			qsort(rec,count,sizeof(int),comparator);
			
			temp=fopen(tempfile,"wb");
			fwrite(rec,sizeof(int),count,temp);
			fclose(temp);
			count=0;
		}
		
		fclose(ipfile);
		
		for(i=0;i<1000;i++)
			rec[i]=INT_MAX;
		
		
		tempfileptr = malloc(sizeof(FILE*)*tottempfile);
		for(i=0;i<tottempfile;i++)
		{
			sprintf(tempfile,"%s",idxFile);
			sprintf(seq,"%.03d",i);
			strcat(tempfile,".");
			strcat(tempfile,seq);
			
			tempfileptr[i] = fopen(tempfile,"rb");
		}
		
		runkey=1000/tottempfile;
		for(i=0;i<tottempfile;i++)
		{	
			fread(&rec[i*runkey],sizeof(int),runkey,tempfileptr[i]);
		}
		
		opfile=fopen(sortedIdxFile,"wb");
		
		do
		{
			minelement = INT_MAX;
			for(i=0;i<1000;i++)
			{
				if(minelement > rec[i])
				{
					minelement = rec[i];
					position = i;
				}
			}
			
			if(count<1000 && minelement != INT_MAX)
			{
				oprec[count]=minelement;

				count++;
				filenum = (position)/runkey;
	
				int tempkeystore;
			
				if(fread(&tempkeystore,sizeof(int),1,tempfileptr[filenum]) == 1)
				{
					rec[position]=tempkeystore;
				}
				else
				{
					rec[position]=INT_MAX;
				}
				
			}
			else
			{
				fwrite(oprec,sizeof(int),count,opfile);
				memset(oprec,0,1000*sizeof(int));
				count=0;
			}

		}while(minelement != INT_MAX);
		
		for(i=0;i<tottempfile;i++)
		{
			fclose(tempfileptr[i]);
		}
		free(tempfileptr);
	}
	fclose(opfile);
}

void multiStepMergeSort(char* idxFile,char* sortedIdxFile)
{
	int i=0,rec[1000],len=0,tottempfile,j,tempkey,runkey;
	int position=0,minelement=INT_MAX,count=0,count2=0;
	int oprec[1000],filenum=0,srunsnum=0,counter=0;
	FILE* ipfile,* opfile, * temp, **tempfileptr;
	char seq[5]="",tempfile[50]="",stempfile[50]="";
	
	ipfile = fopen(idxFile,"rb");
	if(!ipfile)
	{
		printf("Unable to open file\n");
		return;
	}
	fseek(ipfile,0,SEEK_END);
	len=ftell(ipfile);
	len /= sizeof(int);
	fseek(ipfile,0,SEEK_SET);
	
	if(len <= 1000)
	{
		sprintf(tempfile,"%s",idxFile);
		sprintf(seq,"%.03d",i);
		strcat(tempfile,".");
		strcat(tempfile,seq);
		
		fread(&rec,sizeof(int),len,ipfile);
		qsort(rec,len,sizeof(int),comparator);
		temp=fopen(tempfile,"wb");
		fwrite(rec,sizeof(int),len,temp);
		opfile=fopen(sortedIdxFile,"wb");
		fwrite(rec,sizeof(int),len,opfile);
		fclose(temp);
		fclose(ipfile);
	}
	else
	{
		tottempfile = len/1000;
		
		if(tottempfile*1000 != len)
			tottempfile++;
		
		for(i=0;i<tottempfile;i++)
		{
			sprintf(tempfile,"%s",idxFile);
			sprintf(seq,"%.03d",i);
			strcat(tempfile,".");
			strcat(tempfile,seq);
			
			for(j=0;j<1000;j++)
			{
				if(fread(&tempkey,sizeof(int),1,ipfile) == 1)
				{
					rec[j]=tempkey;
					count++;
				}
				else
					break;
			}
			qsort(rec,count,sizeof(int),comparator);
			
			temp=fopen(tempfile,"wb");
			fwrite(rec,sizeof(int),count,temp);
			fclose(temp);
			count=0;
		}
		
		fclose(ipfile);
		
		for(i=0;i<1000;i++)
			rec[i]=INT_MAX;
		
		//Creating Super Runs
		
		srunsnum = tottempfile/15;
		if(srunsnum*15 < tottempfile)
			srunsnum++;
			
		int tempfilecount=tottempfile;
			
		for(i=0;i<srunsnum;i++)
		{
			sprintf(stempfile,"%s",idxFile);
			sprintf(seq,"%.03d",i);
			strcat(stempfile,".super.");
			strcat(stempfile,seq);
			
			if(tempfilecount - 15 > 0)
			{
				counter=15;
				tempfilecount -= 15;
			}
			else
			{
				counter = tempfilecount;
			}
			
			tempfileptr = malloc(sizeof(FILE*)*counter);
			for(j=0;j<counter;j++)
			{
				sprintf(tempfile,"%s",idxFile);
				sprintf(seq,"%.03d",count2);
				strcat(tempfile,".");
				strcat(tempfile,seq);
					
				tempfileptr[j]=fopen(tempfile,"rb");
				count2++;
			}
			
			runkey = 1000/counter;
			for(j=0;j<counter;j++)
			{	
				fread(&rec[j*runkey],sizeof(int),runkey,tempfileptr[j]);
			}
			
			temp=fopen(stempfile,"wb");
			
			do
			{
				minelement = INT_MAX;
				for(j=0;j<1000;j++)
				{
					if(minelement > rec[j])
					{
						minelement = rec[j];
						position = j;
					}
				}
				
				if(count<1000 && minelement != INT_MAX)
				{
					oprec[count]=minelement;
					
					count++;
					filenum = (position)/runkey;

					int tempkeystore;
				
					if(fread(&tempkeystore,sizeof(int),1,tempfileptr[filenum])==1)
					{
						rec[position]=tempkeystore;
					}
					else
					{
						rec[position]=INT_MAX;
					}
					
				}
				else
				{
					fwrite(oprec,sizeof(int),count,temp);
					memset(oprec,0,1000*sizeof(int));
					count=0;
				}

			}while(minelement != INT_MAX);
			
			fclose(temp);
			for(j=0;j<counter;j++)
			{		
				fclose(tempfileptr[j]);
			}
			free(tempfileptr);
		}
		
		//Merging Super Runs
		
		for(i=0;i<1000;i++)
			rec[i]=INT_MAX;
			
		tempfileptr = malloc(sizeof(FILE*)*srunsnum);
		
		for(i=0;i<srunsnum;i++)
		{
			sprintf(stempfile,"%s",idxFile);
			sprintf(seq,"%.03d",i);
			strcat(stempfile,".super.");
			strcat(stempfile,seq);
			
			tempfileptr[i]=fopen(stempfile,"rb");
		}
		
		runkey=1000/srunsnum;
		for(i=0;i<srunsnum;i++)
		{	
			fread(&rec[i*runkey],sizeof(int),runkey,tempfileptr[i]);
		}
		
		opfile=fopen(sortedIdxFile,"wb");
		count=0;
		do
		{
			minelement = INT_MAX;
			for(i=0;i<1000;i++)
			{
				if(minelement > rec[i])
				{
					minelement = rec[i];
					position = i;
				}
			}
			
			if(count<1000 && minelement != INT_MAX)
			{
				oprec[count]=minelement;

				count++;
				filenum = (position)/runkey;
	
				int tempkeystore;
				
				if(fread(&tempkeystore,sizeof(int),1,tempfileptr[filenum])==1)
				{
					rec[position]=tempkeystore;
				}
				else
				{
					rec[position]=INT_MAX;
				}
				
			}
			else
			{
				fwrite(oprec,sizeof(int),count,opfile);
				memset(oprec,0,1000*sizeof(int));
				count=0;
			}

		}while(minelement != INT_MAX);

		for(i=0;i<srunsnum;i++)
		{
			fclose(tempfileptr[i]);
		}
		free(tempfileptr);
	}
	fclose(opfile);
}

void replacementMergeSort(char* idxFile,char* sortedIdxFile)
{
	int i=0,rec[1000],len=0,tottempfile,j,tempkey,runkey;
	int position=0,minelement=INT_MAX,count=0, phsize=0;
	int oprec[1000],filenum=0,counter=0,foflag=0;
	int pheapsiz=0,sheapsiz=0,buffsiz=0,flag=0;
	FILE* ipfile,* opfile, * temp, **tempfileptr;
	char seq[5]="",tempfile[50]="",stempfile[50]="";
	
	ipfile = fopen(idxFile,"rb");
	if(!ipfile)
	{
		printf("Unable to open file\n");
		return;
	}
	fseek(ipfile,0,SEEK_END);
	len=ftell(ipfile);
	len /= sizeof(int);
	fseek(ipfile,0,SEEK_SET);
	
	//Creating Runs
	
	if(len <= 750)
	{
		fread(rec,sizeof(int),len,ipfile);
		buildMinHeap(rec,0,len);
		while(len > 0)
		{
			oprec[count]=rec[0];
			rec[0]=rec[len-1];
			len--;
			buildMinHeap(rec,0,len);
			count++;
		}
		
		temp = fopen(strcat(idxFile,".000"),"wb");
		fwrite(oprec,sizeof(int),count,temp);
		fclose(temp);
		opfile=fopen(sortedIdxFile,"wb");
		fwrite(oprec,sizeof(int),count,opfile);
		fclose(ipfile);
	}
	else if(len > 750 && len <= 1000)
	{
		flag=1;
		fread(rec,sizeof(int),750,ipfile);
		fread(&rec[750],sizeof(int),len-750,ipfile);
		
		buildMinHeap(rec,0,750);
		pheapsiz=750;
		buffsiz=len-750;
	}
	else
	{
		flag=1;
		fread(rec,sizeof(int),750,ipfile);
		fread(&rec[750],sizeof(int),250,ipfile);
		
		buildMinHeap(rec,0,750);
		pheapsiz=750;
		buffsiz=250;
	}
	
	if(flag == 1)
	{
			while(len>0)
			{
				if(buffsiz != 0)
				{
					if(pheapsiz > 0 && count < 1000)
					{
						runkey = rec[0];
						oprec[count]=runkey;

						len--;
						count++;
						if(runkey<=rec[750])
						{
							rec[0]=rec[750];
						}
						else
						{
							rec[0]=rec[pheapsiz-1];
							rec[pheapsiz-1]=rec[750];
							pheapsiz--;
							sheapsiz++;
							
						}
						
						buildMinHeap(rec,0,pheapsiz);
						
						for(i=750;i<750+buffsiz;i++)
								rec[i]=rec[i+1];
							
						fread(&tempkey,sizeof(int),1,ipfile);
						
						if(!feof(ipfile))
						{
							rec[999]=tempkey;
						}
						else
						{
							buffsiz--;
						}
					}
					else
					{
						if(foflag == 0)
						{
							sprintf(tempfile,"%s",idxFile);
							sprintf(seq,"%.03d",filenum);
							strcat(tempfile,".");
							strcat(tempfile,seq);
							
							temp=fopen(tempfile,"wb");
							foflag=1;
						}
						
						fwrite(oprec,sizeof(int),count,temp);
						count=0;
						if(pheapsiz == 0)
						{
							pheapsiz=sheapsiz;
							sheapsiz=0;
							fclose(temp);
							filenum++;
							foflag=0;
							buildMinHeap(rec,0,pheapsiz);
						}
					}
				}
				else
				{
					if(foflag == 0)
					{
						sprintf(tempfile,"%s",idxFile);
						sprintf(seq,"%.03d",filenum);
						strcat(tempfile,".");
						strcat(tempfile,seq);
				
						temp=fopen(tempfile,"wb");
						foflag=1;
						phsize=pheapsiz;
					}
					oprec[count]=rec[0];
					rec[0]=rec[pheapsiz-1];
					len--;
					pheapsiz--;
					buildMinHeap(rec,0,pheapsiz);
					count++;
					
					if(pheapsiz == 0 || count >= 1000)
					{
						fwrite(oprec,sizeof(int),count,temp);
						count=0;
						if(pheapsiz ==0)
						{
							for(j=0;j<sheapsiz;j++)
								rec[j]=rec[j+phsize];
							pheapsiz=sheapsiz;
							sheapsiz=0;
							buildMinHeap(rec,0,pheapsiz);
							foflag=0;
							filenum++;
							fclose(temp);
						}
					}
				}
			}
			fclose(ipfile);
			
		//Merging Runs
		
			tottempfile=filenum;
			count=0;
			
			for(i=0;i<1000;i++)
			rec[i]=INT_MAX;
		
		
		tempfileptr = malloc(sizeof(FILE*)*tottempfile);
		for(i=0;i<tottempfile;i++)
		{
			sprintf(tempfile,"%s",idxFile);
			sprintf(seq,"%.03d",i);
			strcat(tempfile,".");
			strcat(tempfile,seq);
			
			tempfileptr[i] = fopen(tempfile,"rb");
		}
		
		runkey=1000/tottempfile;
	
		for(i=0;i<tottempfile;i++)
		{	
			fread(&rec[i*runkey],sizeof(int),runkey,tempfileptr[i]);
		}
		
		opfile=fopen(sortedIdxFile,"wb");
		
		do
		{
			minelement = INT_MAX;
			for(i=0;i<1000;i++)
			{
				if(minelement > rec[i])
				{
					minelement = rec[i];
					position = i;
				}
			}
			
			if(count<1000 && minelement != INT_MAX)
			{
				oprec[count]=minelement;

				count++;
				filenum = (position)/runkey;
	
				int tempkeystore;
			
				if(fread(&tempkeystore,sizeof(int),1,tempfileptr[filenum])==1)
				{
					rec[position]=tempkeystore;
				}
				else
				{
					rec[position]=INT_MAX;
				}
				
			}
			else
			{
				fwrite(oprec,sizeof(int),count,opfile);
				memset(oprec,0,1000*sizeof(int));
				count=0;
			}

		}while(minelement != INT_MAX);
		
		for(i=0;i<tottempfile;i++)
		{
			fclose(tempfileptr[i]);
		}
		free(tempfileptr);
	}
	fclose(opfile);
}

void buildMinHeap(int* heap,int index,int size)
{
	int i = size/2;
	while(i>=index)
	{
		heapify(heap,i,size);
		i--;
	}
}

void heapify(int* heap,int index,int size)
{
	int left,right,min;
	
	while(index<size/2)
	{
		min=index;
		
		left=2*index;
		right=left+1;
		
		if(left<size && heap[left]<=heap[min])
			min=left;
			
		if(right<size && heap[right]<=heap[min])
			min=right;
		
		if(min==index)
			return;
		
		swap(heap,index,min);
		
		index=min;
	}
}

void swap(int* heap,int index,int min)
{
	int temp;
	temp=heap[index];
	heap[index]=heap[min];
	heap[min]=temp;
}
