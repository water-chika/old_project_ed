#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct
{
	char* buffer;
	size_t size;
}line_t;

int lines_number = 0;
line_t* lines;
int lines_size = 0;
int current_line_address = 0;

typedef struct
{
	int start;
	int end;
}range_t;

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "too few arguments\n");
		exit(-1);
	}
	FILE* file = fopen(argv[1], "a+");

	lines_size = 8;
	lines = (line_t*)malloc(sizeof(line_t)*lines_size);
	memset(lines, 0, sizeof(line_t)*lines_size);
	lines_number = 0;
	
	fseek(file,0,SEEK_SET);
	while (0 < getline(&(lines[lines_number].buffer), &(lines[lines_number].size), file))
	{
		lines_number++;
		if (lines_number == lines_size)
		{
			lines_size = 2*lines_size;
			lines = (line_t*)realloc(lines, sizeof(line_t)*lines_size);
		}
	}
	fclose(file);
	current_line_address = lines_number;

	line_t user_line = {NULL,0};
	while (0 < getline(&user_line.buffer, &user_line.size, stdin))
	{
		char* next = user_line.buffer;
		range_t range = {current_line_address,current_line_address};
		if (isdigit(*next))
		{
			char* tailptr;
			range.start = strtol(next,
					&tailptr,
					0);
			next = tailptr;
			if (*next== ',')
			{
				next++;
				range.end = strtol(next,
						&tailptr,
						0);
				next = tailptr;
			}
			else
			{
				range.end = range.start;
			}
		}
		if (*next == 'p')
		{
			assert(0 < range.start && range.start <= lines_number);
			assert(0 < range.end && range.end <= lines_number);
			for (int i = range.start-1; i < range.end; i++)
				printf("%s", lines[i].buffer);
		}
		else if (*next = 'a')
		{
			assert(0 <= range.start && range.start <= lines_number);
		}
		if (user_line.buffer[0] == 'w')
		{
			FILE* file = fopen(argv[1], "w");
			for (int i = 0; i < lines_number; i++)
			{
				fprintf(file, "%s", lines[i].buffer);
			}
		}
		if (user_line.buffer[0] == 'q')
		{
			break;
		}
	}

	
	free(lines);
	lines_size = 0;
	lines_number = 0;
	return 0;
}
