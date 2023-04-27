#include "os-mem.h"


// Максимальное количество ссылок из каждого объекта:
#define MAX_LINKS_AMOUNT 16


// Структура, представляющая объект:
typedef struct Object
{
    char name[33];                  // имя объекта

    struct Object **links;          // массив ссылок данного объекта на другие объекты
    int links_amount;               // количество ссылок данного объекта на другие объекты
} Object;


// Прочие переменные:
unsigned int mm_size;               // максимальное количество объектов
int created = 0;                    // флаг наличия менеджера памяти
Object **objects = NULL;            // массив указателей на все существующие объекты
int objects_amount = 0;             // количество всех существующих объектов


// Вспомогательные функции:

// Возвращает индекс элемента с заданным именем в массиве элементов
// типа Object или -1, если такого элемента в массиве не существует:
int bin_search(Object **arr, int arr_len, const char *name)
{
	int mid, left = 0, right = arr_len - 1;

	while (left <= right)
	{
		mid = (left + right) / 2;

		if (strcmp(name, arr[mid]->name) < 0)
			right = mid - 1;
		else if (strcmp(name, arr[mid]->name) > 0)
			left = mid + 1;
		else
			return mid;
	}

	return -1;
}

// Возвращает индекс, по которому нужно вставить новый элемент в
// массив элементов типа Object так, чтобы сохранить порядок сортировки
// массива или -1, если такой элемент в массиве уже существует:
int bin_insert(Object **arr, int arr_len, const char *name)
{
	int mid, left = 0, right = arr_len - 1;

	while (left < right)
	{
		mid = (left + right) / 2;

		if (strcmp(name, arr[mid]->name) < 0)
			if (left == right - 1) return left;
			else right = mid;
		else if (strcmp(name, arr[mid]->name) > 0)
			if (left == right - 1) return right;
			else left = mid;
		else
			return -1;
	}
}


// Основные функции:

// Создает менеджер памяти:
int create(int size, int num_pages)
{
	int err_code = 1;

	if (!created)
	{
		mm_size = size;
		created = 1;
	}
	else
		err_code = 0;

	return err_code;
}

// Создает объект:
int create_object(const char* name)
{
	int err_code = 1, req_pos;

	if (created && mm_size && (bin_search(objects, objects_amount, name) == -1))
	{
		if (objects_amount)
		{
			objects = (Object**)realloc(objects, sizeof(Object*) * (objects_amount + 1));

			if (objects != NULL)
			{
				objects_amount++;
				req_pos = bin_insert(objects, objects_amount, name);

				for (int i = objects_amount - 1; i > req_pos; i--)
					objects[i] = objects[i - 1];

				objects[req_pos] = (Object*)malloc(sizeof(Object));

				if (objects[req_pos] != NULL)
				{
					strcpy(objects[req_pos]->name, name);
					objects[req_pos]->links = NULL;
					objects[req_pos]->links_amount = 0;

					mm_size--;
				}
				else
					err_code = 0;
			}
			else
				err_code = 0;
		}
		else
		{
			objects = (Object**)malloc(sizeof(Object*));

			if (objects != NULL)
			{
				objects[0] = (Object*)malloc(sizeof(Object));

				if (objects[0] != NULL)
				{
					strcpy(objects[0]->name, name);
					objects[0]->links = NULL;
					objects[0]->links_amount = 0;

					objects_amount++;
					mm_size--;
				}
				else
					err_code = 0;
			}
			else
				err_code = 0;
		}
	}
	else
		err_code = 0;

	return err_code;
}

// Удаляет объект:
int destroy_object(const char* name)
{
	int err_code = 1, req_pos = bin_search(objects, objects_amount, name);

	if (created && (req_pos != -1))
	{
		mm_size++;

		for (int i = 0; i < objects_amount; i++)
			for (int j = 0; j < objects[i]->links_amount; j++)
				if ((objects[i]->links[j] != NULL) && !strcmp(objects[req_pos]->name, objects[i]->links[j]->name))
					objects[i]->links[j] = NULL;

		free(objects[req_pos]->links);
		free(objects[req_pos]);

		for (int i = req_pos; i < objects_amount - 1; i++)
			objects[i] = objects[i + 1];

		objects_amount--;

		if (!objects_amount)
		{
			free(objects);
			objects = NULL;
		}
		else
			objects = (Object**)realloc(objects, sizeof(Object*) * objects_amount);
	}
	else
		err_code = 0;

	return err_code;
}

// Удаляет менеджер памяти:
int destroy()
{
	int err_code = 1;

	if (created)
	{
		while (objects_amount)
			err_code = destroy_object(objects[0]->name);

		free(objects);
		objects = NULL;
		created = 0;
	}
	else
		err_code = 0;

	return err_code;
}

// Выводит имена всех созданных и не уничтоженных объектов:
void print_objects()
{
	for (int i = 0; i < objects_amount; i++)
		printf("%s\n", objects[i]->name);
}

// Создает ссылку из объекта object1 на объект object2:
int link(const char* object1_name, const char* object2_name)
{
	int err_code = 1, req_pos,
			req_pos1 = bin_search(objects, objects_amount, object1_name),
			req_pos2 = bin_search(objects, objects_amount, object2_name);

	if ((req_pos1 != - 1) && (req_pos2 != -1) && (objects[req_pos1]->links_amount < MAX_LINKS_AMOUNT))
	{
		if (objects[req_pos1]->links_amount)
		{
			objects[req_pos1]->links = (Object**)realloc(objects[req_pos1]->links, sizeof(Object*) * (objects[req_pos1]->links_amount + 1));

			if (objects[req_pos1]->links != NULL)
			{
				objects[req_pos1]->links_amount++;
				req_pos = bin_insert(objects[req_pos1]->links, objects[req_pos1]->links_amount, object2_name);

				for (int i = objects[req_pos1]->links_amount - 1; i > req_pos; i--)
					objects[req_pos1]->links[i] = objects[req_pos1]->links[i - 1];

				objects[req_pos1]->links[req_pos] = objects[req_pos2];
			}
			else
				err_code = 0;
		}
		else
		{
			objects[req_pos1]->links = (Object**)malloc(sizeof(Object*));

			if (objects[req_pos1]->links != NULL)
			{
				objects[req_pos1]->links[0] = objects[req_pos2];
				objects[req_pos1]->links_amount++;
			}
			else
				err_code = 0;
		}
	}
	else
		err_code = 0;

	return err_code;
}

// Для каждого объекта выводит количество ссылок на него:
void print_link_counts()
{
	for (int i = 0; i < objects_amount; i++)
	{
		int links_amount = 0;

		printf("%s ", objects[i]->name);

		for (int j = 0; j < objects_amount; j++)
			for (int k = 0; k < objects[j]->links_amount; k++)
				if ((objects[j]->links[k] != NULL) && !strcmp(objects[i]->name, objects[j]->links[k]->name))
					links_amount++;

		printf("%d\n", links_amount);
	}
}

// Настаивает менеджер памяти перед началом работы:
void setup_memory_manager(memory_manager_t *mm)
{
	mm->create = create;
	mm->destroy = destroy;
	mm->create_object = create_object;
	mm->destroy_object = destroy_object;
	mm->print_objects = print_objects;
	mm->link = link;
	mm->print_link_counts = print_link_counts;
}