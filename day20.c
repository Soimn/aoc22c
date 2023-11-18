#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Node
{
	struct Node* next;
	struct Node* prev;
	long long value;
} Node;

void
BuildRingbuffer(char* input, Node** ringbuffer, Node*** orderbuffer, unsigned long* ringbuffer_size)
{
	Node* prev = 0;
	for (char* scan = input; *scan != 0;)
	{
		long long sign = 1;
		if (*scan == '-') scan -= (sign = -1);

		assert((unsigned char)(*scan-'0') < 10u);

		long long value = 0;
		while ((unsigned char)(*scan-'0') < 10u) value = value*10 + (*(scan++)&0xF);

		Node* node = malloc(sizeof(Node));
		if (prev == 0) *ringbuffer = node;
		else           prev->next  = node;

		node->prev  = prev;
		node->value = value*sign;

		prev = node;
		*ringbuffer_size += 1;

		assert(*scan == '\r' || *scan == '\n' || *scan == 0);
		while (*scan == '\r' || *scan == '\n') ++scan;
	}

	(*ringbuffer)->prev = prev;
	prev->next = *ringbuffer;

	*orderbuffer = malloc((*ringbuffer_size)*sizeof(Node*));
	Node* scan = *ringbuffer;
	for (unsigned long i = 0; i < *ringbuffer_size; ++i, scan = scan->next) (*orderbuffer)[i] = scan;
}

void
MixRingbuffer(Node* ringbuffer, Node** orderbuffer, unsigned long ringbuffer_size, Node** zero_node)
{
	for (unsigned long i = 0; i < ringbuffer_size; ++i)
	{
		Node* scan = orderbuffer[i];

		if      (scan->value == 0) *zero_node = scan;
		else if (scan->value % (ringbuffer_size-1) != 0)
		{
			Node* dst = scan;

			if (scan->value < 0) for (unsigned long j = 0; j < (-scan->value+1) % (ringbuffer_size-1); ++j) dst = dst->prev;
			else                 for (unsigned long j = 0; j < scan->value      % (ringbuffer_size-1); ++j) dst = dst->next;

			if (dst == scan) dst = dst->prev;
			scan->prev->next = scan->next;
			scan->next->prev = scan->prev;
			
			scan->next = dst->next;
			scan->prev = dst;

			dst->next->prev = scan;
			dst->next       = scan;
		}
	}
}

long long
Sum(Node* ringbuffer, unsigned long ringbuffer_size, Node* zero_node)
{
	long long result = 0;

	for (unsigned int i = 0; i < 3; ++i)
	{
		unsigned long offset = (unsigned long[]){1000, 2000, 3000}[i] % ringbuffer_size;
		Node* node = zero_node;
		for (unsigned long j = 0; j < offset; ++j) node = node->next;
		printf("%lld, ", node->value);
		result += node->value;
	}

	return result;
}

int
main(int argc, char** argv)
{
	char* example_input = "1\r\n2\r\n-3\r\n3\r\n-2\r\n0\r\n4";
	FILE* file = fopen("day20_input.txt", "rb");
	fseek(file, 0, SEEK_END);
	unsigned long file_size = ftell(file);
	rewind(file);
	char* file_input = malloc(file_size);
	fread(file_input, 1, file_size, file);
	fclose(file);

	char* input = file_input;

	{ /// Part 1
		Node* ringbuffer              = 0;
		Node** orderbuffer            = 0;
		unsigned long ringbuffer_size = 0;
		BuildRingbuffer(input, &ringbuffer, &orderbuffer, &ringbuffer_size);

		Node* zero_node = 0;
		MixRingbuffer(ringbuffer, orderbuffer, ringbuffer_size, &zero_node);

		assert(zero_node != 0);

		printf("Part 1: %lld\n", Sum(ringbuffer, ringbuffer_size, zero_node));
	}

	{ /// Part 2
		Node* ringbuffer              = 0;
		Node** orderbuffer            = 0;
		unsigned long ringbuffer_size = 0;
		BuildRingbuffer(input, &ringbuffer, &orderbuffer, &ringbuffer_size);

		long long decryption_key = 811589153;

		ringbuffer->value *= decryption_key;
		for (Node* scan = ringbuffer->next; scan != ringbuffer; scan = scan->next) scan->value *= decryption_key;

		Node* zero_node = 0;
		for (unsigned int i = 0; i < 10; ++i) MixRingbuffer(ringbuffer, orderbuffer, ringbuffer_size, &zero_node);

		assert(zero_node != 0);

		printf("Part 2: %lld\n", Sum(ringbuffer, ringbuffer_size, zero_node));
	}

	return 0;
}
