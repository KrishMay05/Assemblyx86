/*
 * CS 250 Project 5 Benchmark * Written by Kian Kasad * 
 */

/*
 * Usage information
 * -----------------
 *
 * There are several settings below which can be adjusted to customize the size
 * of the tree and number of lookups performed. Printing a failure message when
 * the student's solution is wrong can also be enabled/disabled.
 *
 * Compile with:
 * $ gcc -o tsearch_bench_asm6 tsearch_bench_better.c tsearch.c AVLTree.c
 * tsearch_asm5.s
 *
 * Run as:
 * ./tsearch_bench_asm6 <seed>
 *
 * The first (and only) argument is an integer which will be used as the random
 * seed. This allows for fair grading because all students can be given the same
 * random seed(s).
 *
 * If omitted, the seed defaults to 0. If the argument is provided but invalid,
 * an error message is printed and the program aborts.
 *
 * Internal details:
 * This program picks a random number of nodes between MIN_NODES and MAX_NODES
 * to generate. It then picks a random number of lookups to perform, between the
 * number of nodes and MAX_LOOKUPS. The actual number of lookups performed is
 * two times the random number chosen, because n_lookups lookups are performed
 * for each of the two checks:
 *   1. Keys which exist in the tree.
 *   2. Keys which do not exist in the tree.
 *
 * To generate the tree, the keys are randomly generated with lengths
 * between 1 and 31 (inclusive). Half are generated with a static prefix of
 * length STATIC_PREFIX_LEN. This is designed to ensure that full 32-byte string
 * comparison is required. The remaining bytes in the string are set to 0. One
 * random character is reserved and will not appear in any of the keys in the
 * tree. This lets us easily generate keys which do not appear in the tree, as
 * we can just insert the reserved character. All keys are guaranteed to be
 * unique. Key generation repeats until a unique key is generated. This process
 * is possibly slow but necessary. A built-in implementation of tsearch_find()
 * is used to check for duplicates.
 *
 * For each key, a random value (of type long) is generated. These are not
 * guaranteed to be unique.
 *
 * When performing the checks, the keys are searched in a random order.
 * This is not strictly necessary since the node values are randomized anyways,
 * but it helps avoid deterministic test cases.
 */

/* BEGIN BENCHMARK SETTINGS */

/* Minimum/maximum number of nodes in the tree. */
#define MIN_NODES         5000     /* 5 thousand */
#define MAX_NODES         1000000  /* 1 million */

/* Maximum number of lookups to perform for each check. Since there are three
 * checks, the actual max. no. of lookups will be this number times three. */
#define MAX_LOOKUPS       10000000 /* 10 million */

/* Print where and why a check fails. 1 = on, 0 = off. */
#define PRINT_FAILS       1

/* Fraction of the keys to generate with the static prefix. 50% is a good
 * number. */
#define STATIC_PREFIX_PCT 0.50f

/* Length of the static prefix to use for generation of half of the keys.
 * Setting this to 24 ensures that an adequate number of lookups require
 * comparison of all 32 bytes. Must be in [0, 30]. */
#define STATIC_PREFIX_LEN 24

/* Print the values of some variables such as the number of nodes and number of
 * lookups. Useful when picking random seeds to use. 1 = on, 0 = off. */
#define PRINT_INFO        0

/* Easily skip certain checks. 1 = skip, 0 = don't skip. */
#define SKIP_IN_TREE_CHECK     0
#define SKIP_OUT_OF_TREE_CHECK 0
#define SKIP_NULL_ROOT_CHECK   0

/* END BENCHMARK SETTINGS */

/* Enable CPU clock support.
 * Probably not required but the man page says to do it. */
#define _POSIX_CPUTIME

#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "tsearch.h"

#if PRINT_FAILS
#define fail(...) printf(__VA_ARGS__)
#else
#define fail(...) do {} while (0)
#endif

/* Width to pad nanosecond value to in order to work
 * as fractional seconds value. */
#define NS_PAD_WIDTH "9"
#define NS_IN_SECOND 1000000000

static sigjmp_buf segv_handler_jmpbuf;

/*
 * Returns a random integer in the range [min, max].
 * Behavior is undefined if min > max.
 */
static inline
int
rand_range(int min, int max)
{
    return min + random() % (max - min + 1);
}

/*
 * Returns a random ASCII uppercase letter
 * which is not equal to the given character.
 */
static inline
char
rand_char_without(char to_exclude)
{
    char c;
    do {
        c = (char) rand_range((int) 'A', (int) 'Z');
    } while (c == to_exclude);
    return c;
}

static
DATATYPE
bench_tsearch_find(TreeSearch *tsearch, KEYTYPE key)
{
    AVLNode *node = tsearch->root;
    while (node) {
        long cmp = avl_compareKeys(key, node->key);
        if (cmp == 0) {
            break;
        }
        node = (cmp < 0) ? node->left : node->right;
    }
    return node;
}

/*
 * Generate a "good" key, i.e. one which will be used
 * for a node in the tree. Places the key in the string
 * pointed to by the first argument. Generates keys which
 * exclude the character given by the second argument.
 */
static inline
void
gen_good_key(char *keystr, char exclude_char)
{
    long len = rand_range(1, MAXKEY - 1);
    for (int j = 0; j < len; j++) {
        keystr[j] = (char) rand_char_without(exclude_char);
    }
    for (int j = len; j < MAXKEY; j++) {
        keystr[j] = '\0';
    }
}

/*
 * Generate a "good" key, i.e. one which will be used
 * for a node in the tree, with a given prefix. Places
 * the key in the string pointed to by the first argument.
 * Generates keys which exclude the character given by
 * the second argument.
 */
static inline
void
gen_good_key_with_prefix(char *keystr, char exclude_char,
                         const char *prefix, long prefix_len)
{
    strncpy(keystr, prefix, MAXKEY);
    long len = rand_range(prefix_len + 1, MAXKEY - 1);
    for (int j = prefix_len; j < len; j++) {
        keystr[j] = (char) rand_char_without(exclude_char);
    }
    for (int j = len; j < MAXKEY; j++) {
        keystr[j] = '\0';
    }
}

/*
 * Generate a missing key, i.e. one which will be used
 * for a node in the tree. Places the key in the string
 * pointed to by the first argument. Generates keys which
 * are guaranteed to contain the character given by the
 * second argument at least once.
 */
static inline
void
gen_missing_key(char *keystr, char missing_char)
{
    long len = rand_range(1, MAXKEY - 1);
    for (int j = 0; j < len; j++) {
        keystr[j] = (char) rand_range((int) 'A', (int) 'Z');
    }
    keystr[rand_range(0, len - 1)] = missing_char;
    for (int j = len; j < MAXKEY; j++) {
        keystr[j] = '\0';
    }
}

/*
 * Generate a missing key, i.e. one which will be used
 * for a node in the tree. Places the key in the string
 * pointed to by the first argument. Generates keys which
 * are guaranteed to contain the character given by the
 * second argument at least once.
 */
static inline
void
gen_missing_key_with_prefix(char *keystr, char missing_char,
                            const char *prefix, long prefix_len)
{
    strncpy(keystr, prefix, MAXKEY);
    long len = rand_range(prefix_len + 1, MAXKEY - 1);
    for (int j = prefix_len; j < len; j++) {
        keystr[j] = (char) rand_range((int) 'A', (int) 'Z');
    }
    keystr[rand_range(prefix_len, len - 1)] = missing_char;
    for (int j = len; j < MAXKEY; j++) {
        keystr[j] = '\0';
    }
}

/*
 * Handles SIGSEGV by jumping to the registered handler location.
 */
void
segv_handler(int signal)
{
    siglongjmp(segv_handler_jmpbuf, 1);
}


extern void start_histogram();
extern void print_histogram();
/*
 * Main program function. Runs the benchmark.
 */
__attribute__ (( visibility("default") ))
int
main(int argc, char **argv)
{
    start_histogram();
    /* Use first argument as random seed. */
    if (argc >= 2) {
        int seed;
        if (sscanf(argv[1], "%d", &seed) != 1) {
            fprintf(stderr, "Error: argument must be an integer.\n");
            return -1;
        }
        srandom(seed);
    }

    TreeSearch tsearch;
    tsearch_init(&tsearch);

    /* Randomize number of nodes and lookups within bounds */
    const unsigned int n_nodes = rand_range(MIN_NODES, MAX_NODES);
    const unsigned int n_lookups = rand_range(n_nodes, MAX_LOOKUPS);
#if PRINT_INFO
    printf("Number of nodes: %'u\n", n_nodes);
    printf("Number of lookups (per check): %'u\n", n_lookups);
#endif

    /* Arrays to hold the keys and values of nodes in the tree.
     * A node with key keys[i] has value values[i]. */
    char **keys = malloc(n_nodes * sizeof(*keys));
    long *values = malloc(n_nodes * sizeof(*values));

    /* Pick a character to exclude from the keys. */
    char exclude_char = (char) rand_range((int) 'A', (int) 'Z');

    /* Generate a random static prefix to use for half the keys. */
    char prefix[STATIC_PREFIX_LEN + 1];
    for (long i = 0; i < STATIC_PREFIX_LEN; i++) {
        prefix[i] = rand_char_without(exclude_char);
    }
    prefix[STATIC_PREFIX_LEN] = '\0';

    /* Generate the tree. Half the keys are completely random,
     * and half have the known prefix generated above. */
    for (long i = 0; i < n_nodes; i++) {
        /* Generate key. */
        keys[i] = malloc(MAXKEY);
        if (i > (STATIC_PREFIX_PCT * n_nodes)) {
            gen_good_key(keys[i], exclude_char);
        } else {
            gen_good_key_with_prefix(keys[i], exclude_char, prefix, STATIC_PREFIX_LEN);
        }

        /* Generate value */
        values[i] = 1 + random();

        /* Check if key is duplicate. If so, try again. */
        if (bench_tsearch_find(&tsearch, keys[i]) != NULL) {
            /* Decrement i to redo this iteration. */
            i--;
            continue;
        }

        /* Insert into tree */
        tsearch_insert(&tsearch, keys[i], (DATATYPE) values[i]);
    }

    /* Generate a list of the indices of nodes to look up which covers all nodes
     * but in a random order. */
    unsigned int *lookup_indices = malloc(n_lookups * sizeof(*lookup_indices));
    for (long i = 0; i < n_lookups; i++) {
        lookup_indices[i] = i % n_nodes;
    }
    /* Shuffle array of indices. */
    for (long i = 0; i < n_lookups - 1; i++) {
        int swap_i = rand_range(i + 1, n_lookups - 1);
        unsigned int tmp = lookup_indices[i];
        lookup_indices[i] = lookup_indices[swap_i];
        lookup_indices[swap_i] = tmp;
    }

    /* Variables for use in checks.
     * Defined here so we can use them in SEGV handlers. */
    long i;
    char *key;
    long expected_value;

    /* Register SEGV handler */
    signal(SIGSEGV, segv_handler);

    /* Check 1: Perform in-tree lookups. */
#if ! SKIP_IN_TREE_CHECK
    if (sigsetjmp(segv_handler_jmpbuf, 1) != 0) {
        fail("Check 1 got segmentation fault at iteration %ld:"
             " lookup %s, expected %ld\n", i, key, expected_value);
        return 1;
    }
    struct timespec check1_start, check1_end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &check1_start);
    for (i = 0; i < n_lookups; i++) {
        key = keys[lookup_indices[i]];
        expected_value = values[lookup_indices[i]];
        long actual_value = (long) tsearch_find(&tsearch, key);
        if (expected_value != actual_value) {
            fail("Check 1 failed at iteration %ld: lookup %s, expected %ld, got %ld\n",
                   i, key, expected_value, actual_value);
            return 1;
        }
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &check1_end);
#endif

#if ! SKIP_OUT_OF_TREE_CHECK
    /* Generate out-of-tree keys here so they don't count towards the timer. */
    for (int i = 0; i < n_nodes; i++) {
        if (i > (STATIC_PREFIX_PCT * n_nodes)) {
            gen_missing_key(keys[i], exclude_char);
        } else {
            gen_missing_key_with_prefix(keys[i], exclude_char, prefix, STATIC_PREFIX_LEN);
        }
    }

    /* Check 2: Perform out-of-tree lookups. */
    if (sigsetjmp(segv_handler_jmpbuf, 1) != 0) {
        fail("Check 2 got segmentation fault at iteration %ld:"
             " lookup %s, expected NULL\n", i, key);
        return 1;
    }
    struct timespec check2_start, check2_end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &check2_start);
    for (i = 0; i < n_lookups; i++) {
        key = keys[lookup_indices[i]];
        long actual_value = (long) tsearch_find(&tsearch, key);
        if (actual_value != 0) {
            fail("Check 2 failed at iteration %ld: lookup %s, expected NULL, got %ld\n",
                   i, key, actual_value);
            return 2;
        }
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &check2_end);
#endif

    /* Check 3: Perform lookups on null tree. */
#if ! SKIP_NULL_ROOT_CHECK
    TreeSearch tsearch_null = {
        .root = NULL,
    };
    if (sigsetjmp(segv_handler_jmpbuf, 1) != 0) {
        fail("Check 3 got segmentation fault at iteration %ld:"
             " lookup %s, expected NULL\n", i, key);
        return 1;
    }
    struct timespec check3_start, check3_end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &check3_start);
    for (i = 0; i < n_lookups; i++) {
        char key[MAXKEY];
        gen_good_key(key, '\0');
        long actual_value = (long) tsearch_find(&tsearch_null, key);
        if (actual_value != 0) {
            fail("Check 3 failed at iteration %ld: lookup %s, expected NULL, got %ld\n",
                   i, key, actual_value);
            return 3;
        }
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &check3_end);
#endif

    /* Calculate elapsed time */
    long seconds_elapsed = 0;
#if ! SKIP_IN_TREE_CHECK
    seconds_elapsed += check1_end.tv_sec;
    seconds_elapsed -= check1_start.tv_sec;
#endif
#if ! SKIP_OUT_OF_TREE_CHECK
    seconds_elapsed += check2_end.tv_sec;
    seconds_elapsed -= check2_start.tv_sec;
#endif
#if ! SKIP_NULL_ROOT_CHECK
    seconds_elapsed += check3_end.tv_sec;
    seconds_elapsed -= check3_start.tv_sec;
#endif
    long ns_elapsed = 0;
#if ! SKIP_IN_TREE_CHECK
    ns_elapsed += check1_end.tv_nsec;
    ns_elapsed -= check1_start.tv_nsec;
#endif
#if ! SKIP_OUT_OF_TREE_CHECK
    ns_elapsed += check2_end.tv_nsec;
    ns_elapsed -= check2_start.tv_nsec;
#endif
#if ! SKIP_NULL_ROOT_CHECK
    ns_elapsed += check3_end.tv_nsec;
    ns_elapsed -= check3_start.tv_nsec;
#endif
    while (ns_elapsed < 0) {
        ns_elapsed += NS_IN_SECOND;
        seconds_elapsed -= 1;
    }
    seconds_elapsed += ns_elapsed / NS_IN_SECOND;
    ns_elapsed %= NS_IN_SECOND;

    printf("Total CPU time: %ld.%0" NS_PAD_WIDTH "ld seconds\n",
           seconds_elapsed, ns_elapsed);
    
    print_histogram();
    return 0;
}

/* vim: set ts=4 sw=4 et tw=80 : */
