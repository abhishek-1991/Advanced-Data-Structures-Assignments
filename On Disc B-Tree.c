/**************************************************************
* Name: Abhishek Kumar Srivastava
* Student ID: 200203011
* Unity ID: asrivas3
* Description: CSC-541 Assignment 4 - Implementation of B-Tree
* *************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

FILE* op=NULL;
long treeroot = -1;
int order=0;

typedef struct
{
	int n;
	int *key;
	long *child;
}btree_node;

struct node
{
	long val;
	struct node* next;
};

typedef struct
{
	struct node *front, *rear;
}queue;

typedef struct
{
	struct node *top;
}stack;


void addelement(int);
void findelement(int);
void displaytree();
void end();
int comparator(const void*,const void*);
void splitnode(int*,long*,stack*);
void enqueue(queue*, long);
long dequeue(queue*);
queue* initqueue();
void display();
void destructnode(btree_node*);
void printqueue(queue*);
stack* initstack();
void pushstack(stack*, long);
void popstack(stack*);
btree_node initbtreenode();

int main(int argc, char* argv[])
{
	char* outputFile = argv[1],*temp;
	order = atoi(argv[2]);
	char cmd[200];
	int element;
	
	if(argc != 3)
	{
		printf("ERROR: Use command as assn_4 <output_file> <order>");
		exit(-1);
	}
	
	op=fopen(outputFile,"r+b");
	
	if(op == NULL)
	{
		op=fopen(outputFile,"w+b");
		fwrite(&treeroot,sizeof(long),1,op);
	}
	else
	{
		fread(&treeroot,sizeof(long),1,op);
	}
	
	while(-1)
	{
		element = 0;
		fgets(cmd,200,stdin);

		temp = strtok(cmd, " \n");
		
		if(!strcmp(temp,"add"))
		{
			temp = strtok(NULL," \n");
			//printf("%s",temp);
			element = atoi(temp);
			addelement(element);
		}
		else if(!strcmp(temp,"find"))
		{
			temp = strtok(NULL," \n");
			element = atoi(temp);
			findelement(element);
		}
		else if(!strcmp(temp,"print"))
		{
			displaytree();
		}
		else if(!strcmp(temp,"end"))
		{
			end();
		}
		else
		{
			printf("Wrong Command!!! Try again...\n");
		}
	}
}

//Function to add element in the B-Tree
void addelement(int element)
{
	//printf("DEBUG: Entered addelement with value:%d\n",element);
	int list[order],pathidx=0,i,idx=0;
	btree_node treenode;
	long* childlist = (long*)calloc(order, sizeof(long));
	stack* pathstk = initstack();

	treenode.n = 0;
	treenode.key = (int*)calloc(order-1,sizeof(int));
	treenode.child = (long*)calloc(order,sizeof(long));
	
	memset(list,0,sizeof(int)*order);
	
	//printf("DEBUG: treeroot=%ld\n",treeroot);
	if(treeroot == -1)
	{
		//printf("DEBUG\n");
		treenode.key[treenode.n]=element;
		treenode.n++;
		//printf("%d %d\n",treenode.key[treenode.n],treenode.n);
		treeroot = sizeof(long);
		fseek(op,sizeof(long),SEEK_SET);
		fwrite(&treenode.n,sizeof(int),1,op);
		fwrite(treenode.key,sizeof(int),order-1,op);
		fwrite(treenode.child,sizeof(long),order,op);
	}
	else
	{
		fseek(op,treeroot,SEEK_SET);
		fread(&treenode.n,sizeof(int),1,op);
		fread(treenode.key,sizeof(int),order-1,op);
		fread(treenode.child,sizeof(long),order,op);
		
		for(i=0;i<treenode.n;i++)
		{
			if(treenode.key[i] == element)
			{
				printf("Entry with key=%d already exists\n", element);
				return;
			}
		}
		
		pushstack(pathstk,treeroot);
		//printf("Added: %ld\n", path[pathidx]);
		//pathidx++;
		
		
		while(treenode.child[0] != 0)
		{
			//printf("DEBUG1\n");
			for(i=0;i<treenode.n;i++)
			{
				if(treenode.key[i] == element)
				{
					printf("Entry with key=%d already exists\n", element);
					return;
				}
				else if (element > treenode.key[i] && element < treenode.key[i + 1])
				{
					//printf("DEBUG\n");
					i = i + 1;
					break;
				}
				/*else if (element > treenode.key[i] && treenode.key[i + 1] == 0)
				{
				i++;
				break;
				}*/
				else if(treenode.key[i]>element)
					break;
			}
			//printf("%ld\n", treeroot);
			//printf("%ld\n", treenode.child[i]);
			//path = realloc(path,sizeof(long)*(pathidx+1));
			pushstack(pathstk, treenode.child[i]);
			//printf("Added: %ld\n", path[pathidx]);
			//pathidx++;
			
			fseek(op, treenode.child[i],SEEK_SET);
			fread(&treenode.n,sizeof(int),1,op);
			fread(treenode.key,sizeof(int),order-1,op);
			fread(treenode.child,sizeof(long),order,op);
			
			for(i=0;i<treenode.n;i++)
			{
				if(treenode.key[i] == element)
				{
					printf("Entry with key=%d already exists\n", element);
					return;
				}
			}
		}
		
		//printf("DEBUG2\n");
		for(i=0;i<treenode.n;i++)
			list[i]= treenode.key[i];
			
		list[treenode.n]=element;
		qsort(list, treenode.n+1,sizeof(int),comparator);
		//rearrange(list, treenode.n + 1);
		if(treenode.n != order-1)
		{
			for(i=0;i<treenode.n+1;i++)
				treenode.key[i]=list[i];
			treenode.n++;

			fseek(op,pathstk->top->val,SEEK_SET);
			fwrite(&treenode.n,sizeof(int),1,op);
			fwrite(treenode.key,sizeof(int),order-1,op);
			fwrite(treenode.child,sizeof(long),order,op);
		}
		else
		{
			for(i=0;i<treenode.n+1;i++)
				childlist[i]= treenode.child[i];
			splitnode(list,childlist,pathstk);
		}
	}
	destructnode(&treenode);
}

//Function to find element in B-Tree
void findelement(int element)
{
	btree_node node;
	btree_node n = initbtreenode();
	int i;

	node.n = 0;
	node.key = (int*)calloc(order - 1, sizeof(int));
	node.child = (long*)calloc(order, sizeof(long));

	fseek(op, treeroot, SEEK_SET);
	fread(&node.n, sizeof(int), 1, op);
	fread(node.key, sizeof(int), order - 1, op);
	fread(node.child, sizeof(long), order, op);

	for (i = 0; i < node.n; i++)
	{
		if (node.key[i] == element)
		{
			printf("Entry with key=%d exists\n", element);
			return;
		}
	}
	
	while (node.child[0] != 0)
	{
		for (i = 0; i < node.n; i++)
		{
			if (node.key[i] == element)
			{
				printf("Entry with key=%d exists\n", element);
				return;
			}
			else if (node.key[i]<element && node.key[i + 1]>element)
			{
				i = i + 1;
				break;
			}
			else if (node.key[i] > element)
				break;
		}

		fseek(op, node.child[i], SEEK_SET);
		fread(&node.n, sizeof(int), 1, op);
		fread(node.key, sizeof(int), order - 1, op);
		fread(node.child, sizeof(long), order, op);

		for (i = 0; i < node.n; i++)
		{
			if (node.key[i] == element)
			{
				printf("Entry with key=%d exists\n", element);
				return;
			}
		}
	}
	
	printf("Entry with key=%d does not exist\n", element);
	
}

//Function to print B-Tree
void displaytree()
{
	int index = 0;

	if (treeroot != -1)
	{
		//printf("Display\n");
		display();
	}
	else
	{
		printf("%2d:", index);
	}
}

//Function to end the program and write root offset in the file
void end()
{
	fseek(op,0,SEEK_SET);
	fwrite(&treeroot,sizeof(long),1,op);
	fclose(op);
	exit(0);
}

//Comparator for qsort
int comparator(const void* a,const void* b)
{
	return (*(int*)a - *(int*)b);
}

//Recursive Function for splitting node
void splitnode(int*list,long*childlist,stack* stk)
{
	//printf("Entered Split\n");
	int indx = 0;
	btree_node left, right, temp;
	int mid = order / 2, median, i, idx, templistsize;
	long loffset, roffset, tempoffset, *templist;
	
	//printf("%d\n", temp.n);
	temp.n=0;
	temp.key = (int*)calloc(order-1,sizeof(int));
	temp.child = (long*)calloc(order,sizeof(long));
	
	left.n=mid;
	left.key = (int*)calloc(order-1,sizeof(int));
	left.child = (long*)calloc(order,sizeof(long));
	
	right.n=order-1-mid;
	right.key = (int*)calloc(order-1,sizeof(int));
	right.child = (long*)calloc(order,sizeof(long));
	
	median=list[mid];
	
	for (i = 0; i<right.n + 1; i++)
		right.child[i] = childlist[mid + i + 1];

	for (i = 0; i<left.n + 1; i++)
		left.child[i] = childlist[i];

	for (i = 0; i < left.n; i++)
		left.key[i] = list[i];
		
	for(i=0;i<right.n;i++)
		right.key[i]=list[mid+i+1];
		
	loffset=stk->top->val;
	//printf("%ld ", loffset);
	
	fseek(op,loffset,SEEK_SET);
	fwrite(&left.n,sizeof(int),1,op);
	fwrite(left.key,sizeof(int),order-1,op);
	fwrite(left.child,sizeof(long),order,op);
	//path = realloc(path,sizeof(long)*pathidx);
	popstack(stk);
	fseek(op,0,SEEK_END);
	roffset=ftell(op);
	//printf("%ld ", roffset);
	fseek(op,roffset,SEEK_SET);
	fwrite(&right.n,sizeof(int),1,op);
	fwrite(right.key,sizeof(int),order-1,op);
	fwrite(right.child,sizeof(long),order,op);
	//printf("DEBUG: %d\n", pathidx);
	if (stk->top == NULL)
	{
		//printf("DEBUG1\n");
		fseek(op, 0, SEEK_END);
		tempoffset = ftell(op);
		temp.key[temp.n] = median;
		temp.n++;
		temp.child[0] = loffset;
		temp.child[1] = roffset;
		treeroot = tempoffset;
		//printf("%ld ", treeroot);
		fseek(op, treeroot, SEEK_SET);
		fwrite(&temp.n, sizeof(int), 1, op);
		fwrite(temp.key, sizeof(int), order-1, op);
		fwrite(temp.child, sizeof(long), order, op);
	}
	else
	{
		//printf("DEBUG\n");
		fseek(op,stk->top->val,SEEK_SET);
		fread(&temp.n,sizeof(int),1,op);
		fread(temp.key,sizeof(int),order-1,op);
		fread(temp.child,sizeof(long),order,op);
		
		for(i=0;i<order;i++)
			childlist[i]=temp.child[i];
		
		list[order-1]=0;
		for(i=0;i<order-1;i++)
			list[i]=temp.key[i];
		
		childlist[order]=0;
		list[temp.n]=median;
		//rearrange(list, order);
		qsort(list, temp.n + 1, sizeof(int), comparator);
		for (i = 0; i < order; i++)
		{
			if (list[i] == median)
			{
				idx = i;
				break;
			}
		}

		templistsize = order - idx - 1;
		templist = (long*)calloc(templistsize, sizeof(long));
		for (i = 0; i < templistsize; i++)
		{
			templist[i] = childlist[idx + 1 + i];
		}

		childlist[idx + 1] = roffset;
		for (i = 0; i < templistsize; i++)
		{
			childlist[idx + 2 + i] = templist[i];
		}

		if (temp.n != order - 1)
		{
			for (i = 0; i < order - 1; i++)
				temp.key[i] = list[i];

			temp.n++;

			for (i = 0; i < order; i++)
				temp.child[i] = childlist[i];

			fseek(op, stk->top->val, SEEK_SET);
			fwrite(&temp.n, sizeof(int), 1, op);
			fwrite(temp.key, sizeof(int), order - 1, op);
			fwrite(temp.child, sizeof(long), order, op);
		}
		else
		{
			splitnode(list, childlist, stk);
		}

		free(templist);
	}

	destructnode(&temp);
	destructnode(&right);
	destructnode(&left);

}

//Function to initialize Queue
queue* initqueue()
{
	queue *q = (queue*)malloc(sizeof(queue));
	q->front = q->rear = NULL;
	return q;
}

//Function to add element in Queue
void enqueue(queue* q, long val)
{
	struct node* temp = (struct node*)malloc(sizeof(struct node));
	temp->val = val;
	temp->next = NULL;
	
	if (q->rear == NULL)
	{
		q->front = q->rear = temp;
		return;
	}

	q->rear->next = temp;
	q->rear = temp;
}

//Function to delete element first element from queue
long dequeue(queue*q)
{
	struct node* temp;
	long res;

	if (q->front == NULL)
		return ;

	temp = q->front;
	q->front = q->front->next;

	if (q->front == NULL)
		q->rear = NULL;

	res = temp->val;
	free(temp);

	return(res);
}

//Subfunction to print B-Tree
void display()
{
	queue *primary = initqueue(), *secondary = initqueue();
	long tempoffset, childoffset;
	int i,index=1;

	printf("%2d: ", index);
	enqueue(primary, treeroot);
	while (primary->front != NULL)
	{
		btree_node treenode;
		treenode.n = 0;
		treenode.key = (int*) calloc(order - 1, sizeof(int));
		treenode.child = (long*)calloc(order, sizeof(long));

		tempoffset = dequeue(primary);
		//printf("%ld\n",tempoffset);

		fseek(op, tempoffset, SEEK_SET);
		fread(&treenode.n, sizeof(int), 1, op);
		fread(treenode.key, sizeof(int), order-1, op);
		fread(treenode.child, sizeof(long), order, op);

		//printf("n=%d\n",treenode.n-1);
		for (i = 0; i < treenode.n-1; i++)
		{
			printf("%d,", treenode.key[i]);
			//printf("DEBUG\n");
		}
		printf("%d ", treenode.key[treenode.n-1]);

		for (i = 0; i < order; i++)
		{
			if (treenode.child[i] != 0)
			{
				enqueue(secondary, treenode.child[i]);
				//printf("%ld ", treenode.child[i]);
			}
		}

		//printqueue(secondary);

		if (primary->front == NULL && secondary->front != NULL)
		{
			//printf("DEBUG\n");
			while (secondary->front != NULL)
			{
				childoffset = dequeue(secondary);
				//printf("%ld ", childoffset);
				enqueue(primary, childoffset);
			}
			index++;
			printf("\n%2d: ",index);
		}
	}
	printf("\n");
}

//Function for deleting allocated memory in B-Tree node
void destructnode(btree_node* node)
{
	free(node->key);
	free(node->child);
}

//Function for printing contents of Queue
void printqueue(queue* q)
{
	struct node* temp = q->front;
	while (temp != NULL)
	{
		printf("%ld ", temp->val);
		temp = temp->next;
	}
	printf("\n");
}

//Function to initialize stack
stack* initstack()
{
	stack* stk=(stack*)malloc(sizeof(stack));
	stk->top = NULL;
	return stk;
}

//Function for pushing values in stack
void pushstack(stack* s, long val)
{
	struct node* temp = (struct node*)malloc(sizeof(struct node));
	temp->val = val;
	temp->next = s->top;
	s->top = temp;
}

//Function for removing element from stack
void popstack(stack* s)
{
	struct node* tmp = s->top;
	s->top = s->top->next;
	free(tmp);
}

//Function for initializing B-Tree Node
btree_node initbtreenode()
{
	btree_node n;
	n.n = 0;
	n.key = (int*)calloc(order - 1, sizeof(int));
	n.child = (long*)calloc(order, sizeof(long));

	return n;
}