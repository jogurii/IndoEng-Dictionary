#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "data_structures.h"

// Manager kamus
typedef struct {
    BSTNode* bst_root;           // BST untuk Indonesia
    HashTable* hash_english;     // Hash Table untuk Inggris
    TrieNode* trie_root;         // Trie untuk Autocomplete
    LinkedList* search_history;  // Riwayat pencarian
    Stack* undo_stack;           // Operasi Undo stack
    Queue* word_queue;           // Queue WOTD

    int total_words;
    WordEntry last_searched;      // Terakhir dicari
    int has_last_searched;        // Flag terakhir dicari
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

// Init dan cleanup
DictionaryManager* dict_create(void);
void dict_destroy(DictionaryManager* dict);

// Operasi CRUD
int dict_add_word(DictionaryManager* dict, WordEntry word);
int dict_update_word(DictionaryManager* dict, const char* indonesian, WordEntry new_word);
int dict_delete_word(DictionaryManager* dict, const char* indonesian);
WordEntry* dict_search_indonesian(DictionaryManager* dict, const char* indonesian);
WordEntry* dict_search_english(DictionaryManager* dict, const char* english);
WordEntry* dict_search_prefix(DictionaryManager* dict, const char* prefix, int* count);

// Operasi View
void dict_view_all(DictionaryManager* dict);
void dict_display_history(DictionaryManager* dict);
void dict_word_of_day(DictionaryManager* dict);

// Riwayat pencarian
void dict_add_to_history(DictionaryManager* dict, WordEntry word);

// Terakhir dicari
void dict_set_last_searched(DictionaryManager* dict, WordEntry word);
WordEntry* dict_get_last_searched(DictionaryManager* dict);

// Operasi Undo
int dict_undo(DictionaryManager* dict);
int dict_delete_word_no_undo(DictionaryManager* dict, const char* indonesian);

// Statistik
void dict_statistics(DictionaryManager* dict);

// Pemuatan data
void dict_load_initial_data(DictionaryManager* dict);
void dict_save_to_file(DictionaryManager* dict, const char* filename);

#endif