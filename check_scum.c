#include "scum.h"
#include <check.h>

START_TEST (test_make_fixnum)
{
    object *o = make_fixnum (8);
    ck_assert_int_eq (o->data.fixnum.value, 8);
}
END_TEST

START_TEST (test_make_character)
{
    object *o = make_character ('e');
    ck_assert ( o->data.character.value == 'e');
}
END_TEST

START_TEST (test_make_symbol)
{
    object *o = make_symbol ("test");
    ck_assert (lookup ("test") != NULL);
    ck_assert_str_eq (o->data.string.value, "test");
}
END_TEST

START_TEST (test_pair_ops)
{
    make_singletons();
    object *o1 = cons (make_string ("testing"), make_boolean (true));
    object *o2 = cons (make_character ('a'), make_fixnum (5));
    object *o3 = cons (o1, o2);
    ck_assert (o3->type == PAIR);
    ck_assert (car(o3)->type == PAIR);
    ck_assert_str_eq (caar(o3)->data.string.value, "testing");
    ck_assert ((cdar(o3))->type == BOOLEAN);
}
END_TEST

START_TEST (test_delim)
{
    ck_assert (is_delimiter (')') == true);
    ck_assert (is_delimiter ('f') == false);
}
END_TEST

START_TEST (test_char_read)
{
    FILE *f = fopen ("test_files/test_char_read.scm", "r");
    if (f == NULL)
        ck_abort_msg ("file reading didn't work\n");
    object *o = read (f);
    ck_assert (o->data.character.value == 'a');
    o = read (f);
    ck_assert (o->data.character.value == '\n');
    o = read (f);
    o = read (f);
    ck_assert (o->data.character.value == ' ');
}
END_TEST

START_TEST (test_read_string)
{
    FILE *f = fopen ("test_files/test_string_read.scm", "r");
    if (f == NULL)
        ck_abort_msg ("file reading didn't work\n");
    object *o = read (f);
    ck_assert_str_eq (o->data.string.value, "abc");
}
END_TEST

START_TEST (test_lambda)
{
    FILE *f = fopen ("test_files/test_lambda.scm", "r");
    if (f == NULL)
        ck_abort_msg ("file reading didn't work\n");
    interpret (f, true);
}
END_TEST

START_TEST (test_nested_lambda)
{
    FILE *f = fopen ("test_files/test_nested_lambda.scm", "r");
    if (f == NULL)
        ck_abort_msg ("file reading didn't work\n");
    interpret (f, true);

}
END_TEST

START_TEST (test_lambda_recursion)
{
    FILE *f = fopen ("test_files/test_lambda_recursion.scm", "r");
    if (f == NULL)
        ck_abort_msg ("file reading didn't work\n");
    interpret (f, true);
}
END_TEST

START_TEST (test_and)
{
    FILE *f = fopen ("test_files/test_and.scm", "r");
    if (f == NULL)
        ck_abort_msg ("file reading didn't work\n");
    interpret (f, true);
}
END_TEST

START_TEST (test_or)
{
    FILE *f = fopen ("test_files/test_or.scm", "r");
    if (f == NULL)
        ck_abort_msg ("file reading didn't work\n");
    interpret (f, true);
}
END_TEST

START_TEST (test_apply)
{
    FILE *f = fopen ("test_files/test_apply.scm", "r");
    if (f == NULL)
        ck_abort_msg ("file reading didn't work\n");
    interpret (f, true);
}
END_TEST

Suite *
scum_suite (void)
{
    Suite *s = suite_create ("Scum");
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_make_fixnum);
    tcase_add_test (tc_core, test_make_character);
    tcase_add_test (tc_core, test_make_symbol);
    tcase_add_test (tc_core, test_pair_ops);
    tcase_add_test (tc_core, test_delim);
    tcase_add_test (tc_core, test_char_read);
    tcase_add_test (tc_core, test_read_string);
    tcase_add_test (tc_core, test_lambda);
    tcase_add_test (tc_core, test_nested_lambda);
    tcase_add_test (tc_core, test_lambda_recursion);
    tcase_add_test (tc_core, test_and);
    tcase_add_test (tc_core, test_or);
    tcase_add_test (tc_core, test_apply);
    suite_add_tcase (s, tc_core);

    return s;
}

int main ()
{
    int number_failed;
    Suite *s = scum_suite ();
    SRunner *sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
