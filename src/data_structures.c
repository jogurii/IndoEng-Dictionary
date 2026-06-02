#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include "data_structures.h"

// ============================================================================
// HELPER: Konversi Enum ke String
// ============================================================================

// Mengubah PartOfSpeech menjadi string
const char* pos_to_string(PartOfSpeech pos) {
    switch(pos) {
        case NOUN: return "Noun";
        case VERB: return "Verb";
        case ADJECTIVE: return "Adj.";
        case ADVERB: return "Adv.";
        case PRONOUN: return "Pronoun";
        case PREPOSITION: return "Prep.";
        case CONJUNCTION: return "Conj.";
        case INTERJECTION: return "Interj.";
        case DETERMINER: return "Determ.";
        default: return "Unknown";
    }
}

// Mengubah WordCategory menjadi string
const char* category_to_string(WordCategory cat) {
    switch(cat) {
        case EVERYDAY: return "Everyday";
        case FORMAL: return "Formal";
        case SLANG: return "Slang";
        case TECHNICAL: return "Technical";
        default: return "Unknown";
    }
}

// Menampilkan detail satu kata
void print_word_entry(WordEntry* word) {
    if (word == NULL) return;

    printf("\n");
    printf("  =============================================================\n");
    printf("  Indonesian : %-45s\n", word->indonesian);
    printf("  English    : %-45s\n", word->english);
    printf("  POS        : %-45s\n", pos_to_string(word->pos));
    printf("  Category   : %-45s\n", category_to_string(word->category));
    printf("  Pronounce  : %-45s\n", word->pronunciation);
    printf("  ------------------------------------------------------------\n");
    printf("  Definition(s):\n");
    for (int i = 0; i < word->meaning_count; i++) {
        printf("    %d. %s\n", i + 1, word->meanings[i].text);
    }
    printf("  ------------------------------------------------------------\n");
    printf("  Example    : %s\n", word->example);
    printf("  =============================================================\n");
}

// Membuat instance WordEntry baru
WordEntry create_word_entry(
    const char* indonesian,
    const char* english,
    PartOfSpeech pos,
    const char* definition,
    const char* example,
    const char* pronunciation,
    WordCategory category
) {
    WordEntry word;
    strncpy(word.indonesian, indonesian, MAX_WORD_LEN - 1);
    word.indonesian[MAX_WORD_LEN - 1] = '\0';

    strncpy(word.english, english, MAX_WORD_LEN - 1);
    word.english[MAX_WORD_LEN - 1] = '\0';

    word.pos = pos;

    word.meaning_count = 0;
    if (definition != NULL && strlen(definition) > 0) {
        strncpy(word.meanings[0].text, definition, MAX_DEF_LEN - 1);
        word.meanings[0].text[MAX_DEF_LEN - 1] = '\0';
        word.meaning_count = 1;
    }

    if (example != NULL) {
        strncpy(word.example, example, MAX_EXAMPLE_LEN - 1);
        word.example[MAX_EXAMPLE_LEN - 1] = '\0';
    } else {
        word.example[0] = '\0';
    }

    if (pronunciation != NULL) {
        strncpy(word.pronunciation, pronunciation, MAX_WORD_LEN - 1);
        word.pronunciation[MAX_WORD_LEN - 1] = '\0';
    } else {
        word.pronunciation[0] = '\0';
    }

    word.category = category;
    word.timestamp = time(NULL);

    return word;
}

// ============================================================================
// BINARY SEARCH TREE (BST)
// ============================================================================

// Membuat node BST baru
BSTNode* bst_create_node(WordEntry word) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Gagal alokasi memori untuk BST node\n");
        return NULL;
    }
    node->word = word;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Menyisipkan kata ke BST
BSTNode* bst_insert(BSTNode* root, WordEntry word) {
    if (root == NULL) {
        return bst_create_node(word);
    }

    int cmp = strcasecmp(word.indonesian, root->word.indonesian);

    if (cmp < 0) {
        root->left = bst_insert(root->left, word);
    } else if (cmp > 0) {
        root->right = bst_insert(root->right, word);
    } else {
        root->word = word;
    }

    return root;
}

// Mencari kata dalam BST
BSTNode* bst_search(BSTNode* root, const char* indonesian) {
    if (root == NULL) {
        return NULL;
    }

    int cmp = strcasecmp(indonesian, root->word.indonesian);

    if (cmp == 0) {
        return root;
    } else if (cmp < 0) {
        return bst_search(root->left, indonesian);
    } else {
        return bst_search(root->right, indonesian);
    }
}

// Mencari node dengan nilai terkecil
BSTNode* bst_find_min(BSTNode* node) {
    BSTNode* current = node;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

// Update key (indonesian) di BST
void bst_update_key(BSTNode* root, const char* old_key, const char* new_key) {
    if (root == NULL) return;

    int cmp = strcasecmp(old_key, root->word.indonesian);
    if (cmp == 0) {
        // Found the node, update the key
        strncpy(root->word.indonesian, new_key, MAX_WORD_LEN - 1);
        root->word.indonesian[MAX_WORD_LEN - 1] = '\0';
    } else if (cmp < 0) {
        bst_update_key(root->left, old_key, new_key);
    } else {
        bst_update_key(root->right, old_key, new_key);
    }
}

// Menghapus kata dari BST
BSTNode* bst_delete(BSTNode* root, const char* indonesian) {
    if (root == NULL) {
        return NULL;
    }

    int cmp = strcasecmp(indonesian, root->word.indonesian);

    if (cmp < 0) {
        root->left = bst_delete(root->left, indonesian);
    } else if (cmp > 0) {
        root->right = bst_delete(root->right, indonesian);
    } else {
        if (root->left == NULL) {
            BSTNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            BSTNode* temp = root->left;
            free(root);
            return temp;
        }

        BSTNode* temp = bst_find_min(root->right);
        root->word = temp->word;
        root->right = bst_delete(root->right, temp->word.indonesian);
    }

    return root;
}

// Traversing in-order BST
void bst_inorder(BSTNode* root, void (*callback)(WordEntry)) {
    if (root == NULL) return;

    bst_inorder(root->left, callback);
    callback(root->word);
    bst_inorder(root->right, callback);
}

// Membebaskan memori BST
void bst_destroy(BSTNode* root) {
    if (root == NULL) return;
    bst_destroy(root->left);
    bst_destroy(root->right);
    free(root);
}

// Menghitung jumlah node BST
int bst_count(BSTNode* root) {
    if (root == NULL) return 0;
    return 1 + bst_count(root->left) + bst_count(root->right);
}

// ============================================================================
// HASH TABLE
// ============================================================================

// Fungsi hash untuk string
unsigned int hash_function(const char* str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + (unsigned char)(tolower(*str));
        str++;
    }
    return hash % HASH_TABLE_SIZE;
}

// Membuat hash table baru
HashTable* hash_create() {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (ht == NULL) {
        fprintf(stderr, "Error: Gagal alokasi memori untuk hash table\n");
        return NULL;
    }

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        ht->buckets[i] = NULL;
    }
    ht->size = 0;
    return ht;
}

// Menyisipkan kata ke hash table
void hash_insert(HashTable* ht, WordEntry word) {
    unsigned int index = hash_function(word.english);

    HashNode* current = ht->buckets[index];
    while (current != NULL) {
        if (strcasecmp(current->word.english, word.english) == 0) {
            current->word = word;
            return;
        }
        current = current->next;
    }

    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    if (new_node == NULL) {
        fprintf(stderr, "Error: Gagal alokasi memori untuk hash node\n");
        return;
    }

    new_node->word = word;
    new_node->next = ht->buckets[index];
    ht->buckets[index] = new_node;
    ht->size++;
}

// Mencari kata dalam hash table
WordEntry* hash_search(HashTable* ht, const char* english) {
    unsigned int index = hash_function(english);

    HashNode* current = ht->buckets[index];
    while (current != NULL) {
        if (strcasecmp(current->word.english, english) == 0) {
            return &(current->word);
        }
        current = current->next;
    }

    return NULL;
}

// Menghapus kata dari hash table
WordEntry* hash_delete(HashTable* ht, const char* english) {
    unsigned int index = hash_function(english);

    HashNode* current = ht->buckets[index];
    HashNode* prev = NULL;

    while (current != NULL) {
        if (strcasecmp(current->word.english, english) == 0) {
            WordEntry* deleted_word = (WordEntry*)malloc(sizeof(WordEntry));
            if (deleted_word) {
                *deleted_word = current->word;
            }

            if (prev == NULL) {
                ht->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }

            free(current);
            ht->size--;
            return deleted_word;
        }
        prev = current;
        current = current->next;
    }

    return NULL;
}

// Menghapus entry dari hash table berdasarkan english (tanpa free WordEntry)
void hash_remove(HashTable* ht, const char* english) {
    if (ht == NULL || english == NULL) return;

    unsigned int index = hash_function(english) % HASH_TABLE_SIZE;
    HashNode* current = ht->buckets[index];
    HashNode* prev = NULL;

    while (current != NULL) {
        if (strcasecmp(current->word.english, english) == 0) {
            if (prev == NULL) {
                ht->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            ht->size--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Membebaskan memori hash table
void hash_destroy(HashTable* ht) {
    if (ht == NULL) return;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = ht->buckets[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(ht);
}

// Mendapatkan ukuran hash table
int hash_size(HashTable* ht) {
    return ht ? ht->size : 0;
}

// ============================================================================
// TRIE (PREFIX TREE)
// ============================================================================

// Membuat node Trie baru
TrieNode* trie_create_node(void) {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Gagal alokasi memori untuk trie node\n");
        return NULL;
    }

    node->is_end_of_word = 0;
    node->word_count = 0;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }

    return node;
}

// Menyisipkan kata ke Trie
void trie_insert(TrieNode* root, const char* word, WordEntry entry) {
    TrieNode* current = root;

    while (*word) {
        int index = tolower(*word) - 'a';

        if (index < 0 || index >= ALPHABET_SIZE) {
            word++;
            continue;
        }

        if (current->children[index] == NULL) {
            current->children[index] = trie_create_node();
        }

        current = current->children[index];
        current->word_count++;
        word++;
    }

    current->is_end_of_word = 1;
    current->word = entry;
}

// Mencari prefix dalam Trie
TrieNode* trie_search_prefix(TrieNode* root, const char* prefix) {
    TrieNode* current = root;

    while (*prefix) {
        int index = tolower(*prefix) - 'a';

        if (index < 0 || index >= ALPHABET_SIZE) {
            return NULL;
        }

        if (current->children[index] == NULL) {
            return NULL;
        }

        current = current->children[index];
        prefix++;
    }

    return current;
}

// Mengumpulkan semua kata dari node Trie
void trie_collect_all(TrieNode* node, WordEntry** results, int* count, int* capacity) {
    if (node == NULL) return;

    if (node->is_end_of_word) {
        if (*count >= *capacity) {
            *capacity *= 2;
            *results = (WordEntry*)realloc(*results, (*capacity) * sizeof(WordEntry));
        }
        (*results)[*count] = node->word;
        (*count)++;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        trie_collect_all(node->children[i], results, count, capacity);
    }
}

// Mendapatkan semua kata dengan prefix tertentu
void trie_get_all_with_prefix(TrieNode* root, const char* prefix, WordEntry** results, int* count) {
    TrieNode* prefix_node = trie_search_prefix(root, prefix);

    if (prefix_node == NULL) {
        *results = NULL;
        *count = 0;
        return;
    }

    int capacity = 10;
    *results = (WordEntry*)malloc(capacity * sizeof(WordEntry));
    *count = 0;

    trie_collect_all(prefix_node, results, count, &capacity);
}

// Membebaskan memori Trie
void trie_destroy(TrieNode* root) {
    if (root == NULL) return;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        trie_destroy(root->children[i]);
    }

    free(root);
}

// ============================================================================
// LINKED LIST
// ============================================================================

// Membuat linked list baru
LinkedList* list_create(int max_size) {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    if (list == NULL) {
        fprintf(stderr, "Error: Gagal alokasi memori untuk linked list\n");
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->max_size = max_size;

    return list;
}

// Menyisipkan kata di awal list
void list_insert_front(LinkedList* list, WordEntry word) {
    if (list == NULL) return;

    ListNode* current = list->head;
    while (current != NULL) {
        if (strcasecmp(current->word.indonesian, word.indonesian) == 0) {
            if (current != list->head) {
                if (current->prev != NULL) {
                    current->prev->next = current->next;
                }
                if (current->next != NULL) {
                    current->next->prev = current->prev;
                }
                if (current == list->tail) {
                    list->tail = current->prev;
                }

                current->prev = NULL;
                current->next = list->head;
                if (list->head != NULL) {
                    list->head->prev = current;
                }
                list->head = current;
            }
            return;
        }
        current = current->next;
    }

    if (list->size >= list->max_size && list->tail != NULL) {
        ListNode* temp = list->tail;
        list->tail = temp->prev;
        if (list->tail != NULL) {
            list->tail->next = NULL;
        } else {
            list->head = NULL;
        }
        free(temp);
        list->size--;
    }

    ListNode* new_node = (ListNode*)malloc(sizeof(ListNode));
    if (new_node == NULL) {
        fprintf(stderr, "Error: Gagal alokasi memori untuk list node\n");
        return;
    }

    new_node->word = word;
    new_node->next = list->head;
    new_node->prev = NULL;

    if (list->head != NULL) {
        list->head->prev = new_node;
    }

    list->head = new_node;

    if (list->tail == NULL) {
        list->tail = new_node;
    }

    list->size++;
}

// Menyisipkan kata di akhir list
void list_insert_back(LinkedList* list, WordEntry word) {
    if (list == NULL) return;

    if (list->size >= list->max_size && list->head != NULL) {
        ListNode* temp = list->head;
        list->head = temp->next;
        if (list->head != NULL) {
            list->head->prev = NULL;
        } else {
            list->tail = NULL;
        }
        free(temp);
        list->size--;
    }

    ListNode* new_node = (ListNode*)malloc(sizeof(ListNode));
    if (new_node == NULL) {
        fprintf(stderr, "Error: Gagal alokasi memori untuk list node\n");
        return;
    }

    new_node->word = word;
    new_node->next = NULL;
    new_node->prev = list->tail;

    if (list->tail != NULL) {
        list->tail->next = new_node;
    }

    list->tail = new_node;

    if (list->head == NULL) {
        list->head = new_node;
    }

    list->size++;
}

// Mencari kata dalam list
WordEntry* list_search(LinkedList* list, const char* indonesian) {
    if (list == NULL) return NULL;

    ListNode* current = list->head;
    while (current != NULL) {
        if (strcasecmp(current->word.indonesian, indonesian) == 0) {
            return &(current->word);
        }
        current = current->next;
    }

    return NULL;
}

// Menghapus kata dari list
void list_remove(LinkedList* list, const char* indonesian) {
    if (list == NULL) return;

    ListNode* current = list->head;
    while (current != NULL) {
        if (strcasecmp(current->word.indonesian, indonesian) == 0) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            } else {
                list->head = current->next;
            }

            if (current->next != NULL) {
                current->next->prev = current->prev;
            } else {
                list->tail = current->prev;
            }

            free(current);
            list->size--;
            return;
        }
        current = current->next;
    }
}

// Menampilkan isi list
void list_display(LinkedList* list) {
    if (list == NULL || list->size == 0) {
        printf("  [Kosong]\n");
        return;
    }

    ListNode* current = list->head;
    int num = 1;

    while (current != NULL) {
        printf("  %d. %s = %s\n", num++, current->word.indonesian, current->word.english);
        current = current->next;
    }
}

// Membebaskan memori list
void list_destroy(LinkedList* list) {
    if (list == NULL) return;

    ListNode* current = list->head;
    while (current != NULL) {
        ListNode* temp = current;
        current = current->next;
        free(temp);
    }

    free(list);
}

// ============================================================================
// STACK
// ============================================================================

// Membuat stack baru
Stack* stack_create(int max_size) {
    Stack* s = (Stack*)malloc(sizeof(Stack));
    if (s == NULL) {
        fprintf(stderr, "Error: Gagal alokasi memori untuk stack\n");
        return NULL;
    }

    s->top = -1;
    s->max_size = max_size > 0 ? max_size : MAX_STACK_SIZE;

    return s;
}

// Push elemen ke stack
int stack_push(Stack* s, WordEntry entry) {
    if (s == NULL || stack_is_full(s)) {
        return 0;
    }

    s->entries[++s->top] = entry;
    return 1;
}

// Pop elemen dari stack
int stack_pop(Stack* s, WordEntry* entry) {
    if (s == NULL || stack_is_empty(s)) {
        return 0;
    }

    *entry = s->entries[s->top--];
    return 1;
}

// Lihat elemen teratas stack
int stack_peek(Stack* s, WordEntry* entry) {
    if (s == NULL || stack_is_empty(s)) {
        return 0;
    }

    *entry = s->entries[s->top];
    return 1;
}

// Cek apakah stack kosong
int stack_is_empty(Stack* s) {
    return s == NULL || s->top == -1;
}

// Cek apakah stack penuh
int stack_is_full(Stack* s) {
    return s != NULL && s->top >= s->max_size - 1;
}

// Membebaskan memori stack
void stack_destroy(Stack* s) {
    if (s != NULL) {
        free(s);
    }
}

// ============================================================================
// QUEUE
// ============================================================================

// Membuat queue baru
Queue* queue_create(int max_size) {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
        fprintf(stderr, "Error: Gagal alokasi memori untuk queue\n");
        return NULL;
    }

    q->front = 0;
    q->rear = -1;
    q->size = 0;
    q->max_size = max_size > 0 ? max_size : MAX_QUEUE_SIZE;

    return q;
}

// Enqueue elemen ke queue
int queue_enqueue(Queue* q, WordEntry entry) {
    if (q == NULL || queue_is_full(q)) {
        return 0;
    }

    q->rear = (q->rear + 1) % q->max_size;
    q->entries[q->rear] = entry;
    q->size++;

    return 1;
}

// Dequeue elemen dari queue
int queue_dequeue(Queue* q, WordEntry* entry) {
    if (q == NULL || queue_is_empty(q)) {
        return 0;
    }

    *entry = q->entries[q->front];
    q->front = (q->front + 1) % q->max_size;
    q->size--;

    return 1;
}

// Lihat elemen terdepan queue
int queue_peek(Queue* q, WordEntry* entry) {
    if (q == NULL || queue_is_empty(q)) {
        return 0;
    }

    *entry = q->entries[q->front];
    return 1;
}

// Cek apakah queue kosong
int queue_is_empty(Queue* q) {
    return q == NULL || q->size == 0;
}

// Cek apakah queue penuh
int queue_is_full(Queue* q) {
    return q != NULL && q->size >= q->max_size;
}

// Membebaskan memori queue
void queue_destroy(Queue* q) {
    if (q != NULL) {
        free(q);
    }
}

// Mendapatkan kata acak dari queue
WordEntry* queue_get_random(Queue* q) {
    if (q == NULL || q->size == 0) {
        return NULL;
    }

    srand((unsigned int)time(NULL));
    int index = rand() % q->size;
    int actual_index = (q->front + index) % q->max_size;

    return &(q->entries[actual_index]);
}

// Mengacak elemen queue (Fisher-Yates shuffle)
void queue_shuffle(Queue* q) {
    if (q == NULL || q->size <= 1) return;

    srand((unsigned int)time(NULL));

    WordEntry temp;
    int n = q->size;

    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        int idx_i = (q->front + i) % q->max_size;
        int idx_j = (q->front + j) % q->max_size;

        temp = q->entries[idx_i];
        q->entries[idx_i] = q->entries[idx_j];
        q->entries[idx_j] = temp;
    }
}