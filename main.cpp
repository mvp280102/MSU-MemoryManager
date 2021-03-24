#include "os-mem.h"

#include <cstdio>

int main()
{
	memory_manager_t memory_manager;

	setup_memory_manager(&memory_manager);

	printf("Author test:\n\n");

	printf("create(16, 0) = %d\n\n", memory_manager.create(16, 0));

	printf("create_object(\"obj1\") = %d\n", memory_manager.create_object("obj1"));
	printf("create_object(\"obj1\") = %d\n", memory_manager.create_object("obj1"));
	printf("create_object(\"obj3\") = %d\n", memory_manager.create_object("obj3"));
	printf("create_object(\"obj2\") = %d\n", memory_manager.create_object("obj2"));
	printf("create_object(\"obj4\") = %d\n\n", memory_manager.create_object("obj4"));

	printf("print_objects():\n");
	memory_manager.print_objects();
	printf("\n");

	printf("link(\"obj4\", \"obj1\") = %d\n", memory_manager.link("obj4", "obj1"));
	printf("link(\"obj3\", \"obj2\") = %d\n", memory_manager.link("obj3", "obj2"));
	printf("link(\"obj4\", \"obj2\") = %d\n", memory_manager.link("obj4", "obj2"));
	printf("link(\"obj3\", \"obj4\") = %d\n", memory_manager.link("obj3", "obj4"));
	printf("link(\"obj2\", \"obj3\") = %d\n", memory_manager.link("obj2", "obj3"));
	printf("link(\"obj1\", \"obj2\") = %d\n\n", memory_manager.link("obj1", "obj2"));

	printf("print_link_counts():\n");
	memory_manager.print_link_counts();
	printf("\n");

	printf("destroy_object(\"obj2\") = %d\n", memory_manager.destroy_object("obj2"));
	printf("destroy_object(\"obj4\") = %d\n\n", memory_manager.destroy_object("obj4"));

	printf("print_link_counts():\n");
	memory_manager.print_link_counts();
	printf("\n");

	printf("destroy() = %d\n", memory_manager.destroy());

	return 0;
}