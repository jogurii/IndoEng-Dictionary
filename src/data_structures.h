#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Warna untuk Terminal UI (ANSI Escape Codes)
#define CLR_RED     "\x1b[1;31m"
#define CLR_GREEN   "\x1b[1;32m"
#define CLR_YELLOW  "\x1b[1;33m"
#define CLR_BLUE    "\x1b[1;34m"
#define CLR_MAGENTA "\x1b[1;35m"
#define CLR_CYAN    "\x1b[1;36m"
#define CLR_WHITE   "\x1b[1;37m"
#define CLR_RESET   "\x1b[0m"
#define CLR_BOLD    "\x1b[1m"

// Case-insensitive string comparison mapping for Windows/POSIX strict C99 compliance
int strcasecmp(const char *s1, const char *s2);

// Konstanta ukuran
#define MAX_WORD_LEN 100
#define MAX_DEF_LEN 500
#define MAX_EXAMPLE_LEN 300
#define MAX_CATEGORY_LEN 50
#define MAX_MEANINGS 5

// Enum Part of Speech
typedef enum {
    NOUN,
    VERB,
    ADJECTIVE,
    ADVERB,
    PRONOUN,
    PREPOSITION,
    CONJUNCTION,
    INTERJECTION,
    DETERMINER
} PartOfSpeech;

// Enum Kategori Kata
typedef enum {
    EVERYDAY,
    FORMAL,
    SLANG,
    TECHNICAL
} WordCategory;

// Struct Meaning
typedef struct {
    char text[MAX_DEF_LEN];
} Meaning;

// Struct WordEntry
typedef struct {
    char indonesian[MAX_WORD_LEN];
    char english[MAX_WORD_LEN];
    PartOfSpeech pos;
    Meaning meanings[MAX_MEANINGS];
    int meaning_count;
    char example[MAX_EXAMPLE_LEN];
    char pronunciation[MAX_WORD_LEN];
    WordCategory category;
    time_t timestamp;
} WordEntry;

// ============================================================================
// BST - Penyimpanan kata secara alfabetis
// ============================================================================

typedef struct BSTNode {
    WordEntry word;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

BSTNode* bst_create_node(WordEntry word);
BSTNode* bst_insert(BSTNode* root, WordEntry word);
BSTNode* bst_search(BSTNode* root, const char* indonesian);
BSTNode* bst_delete(BSTNode* root, const char* indonesian);
void bst_update_key(BSTNode* root, const char* old_key, const char* new_key);
void bst_inorder(BSTNode* root, void (*callback)(WordEntry));
void bst_destroy(BSTNode* root);
int bst_count(BSTNode* root);

// ============================================================================
// HASH TABLE - Pencarian O(1) berdasarkan kata Inggris
// ============================================================================

#define HASH_TABLE_SIZE 1000

typedef struct HashNode {
    WordEntry word;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* buckets[HASH_TABLE_SIZE];
    int size;
} HashTable;

unsigned int hash_function(const char* str);
HashTable* hash_create(void);
void hash_insert(HashTable* ht, WordEntry word);
WordEntry* hash_search(HashTable* ht, const char* english);
WordEntry* hash_delete(HashTable* ht, const char* english);
void hash_remove(HashTable* ht, const char* english);
void hash_destroy(HashTable* ht);
int hash_size(HashTable* ht);

// ============================================================================
// TRIE - Autocomplete dan pencarian prefix
// ============================================================================

#define ALPHABET_SIZE 26

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    int is_end_of_word;
    WordEntry word;
    int word_count;
} TrieNode;

TrieNode* trie_create_node(void);
void trie_insert(TrieNode* root, const char* word, WordEntry entry);
void trie_update(TrieNode* root, const char* old_word, const char* new_word, WordEntry entry);
TrieNode* trie_search_prefix(TrieNode* root, const char* prefix);
void trie_get_all_with_prefix(TrieNode* root, const char* prefix, WordEntry** results, int* count);
void trie_delete_word(TrieNode* root, const char* word);
void trie_destroy(TrieNode* root);

// ============================================================================
// LINKED LIST - History pencarian dan undo
// ============================================================================

typedef struct ListNode {
    WordEntry word;
    struct ListNode* next;
    struct ListNode* prev;
} ListNode;

typedef struct {
    ListNode* head;
    ListNode* tail;
    int size;
    int max_size;
} LinkedList;

LinkedList* list_create(int max_size);
void list_insert_front(LinkedList* list, WordEntry word);
void list_insert_back(LinkedList* list, WordEntry word);
WordEntry* list_search(LinkedList* list, const char* indonesian);
void list_remove(LinkedList* list, const char* indonesian);
void list_destroy(LinkedList* list);
void list_display(LinkedList* list);

// ============================================================================
// STACK - Operasi undo dan navigasi history
// ============================================================================

#define MAX_STACK_SIZE 100

typedef struct {
    WordEntry entries[MAX_STACK_SIZE];
    int top;
    int max_size;
} Stack;

Stack* stack_create(int max_size);
int stack_push(Stack* s, WordEntry entry);
int stack_pop(Stack* s, WordEntry* entry);
int stack_peek(Stack* s, WordEntry* entry);
int stack_is_empty(Stack* s);
int stack_is_full(Stack* s);
void stack_destroy(Stack* s);

// ============================================================================
// QUEUE - Word of the day
// ============================================================================

#define MAX_QUEUE_SIZE 1000

typedef struct {
    WordEntry entries[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
    int max_size;
} Queue;

Queue* queue_create(int max_size);
int queue_enqueue(Queue* q, WordEntry entry);
int queue_dequeue(Queue* q, WordEntry* entry);
int queue_peek(Queue* q, WordEntry* entry);
int queue_is_empty(Queue* q);
int queue_is_full(Queue* q);
void queue_destroy(Queue* q);
WordEntry* queue_get_random(Queue* q);
void queue_shuffle(Queue* q);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

const char* pos_to_string(PartOfSpeech pos);
const char* category_to_string(WordCategory cat);
void print_word_entry(WordEntry* word);
WordEntry create_word_entry(
    const char* indonesian,
    const char* english,
    PartOfSpeech pos,
    const char* definition,
    const char* example,
    const char* pronunciation,
    WordCategory category
);

#endif