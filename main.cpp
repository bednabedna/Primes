#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#define ALLOC(t, n) ((t*)malloc(sizeof(t) * (n)))
#define CALLOC(t, n) ((t*)calloc(n, sizeof(t)))
#define loop while(1)

typedef uint64_t Number;

Number getIndex3(Number x) { return x / 3; }
Number getLength3(Number max) { return getIndex3(max  -2 + (max % 2)) + 1; }
Number getValue3(Number x) { return x * 3 + 1 + (x%2); }

typedef uint64_t bitset;

#define SET_SIZE               64ULL
#define SET_DIV                 6ULL
#define SET_REM              0x3FULL
#define SET_EMPTY               0ULL
#define SET_FULLL 0xFFFFFFFFFFFFFFFF

#define SET_INDEX_MASK(n)                       ( 1ULL << ((n) & SET_REM) )
#define SET_CONTAINS_NUMBER(set, n)             ( (set) & SET_INDEX_MASK(n) )
#define SET_ADD_NUMBER(set, n)                  ( (set) | SET_INDEX_MASK(n) )
#define SET_REMOVE_NUMBER(set, n)               ( (set) & ~ SET_INDEX_MASK(n) )

#define SETS_LENGTH(elements)                   ( ((elements) >> SET_DIV) + 1ULL )
#define SETS_SET_INDEX_OF_NUMBER(n)             ( (n) >> SET_DIV )
#define SETS_SET_OF_NUMBER(sets, n)             ( (sets)[SETS_SET_INDEX_OF_NUMBER(n)] )


bool sets_contains_number(bitset* sets, Number n)
{
	return SET_CONTAINS_NUMBER(SETS_SET_OF_NUMBER(sets, n), n);
}

void sets_add_number(bitset* sets, Number n)
{
	SETS_SET_OF_NUMBER(sets, n) |= SET_INDEX_MASK(n);
}

void sets_remove_number(bitset * sets, Number n)
{
	SETS_SET_OF_NUMBER(sets, n) &= ~SET_INDEX_MASK(n);
}

Number sets_skip_numbers(bitset* sets, Number index)
{
	while (sets_contains_number(sets, index))
		++index;
	return index;
}

Number next_number(bitset* sets, Number current)
{
	return getValue3(sets_skip_numbers(sets, getIndex3(current) + 1));
}

Number findPrimes3_4(bitset** sets_ptr, Number max)
{
	bitset* sets = NULL;
	Number length = getLength3(max);
	Number found = 0;
	Number* post_eliminations = NULL;
	Number post_eliminations_length = max / 5 < 25 ? 0 : (getLength3(max / 5) / 5);

	if (sets = CALLOC(bitset, SETS_LENGTH(length)))
	{
		if ((!post_eliminations_length) || (post_eliminations = ALLOC(Number, post_eliminations_length)))
		{
			found = length + 1;
			// sets are initialized to 0s, bit set to 1 means number is not prime
			Number p = 5;
			loop{
				Number f = p, r = p * f;
				if (r > max)
				{
					break;
				}
				Number pre_eliminated = 0, post_eliminated = 0;
				Number limit = max / p;
				while (r <= limit)
				{
					post_eliminations[post_eliminated] = getIndex3(r);
					++post_eliminated;
					f = next_number(sets, f);
					r = p * f;
				}
				while (r <= max)
				{
					sets_add_number(sets, getIndex3(r));
					++pre_eliminated;
					f = next_number(sets, f);
					r = p * f;
				}
				for (Number i = 0; i < post_eliminated; ++i)
				{
					sets_add_number(sets, post_eliminations[i]);
				}
				p = next_number(sets, p);
				found -= pre_eliminated + post_eliminated;
			}
			*sets_ptr = sets;
			if (post_eliminations)
				free(post_eliminations);
		}
	}

	return found;
}

struct Primes {
	bitset* values;
	Number count, values_length;
};

Primes generatePrimes3_4(Number max)
{
	Primes primes = { NULL, 0, 0 };

	if (max < 5)
	{
		if (max > 2)
		{
			primes.count = 2;
		}
		else if(max == 2)
		{
			primes.count = 1;
		}
	}
	else
	{
		primes.count = findPrimes3_4(&primes.values, max);
		primes.values_length = SETS_LENGTH(getLength3(max));
	}
	return primes;
}

struct PrimesIter {
	Primes primes;
	Number index;
	Number count;
};

PrimesIter primes_iter_create(Primes primes)
{
	return { primes, 1, 0 };
}

Number primes_iter_next(PrimesIter* p)
{
	Number index = sets_skip_numbers(p->primes.values, p->index);
	p->index = index + 1;
	++p->count;
	return getValue3(index);
}

Number primes_iter_has_next(PrimesIter* p)
{
	return p->count < p->primes.count - 2;
}

struct Array {
	Number* values;
	Number count;
};

Array primes_to_array(Primes primes)
{
	Array result = { NULL, 0 };
	if (result.values = ALLOC(Number, primes.count))
	{
		result.count = primes.count;
		result.values[0] = 2;
		result.values[1] = 3;
		PrimesIter it = primes_iter_create(primes);
		for (Number i = 2; primes_iter_has_next(&it); ++i)
		{
			primes.values[i] = primes_iter_next(&it);
		}
	}
	return result;
}

#include <time.h>
#include <ctime>

void primes_print(Primes primes)
{
	if (primes.count == 0)
	{
		// do nothing
	}
	else if (primes.count == 1)
	{
		fputs("2", stdout);
	}
	else
	{
		fputs("2, 3", stdout);
		PrimesIter it = primes_iter_create(primes);
		while (primes_iter_has_next(&it))
			printf(", %" PRId64, primes_iter_next(&it));
	}
}

bool str_is_number(char* str)
{
	if (!str)
		return false;
	for (size_t i = 0; str[i]; i++)
		if (!isdigit(str[i]))
			return false;
	return true;
}

int main(int argc, char* argv[]) {
	Number max = 0;
	bool print = false;
	char* max_str = NULL;

	if (argc != 2 && argc != 3)
	{
		fprintf(stderr, "USAGE: %s [-p] maximum\n", argv[0]);
		exit(-1);
	}

	if (!strcmp(argv[1], "-p") || (argc == 3 && !strcmp(argv[1], "-p")))
	{
		print = true;
	}

	if (argc == 3 && !print)
	{
		fprintf(stderr, "USAGE: %s [-p] maximum", argv[0]);
		exit(-1);
	}
	
	if (str_is_number(argv[1]))
	{
		max = atoll(argv[1]);
	}

	if (str_is_number(argv[2]))
	{
		max = atoll(argv[2]);
	}

	clock_t start = clock();
	Primes primes = generatePrimes3_4(max);
	if (print)
	{
		primes_print(primes);
		putchar('\n');
	}
	clock_t end = clock();
	double cpu_time_used = ((double)(end - start)) / (double)(CLOCKS_PER_SEC);
	printf("%lf secs, %" PRId64 " primes", cpu_time_used, primes.count);
	return 0;
}
