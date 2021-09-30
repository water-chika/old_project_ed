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

int current_line_address = 0;

typedef struct
{
	int start;
	int end;
}range_t;

typedef struct
{
	int number;
	line_t* lines;
	int size;
}lines_t;

void init_lines(lines_t* lines)
{
	lines->number = 0;
	lines->size = 8;
	lines->lines = (line_t*)malloc(sizeof(line_t)*lines->size);
	memset(lines->lines, 0, sizeof(line_t)*lines->size);
}
void free_lines(lines_t* lines)
{
	for (int i = 0; i < lines->number; i++)
	{
		free(lines->lines[i].buffer);
	}
	free(lines->lines);
}

int get_lines(lines_t* lines, FILE* file)
{
	while (0 < getline(& lines->lines[lines->number].buffer, &lines->lines[lines->number].size, file))
	{
		if (0 == strcmp(".\n", lines->lines[lines->number].buffer))
		{
			break;
		}
		lines->number++;
		if (lines->number == lines->size)
		{
			lines->size = 2 * lines->size;
			lines->lines = (line_t*)realloc(lines, sizeof(line_t)*lines->size);
			memset(lines->lines + lines->number, 0, sizeof(line_t)*lines->size);
		}
	}
}

int lines_delete(lines_t* lines, range_t range)
{
	for (int i = range.start-1; i<range.end; i++)
	{
		free(lines->lines[i].buffer);
	}
	memmove(lines->lines+range.start-1, lines->lines+range.end, sizeof(line_t)*(lines->number - range.end));
	lines->number -= range.end - range.start +1;
}

int lines_append(lines_t* lines, int address, lines_t* append_lines)
{
	if (lines->size < lines->number + append_lines->number)
	{
		lines->size = lines->number + append_lines->number;
		lines->lines = (line_t*)realloc(lines->lines, sizeof(line_t)*lines->size);
	}

	memmove(lines->lines+append_lines->number + address, lines->lines+address, sizeof(line_t)*(lines->number-address));
	memcpy(lines->lines+address, append_lines->lines, sizeof(line_t)*append_lines->number);
	memset(append_lines->lines, 0, append_lines->number*sizeof(line_t));
	lines->number += append_lines->number;
	current_line_address = address+append_lines->number;
	append_lines->number = 0;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "too few arguments\n");
		exit(-1);
	}
	FILE* file = fopen(argv[1], "a+");

	lines_t lines;
	init_lines(&lines);
	
	fseek(file,0,SEEK_SET);
	while (0 < getline(&(lines.lines[lines.number].buffer), &(lines.lines[lines.number].size), file))
	{
		lines.number++;
		if (lines.number == lines.size)
		{
			lines.size = 2*lines.size;
			lines.lines = (line_t*)realloc(lines.lines, sizeof(line_t)*lines.size);
			memset(lines.lines + lines.number, 0, sizeof(line_t)*lines.size);
		}
	}
	fclose(file);
	current_line_address = lines.number;

	line_t user_line = {NULL,0};
	while (printf("*"), 0 < getline(&user_line.buffer, &user_line.size, stdin))
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
				if (*next == '$')
				{
					range.end = lines.number;
					next++;
				}
				else{
					 range.end = strtol(next,
						&tailptr,
						0);
					next = tailptr;
				}
			}
			else
			{
				range.end = range.start;
			}
		}
		if (*next == 'p')
		{
			assert(0 <= range.start && range.start <= lines.number);
			assert(0 <= range.end && range.end <= lines.number);
			if (range.start == 0);
			else
			for (int i = range.start-1; i < range.end; i++)
				printf("%s", lines.lines[i].buffer);
		}
		else if (*next == 'a')
		{
			assert(0 <= range.start && range.start <= lines.number);
			lines_t append_lines;
			init_lines(&append_lines);
			get_lines(&append_lines, stdin);
			printf("lines %d\n", append_lines.number);
			lines_append(&lines, range.start, &append_lines);
			free_lines(&append_lines);
		}
		else if (*next == 'd')
		{
			assert(0 <= range.start && range.start <= lines.number);
			lines_delete(&lines, range);
		}
		if (user_line.buffer[0] == 'w')
		{
			FILE* file = fopen(argv[1], "w");
			for (int i = 0; i < lines.number; i++)
			{
				fprintf(file, "%s", lines.lines[i].buffer);
			}
		}
		if (user_line.buffer[0] == 'q')
		{
			break;
		}
	}

	
	free_lines(&lines);
	return 0;
}
