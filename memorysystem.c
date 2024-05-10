#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MEMSIZE 500

struct __attribute__((packed)) framestatus
{
	int number;			 // frame number
	char name[8];		 // function name representing the frame
	int functionaddress; // address of function in code section (will be randomly generated in this case)
	int frameaddress;	 // starting address of frame belonging to this header in Stack
	bool used;			 // a boolean value indicating wheter the frame status entry is in use or not
};

struct freelist
{
	int start;			   // start address of free region
	int size;			   // size of free region
	struct freelist *next; // pointer to next free region
};

struct allocList
{
	int start;
	char name[8];
	struct allocList *next;
};

struct freelist head;
struct allocList top;

char mem[MEMSIZE];
int css = 200;
int chs = 100;
int stkptr = 395;
int FS[5];
int top_frame = 0;

int CF(char *name, int add)
{
	struct framestatus f;
	int off = 395;
	memcpy(&f, &mem[off], 21);
	int c = 1;
	while (f.used == 1)
	{
		if (strncmp(name, f.name, 8) == 0)
		{
			printf("function of this name exists. Exiting.\n");
			return 0;
		}
		else if (c > 5)
		{
			printf("not enough frames\n");
			return 0;
		}
		off += 21;
		c += 1;
		memcpy(&f, &mem[off], 21);
	}
	strcpy(f.name, name);
	f.functionaddress = add;
	f.used = 1;
	f.number = c;
	top_frame = c;
	FS[top_frame - 1] = 0;
	f.frameaddress = stkptr;
	stkptr -= 10;
	memcpy(&mem[off], &f, 21);
	printf("Frame created successfully.\n");
	return 1;
}

int checkStack(int size)
{
	if (((stkptr - size + 1) >= chs) && ((stkptr - size + 1) > 199))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void DF()
{
	struct framestatus f;
	if (top_frame > 0)
	{
		int off = 395 + 21 * (top_frame - 1);
		memcpy(&f, &mem[off], 21);
		stkptr = f.frameaddress;
		f.used = 0;
		memcpy(&mem[off], &f, 21);
		top_frame -= 1;
		css -= FS[top_frame];
		FS[top_frame] = 0;
		printf("Frame deleted successfully.\n");
	}
	else
	{
		printf("Error. Stack is empty.\n");
	}
}

void CI(char *name, int val)
{
	struct framestatus f;
	if (top_frame > 0)
	{
		int off = 395 + 21 * (top_frame - 1);
		memcpy(&f, &mem[off], 21);
		if ((FS[top_frame - 1] + sizeof(int)) < 80)
		{
			int temp = f.frameaddress - FS[top_frame - 1] - sizeof(int);
			memcpy(&mem[temp], &val, sizeof(int));
			FS[top_frame - 1] += sizeof(int);
			if (FS[top_frame - 1] > 10)
			{
				stkptr = temp;
			}
			printf("Integer created successfully.\n");
		}
		else
		{
			printf("Error. Not enough space  on frame.\n");
		}
	}
	else
	{
		printf("Error. Stack is empty.\n");
	}
}

void CD(char *name, double val)
{
	struct framestatus f;
	if (top_frame > 0)
	{
		int off = 395 + 21 * (top_frame - 1);
		memcpy(&f, &mem[off], 21);
		if ((FS[top_frame - 1] + sizeof(double)) < 80)
		{
			int temp = f.frameaddress - FS[top_frame - 1] - sizeof(double);
			memcpy(&mem[temp], &val, sizeof(double));
			FS[top_frame - 1] += sizeof(double);
			if (FS[top_frame - 1] > 10)
			{
				stkptr = temp;
			}
			printf("Double created successfully.\n");
		}
		else
		{
			printf("Error. Not enough space  on frame.\n");
		}
	}
	else
	{
		printf("Error. Stack is empty.\n");
	}
}

void CC(char *name, char val)
{
	struct framestatus f;
	if (top_frame > 0)
	{
		int off = 395 + 21 * (top_frame - 1);
		memcpy(&f, &mem[off], 21);
		if ((FS[top_frame - 1] + sizeof(char)) < 80)
		{
			int temp = f.frameaddress - FS[top_frame - 1] - sizeof(char);
			memcpy(&mem[temp], &val, sizeof(char));
			FS[top_frame - 1] += sizeof(char);
			if (FS[top_frame - 1] > 10)
			{
				stkptr = temp;
			}
			printf("Character created successfully.\n");
		}
		else
		{
			printf("Error. Not enough space  on frame.\n");
		}
	}
	else
	{
		printf("Error. Stack is empty.\n");
	}
}

int AP(int val)
{
	struct framestatus f;
	if (top_frame > 0)
	{
		int off = 395 + 21 * (top_frame - 1);
		memcpy(&f, &mem[off], 21);
		if ((FS[top_frame - 1] + sizeof(int)) < 80)
		{
			int temp = f.frameaddress - FS[top_frame - 1] - sizeof(int);
			memcpy(&mem[temp], &val, sizeof(int));
			FS[top_frame - 1] += sizeof(int);
			if (FS[top_frame - 1] > 10)
			{
				stkptr = temp;
			}
			printf("Pointer created successfully.\n");
			return 1;
		}
		else
		{
			printf("Error. Not enough space  on frame.\n");
			return 0;
		}
	}
	else
	{
		printf("Error. Stack is empty.\n");
		return 0;
	}
}

int expand(int num)
{
	if ((chs + num) > (MEMSIZE - css) || ((chs + num) > 300))
	{
		return 0;
	}
	else
	{
		chs += num;
		return 1;
	}
}

void CH(char *name, int size)
{
	if (top_frame > 0)
	{
		struct freelist *freenode = &head;
		int res = checkStack(sizeof(int));
		if (res == 1)
		{
			int z = AP(freenode->start);
			if (z == 0)
			{
				return;
			}
			if (stkptr < 300)
			{
				css = MEMSIZE - stkptr;
			}
		}
		else
		{
			printf("Not enough space on stack\n");
			return;
		}
		while ((freenode->size) < size + 4)
		{
			if (freenode->next != NULL)
			{
				freenode = freenode->next;
			}
			else
			{
				int sn = 4 + size - freenode->size;
				int x = expand(sn);
				if (x == 0)
				{
					printf("Not enough free space.\n");
					return;
				}
				else
				{
					freenode->size += sn;
				}
			}
		}
		struct allocList allocNode;
		allocNode.start = freenode->start;
		strcpy((allocNode.name), name);
		allocNode.next = NULL;
		setAlloc(&allocNode);
		char s = 'A';
		memcpy(&mem[freenode->start], &size, 4);
		for (int i = 4; i < (size + 4); i++)
		{
			if (s > 90)
			{
				s = 65;
			}
			mem[(freenode->start + i)] = s;
			s++;
		}
		struct freelist fnode;
		fnode.start = freenode->start + size + 4;
		fnode.size = freenode->size - (size + 4);
		fnode.next = NULL;
		int a = delFree(freenode);
		if (a == 0)
		{
			head = fnode;
		}
		else
		{
			setFree(fnode);
		}
	}
	else
	{
		printf("Error. Stack is empty.\n");
	}
}

void setFree(struct freelist nodd)
{
	struct freelist *node = (struct allocList *)malloc(sizeof(struct allocList));
	*node = nodd;
	struct freelist *freenode = &head;
	while ((freenode->start) < node->start)
	{
		if ((freenode->next) == NULL)
		{
			node->next = freenode->next;
			freenode->next = node;
			return;
		}
		else
		{
			struct freelist temp = *(freenode->next);
			if (temp.start > node->start)
			{
				node->next = freenode->next;
				freenode->next = node;
				return;
			}
			else
			{
				freenode = freenode->next;
			}
		}
	}
	struct freelist tem = head;
	head = *node;
	head.next = node;
	*node = tem;
}

void setAlloc(struct allocList *nodd)
{
	struct allocList *node = (struct allocList *)malloc(sizeof(struct allocList));
	*node = *nodd;
	struct allocList *allocNode = &top;
	while ((allocNode->start) < node->start)
	{
		if ((allocNode->next) == NULL)
		{
			allocNode->next = node;
			return;
		}
		else
		{
			struct allocList temp = *(allocNode->next);
			if (temp.start > node->start)
			{
				node->next = allocNode->next;
				allocNode->next = node;
				return;
			}
			else
			{
				allocNode = allocNode->next;
			}
		}
	}
	top = *node;
}

void delAlloc(struct allocList *node)
{
	struct allocList *allocNode = &top;
	while (allocNode != node)
	{
		struct allocList *temp = (allocNode->next);
		if (temp = node)
		{
			allocNode->next = temp->next;
			free(temp);
			return;
		}
		else
		{
			allocNode = allocNode->next;
		}
	}
	if (top.next)
	{
		top = *(top.next);
	}
	else
	{
		struct allocList tem;
		tem.next = NULL;
		top = tem;
	}
}

int delFree(struct freelist *node)
{
	struct freelist *freeNode = &head;
	while (freeNode != node)
	{
		struct freelist *temp = (freeNode->next);
		if (temp = node)
		{
			freeNode->next = temp->next;
			free(temp);
			return 1;
		}
		else
		{
			freeNode = freeNode->next;
		}
	}
	return 0;
}

void DH(char *name)
{
	if (top_frame > 0)
	{
		struct allocList *allocNode = &top;
		while (strncmp(name, allocNode->name, 8) != 0)
		{
			if ((allocNode->next) == NULL)
			{
				printf("No such buffer on heap.\n");
				return;
			}

			allocNode = allocNode->next;
		}
		struct freelist freeNode;
		freeNode.start = allocNode->start;
		memcpy(&(freeNode.size), &mem[freeNode.start], sizeof(int));
		freeNode.size += 4;
		freeNode.next = NULL;
		setFree(freeNode);
		delAlloc(allocNode);
		printf("Buffer deleted successfully.\n");
	}
	else
	{
		printf("Error. Stack is empty.\n");
	}
}

void freeAllocList()
{
	if (top.next)
	{
		struct allocList *node = top.next;
		while (node != NULL)
		{
			struct allocList *temp = node;
			node = node->next;
			free(temp); // Free the memory for each node
		}
	}
}

void freeFreeList()
{
	if (head.next)
	{
		struct freelist *node = head.next;
		while (node != NULL)
		{
			struct freelist *temp = node;
			node = node->next;
			free(temp); // Free the memory for each node
		}
	}
}

void SM()
{
	for (int i = 499; i > -1; i--)
	{
		if (i == 499)
		{
			printf("Framestatus Region Start\n");
		}
		else if (i == 394)
		{
			printf("Stack Frames Region Start\n");
		}
		else if (i == MEMSIZE - css)
		{
			printf("Free Region Start\n");
		}
		else if (i == chs)
		{
			printf("Heap Region Start\n");
		}
		printf("Memory index %d: %c\n", i, mem[i]);
	}
}

int main()
{
	printf("%d",sizeof(double));
	top.next = NULL;
	head.start = 0;
	head.size = 100;
	head.next = NULL;
	struct framestatus fs;
	int offset = 395;
	fs.used = 0;
	printf("%d\n", sizeof(struct framestatus));
	for (int i = 0; i < 5; i++)
	{
		memcpy(&mem[offset], &fs, sizeof(struct framestatus) - 3);
		offset += sizeof(struct framestatus) - 3;
	}
	char input[100];
	while (1)
	{
		printf("Enter command: ");
		if (fgets(input, sizeof(input), stdin) == NULL)
		{
			printf("Error reading input.\n");
			exit(1);
		}
		input[strcspn(input, "\n")] = '\0'; // Remove newline character from input
		char command[20];
		char arg1[20];
		char arg2[20];

		if (sscanf(input, "%19s %19s %19s", command, arg1, arg2) < 1)
		{
			printf("Invalid input format.\n");
			continue;
		}

		if (strcmp(command, "CF") == 0)
		{
			int addr;
			if (sscanf(arg2, "%d", &addr) != 1)
			{
				printf("Invalid address format for CF command.\n");
				continue;
			}
			int res = checkStack(10);
			if (res == 1)
			{
				int a = CF(arg1, addr);
				if ((a == 1) && (stkptr < 300))
				{
					css = MEMSIZE - stkptr;
				}
			}
			else
			{
				printf("Not enough space on stack\n");
			}
		}
		else if (strcmp(command, "DF") == 0)
		{
			DF();
		}
		else if (strcmp(command, "CI") == 0)
		{
			int int_value;
			if (sscanf(arg2, "%d", &int_value) != 1)
			{
				printf("Invalid integer value for CI command.\n");
				continue;
			}
			int res = checkStack(sizeof(int));
			if (res == 1)
			{
				CI(arg1, int_value);
				if (stkptr < 300)
				{
					css = MEMSIZE - stkptr;
				}
			}
			else
			{
				printf("Not enough space on stack\n");
			}
		}
		else if (strcmp(command, "CD") == 0)
		{
			double double_value;
			if (sscanf(arg2, "%lf", &double_value) != 1)
			{
				printf("Invalid double value for CD command.\n");
				continue;
			}
			int res = checkStack(sizeof(double));
			if (res == 1)
			{
				CD(arg1, double_value);
				if (stkptr < 300)
				{
					css = MEMSIZE - stkptr;
				}
			}
			else
			{
				printf("Not enough space on stack\n");
			}
		}
		else if (strcmp(command, "CC") == 0)
		{
			if (strlen(arg2) != 1)
			{
				printf("Invalid character value for CC command.\n");
				continue;
			}
			int res = checkStack(sizeof(char));
			if (res == 1)
			{
				CC(arg1, arg2[0]);
				if (stkptr < 300)
				{
					css = MEMSIZE - stkptr;
				}
			}
			else
			{
				printf("Not enough space on stack\n");
			}
		}
		else if (strcmp(command, "CH") == 0)
		{
			int size;
			if (sscanf(arg2, "%d", &size) != 1)
			{
				printf("Invalid size value for CH command.\n");
				continue;
			}
			CH(arg1, size);
		}
		else if (strcmp(command, "DH") == 0)
		{
			DH(arg1);
		}
		else if (strcmp(command, "SM") == 0)
		{
			SM();
		}
		else if (strcmp(command, "end") == 0)
		{
			printf("End command encountered. Exiting...\n");
			freeAllocList();
			freeFreeList();
			exit(0);
		}
		else
		{
			printf("Unknown command: %s\n", command);
		}
	}
	return 0;
}