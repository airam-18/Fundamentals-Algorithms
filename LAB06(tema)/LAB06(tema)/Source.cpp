#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Profiler.h"

typedef struct {
	int id;
	char name[30];
}Entry;

typedef struct {
	Entry* table;
	int* status;
	int size;
	int c1, c2;

}HashTable;

HashTable* createHashTable(int size, int c1, int c2)
{
	HashTable* H = (HashTable*)malloc(sizeof(HashTable));
	H->size = size;
	H->c1 = c1;
	H->c2 = c2;
	H->table = (Entry*)malloc(H->size * sizeof(Entry));
	H->status = (int*)malloc(H->size * sizeof(int));

	for (int i = 0; i < H->size; i++)
	{
		H->status[i] = 0;
	}

	return H;
}

int h_function(HashTable* h, int i, int key)
{
	return ((key) % h->size + h->c1 * i + h->c2 * i * i) % h->size;
}

int insert(HashTable* H, int id, const char* name)
{
	//average O(1/(1-l))
	int i = 0;
	int index;
	while (i < H->size)
	{
		index = h_function(H, i, id);

		if (H->status[index] == 0 || H->status[index] == -1)
		{
			H->table[index].id = id;
			H->status[index] = 1;
			strcpy_s(H->table[index].name,sizeof(H->table[index].name),name);
			return 1;	//we had space to insert
		}
		i++;
	}
	return 0;	//table is full	
}

int search(HashTable* H, int id, int *opcount)
{
	int i = 0;
	int index;

	while (i < H->size)
	{
		index = h_function(H, i, id);
		(*opcount)++;

		if (H->status[index] == 0)
		{
			//printf("id= %d negasit\n", id);
			return 0;
		}
		else
			if (H->status[index] == 1 && H->table[index].id == id)
			{
				//printf("id=%d -> name=%s gasit\n", id, H->table[index].name);
				return 1;
			}
		i++;
	}
	printf("id negasit\n");
	return 0;
}

int delete_key(HashTable *H,int id)
{
	int i = 0;
	int index;

	while (i < H->size)
	{
		index = h_function(H, i, id);
		if (H->status[index] == 0)		//key not in the table
			return 0;

		if (H->status[index] == 1 && H->table[index].id == id)
		{
			H->status[index] = -1;
			return 1;
		}
		i++;
	}
	return 0;
}

void demo()
{
	int opcount = 0;
	HashTable* H = createHashTable(10, 1, 1);
	insert(H, 2, "Student");
	insert(H, 7, "Mate");
	insert(H, 25, "AF");
	insert(H, 10, "BD");
	insert(H, 20, "CAN");
	insert(H, 40, "MSI");

	search(H, 10,&opcount);
	search(H, 7,&opcount);
	search(H, 4,&opcount);

	delete_key(H, 2);
	delete_key(H, 7);
	delete_key(H, 4);

	search(H, 7,&opcount);
}

void evaluare_search_caz_mediu(double l, int *total_f_op,int *total_nf_op,int *max_f_op,int *max_nf_op )
{
	const int N = 9973;

	for (int i = 0; i < 5; i++) {
		int n = 0;
		HashTable* H = createHashTable(N, 1, 1);
		int* inserted_keys = (int*)malloc(N*sizeof(int));
		while ((n * 1.0 / N) < l)
		{
			int key = rand() % (3*N);
			if (insert(H, key, ""))
			{
				inserted_keys[n] = key;
				n++;
			}
		}
		//printf("done");

		int m = 3000;
		int f_count = 0;
		int nf_count = 0;
		int op;

		while (f_count + nf_count < m)
		{
			
			if (f_count < m / 2)
			{
				op = 0;
				if (search(H, inserted_keys[rand() % n], &op))
				{
					f_count++;
					*total_f_op += op;
					if (op > *max_f_op)
						*max_f_op = op;
				}
			}

			else {
				op = 0;
				if (!search(H,  rand() % (3*N), &op))
				{
					nf_count++;
					*total_nf_op += op;
					if (op > *max_nf_op)
						*max_nf_op = op;
				}
			}
		}

	}
}

void evaluare_delete_tabele(double *l, int* total_f_op, int* total_nf_op, int* max_f_op, int* max_nf_op)
{
	const int N = 9973;


	int n = 0;
	HashTable* H = createHashTable(N, 1, 1);
	int* inserted_keys = (int*)malloc(N * sizeof(int));
	while ((n * 1.0 / N) < 0.99)
	{
		int key = rand() %(3*N);
		if (insert(H, key, ""))
		{
			inserted_keys[n] = key;
			n++;
		}
	}
	//printf("done");
	int p = n;

	while (*l > 0.80) {
		int idx = rand() % n;
		if (delete_key(H, inserted_keys[idx])) {
			inserted_keys[idx] = inserted_keys[n - 1];
			n--;
			*l = (n * 1.0) / N;
		}
		

	}

	int m = 3000;
	int f_count = 0;
	int nf_count = 0;
	int op;

	while (f_count + nf_count < m)
	{

		if (f_count < m / 2)
		{
			op = 0;
			if (search(H, inserted_keys[rand() % n], &op))
			{
				f_count++;
				*total_f_op += op;
				if (op > *max_f_op)
					*max_f_op = op;
			}
		}

		else {
			op = 0;
			if (!search(H, rand() % (3*N), &op))
			{
				nf_count++;
				*total_nf_op += op;
				if (op > *max_nf_op)
					*max_nf_op = op;
			}
		}

	}
}


void afisare_tabele()
{
		printf("_________________________________________________________________\n");
		printf("|Filling       Avg.       Max Effort        Avg.      Max Effort |\n");
		printf("|factor      Effort       (found)          Effort       (not-    |\n");
		printf("|            (found)                       (not-        found)   |\n");
		printf("|                                         found)                 |\n");
		printf("|________________________________________________________________|\n");
		for (double l = 0.80; l <= 0.95; l += 0.05) {
			int total_f_op = 0;
			int total_nf_op = 0;
			int max_f_op = 0;
			int max_nf_op = 0;
			evaluare_search_caz_mediu(l, &total_f_op, &total_nf_op, &max_f_op, &max_nf_op);

			printf("|%.02lf        %.02lf           %d             %.02lf          %d     |\n", l, total_f_op * 1.0 / 1500/5.0, max_f_op, total_nf_op * 1.0 / 1500/5.0, max_nf_op);

		}
		{
			double l = 0.99;
			int total_f_op = 0;
			int total_nf_op = 0;
			int max_f_op = 0;
			int max_nf_op = 0;

			evaluare_search_caz_mediu(l, &total_f_op, &total_nf_op, &max_f_op, &max_nf_op);

			printf("|%.02lf        %.02lf           %d            %.02lf         %d    |\n", l, total_f_op * 1.0 / 1500/5.0, max_f_op, total_nf_op * 1.0 / 1500/5.0, max_nf_op);
		}
		{
			double l = 0.99;
			int total_f_op = 0;
			int total_nf_op = 0;
			int max_f_op = 0;
			int max_nf_op = 0;
			evaluare_delete_tabele(&l, &total_f_op, &total_nf_op, &max_f_op, &max_nf_op);
			printf("|%.02lf        %.02lf           %d            %.02lf         %d    |\n", l, total_f_op * 1.0 / 1500, max_f_op, total_nf_op * 1.0 / 1500, max_nf_op);

		}
		printf("|________________________________________________________________|\n");



}

int main()
{
	//demo();
	
	afisare_tabele();
	return 0;
}