#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "data_structures.h"

// Dictionary Manager
typedef struct {
    BSTNode* bst_root;           // BST untuk lookup Indonesia
    HashTable* hash_english;     // Hash table untuk lookup Inggris
    TrieNode* trie_root;         // Trie untuk autocomplete
    LinkedList* search_history;  // History pencarian
    Stack* undo_stack;           // Stack untuk undo operations
    Queue* word_queue;           // Queue untuk word of the day

    int total_words;
    WordEntry last_searched;      // Kata terakhir yang dicari
    int has_last_searched;        // Flag apakah ada kata terakhir
} DictionaryManager;

typedef enum {
    VIEW_ALL = 1,
    SEARCH_INDONESIAN,
    SEARCH_ENGLISH,
    SEARCH_PREFIX,
    ADD_WORD,
    UPDATE_WORD,
    DELETE_WORD,
    WORD_OF_DAY,
    VIEW_HISTORY,
    UNDO,
    STATISTICS,
    EXIT_PROGRAM
} MenuOption;

// Inisialisasi dan cleanup
DictionaryManager* dict_create(void);
void dict_destroy(DictionaryManager* dict);

// CRUD
int dict_add_word(DictionaryManager* dict, WordEntry word);
int dict_update_word(DictionaryManager* dict, const char* indonesian, WordEntry new_word);
int dict_delete_word(DictionaryManager* dict, const char* indonesian);
WordEntry* dict_search_indonesian(DictionaryManager* dict, const char* indonesian);
WordEntry* dict_search_english(DictionaryManager* dict, const char* english);
WordEntry* dict_search_prefix(DictionaryManager* dict, const char* prefix, int* count);

// View
void dict_view_all(DictionaryManager* dict);
void dict_display_history(DictionaryManager* dict);
void dict_word_of_day(DictionaryManager* dict);

// Search history
void dict_add_to_history(DictionaryManager* dict, WordEntry word);

// Last searched
void dict_set_last_searched(DictionaryManager* dict, WordEntry word);
WordEntry* dict_get_last_searched(DictionaryManager* dict);

// Undo
int dict_undo(DictionaryManager* dict);
int dict_delete_word_no_undo(DictionaryManager* dict, const char* indonesian);

// Statistik
void dict_statistics(DictionaryManager* dict);

// Load data
void dict_load_initial_data(DictionaryManager* dict);
void dict_save_to_file(DictionaryManager* dict, const char* filename);

#endif