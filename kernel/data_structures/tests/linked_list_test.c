#include "../../include/linked_list.h"
#include "../../include/klibc.h"

int compare(list *l, int *a)
{
    int result = 1;
    int i = 0;
    ll_node *t = l->head; // t for test
    while(t && a[i]) {
        result = result && (*(int *)t->data == a[i]);
        t = t->next;
        i++;
    }
    return result;
}

void print_test(int i, int r)
{
    os_printf("test %d: %s", i, r? "Passed" : "Failed");
}

void test0()
{
    int a[4] = {0, 1, 2, 3};
    int i;
    int r;
    list *l = create_list(0);
    for (i = 1; i < 4; i++) append(l, (void *)i);
    r = compare(l, a);
    print_test(0, r);
}

void test1()
{
    int i;
    int a[4] = {0, 2, 3};
    int r;
    list *l = create_list(0);
    for (i = 1; i < 4; i++) append(l, (void *)i);
    delete_at(l, 1);
    r = compare(l, a);
    print_test(0, r);
}

void test_linked_list()
{
    test0();
    /* test1(); */
    /* test2(); */
    /* test3(); */
    /* test4(); */
    /* test5(); */
}
