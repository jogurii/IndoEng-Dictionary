#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Mengumpulkan semua kata dari BST ke array
static void collect_all_words(BSTNode* node, WordEntry** results, int* count) {
    if (node == NULL) return;

    collect_all_words(node->left, results, count);
    (*results)[*count] = node->word;
    (*count)++;
    collect_all_words(node->right, results, count);
}

// Rebuild Trie dari BST (setelah update/delete)
static void rebuild_trie(TrieNode* trie_root, BSTNode* bst_root) {
    if (bst_root == NULL) return;
    trie_insert(trie_root, bst_root->word.indonesian, bst_root->word);
    rebuild_trie(trie_root, bst_root->left);
    rebuild_trie(trie_root, bst_root->right);
}

// Hitung statistik dari BST
static void count_bst_stats(BSTNode* n, int* pos_count, int* cat_count) {
    if (n == NULL) return;
    int pos_idx = (int)n->word.pos;
    int cat_idx = (int)n->word.category;
    if (pos_idx >= 0 && pos_idx < 10) pos_count[pos_idx]++;
    if (cat_idx >= 0 && cat_idx < 4) cat_count[cat_idx]++;
    count_bst_stats(n->left, pos_count, cat_count);
    count_bst_stats(n->right, pos_count, cat_count);
}

// Simpan satu BST node ke file
static void save_bst_node(FILE* file, BSTNode* n) {
    if (n == NULL) return;

    fprintf(file, "%s|%s|%d|%d|%s|%s|%s\n",
            n->word.indonesian,
            n->word.english,
            n->word.pos,
            n->word.category,
            n->word.meanings[0].text,
            n->word.example,
            n->word.pronunciation);

    save_bst_node(file, n->left);
    save_bst_node(file, n->right);
}

// ============================================================================
// INITIALIZATION AND CLEANUP
// ============================================================================

// Membuat instance DictionaryManager baru
DictionaryManager* dict_create(void) {
    DictionaryManager* dict = (DictionaryManager*)malloc(sizeof(DictionaryManager));

    if (dict == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for dictionary\n");
        return NULL;
    }

    dict->bst_root = NULL;
    dict->hash_english = hash_create();
    dict->trie_root = trie_create_node();
    dict->search_history = list_create(20);
    dict->undo_stack = stack_create(50);
    dict->word_queue = queue_create(1000);

    dict->total_words = 0;
    dict->word_of_day_index = 0;
    dict->has_last_searched = 0;
    memset(&dict->last_searched, 0, sizeof(WordEntry));

    return dict;
}

// Membebaskan memori DictionaryManager
void dict_destroy(DictionaryManager* dict) {
    if (dict == NULL) return;

    bst_destroy(dict->bst_root);
    hash_destroy(dict->hash_english);
    trie_destroy(dict->trie_root);
    list_destroy(dict->search_history);
    stack_destroy(dict->undo_stack);
    queue_destroy(dict->word_queue);

    free(dict);
}

// ============================================================================
// CRUD OPERATIONS
// ============================================================================

// Menambah kata baru ke dictionary
int dict_add_word(DictionaryManager* dict, WordEntry word) {
    if (dict == NULL) return 0;

    if (bst_search(dict->bst_root, word.indonesian) != NULL) {
        return 0;
    }

    dict->bst_root = bst_insert(dict->bst_root, word);
    hash_insert(dict->hash_english, word);
    trie_insert(dict->trie_root, word.indonesian, word);
    queue_enqueue(dict->word_queue, word);

    dict->total_words++;

    // Push undo action - store marker with @ADD: prefix
    WordEntry undo_marker = word;
    char marker_prefix[MAX_WORD_LEN];
    snprintf(marker_prefix, sizeof(marker_prefix), "@ADD:%s", word.indonesian);
    strncpy(undo_marker.indonesian, marker_prefix, MAX_WORD_LEN - 1);
    undo_marker.indonesian[MAX_WORD_LEN - 1] = '\0';
    stack_push(dict->undo_stack, undo_marker);

    return 1;
}

// Mengupdate kata yang sudah ada di dictionary
int dict_update_word(DictionaryManager* dict, const char* indonesian, WordEntry new_word) {
    if (dict == NULL || indonesian == NULL) return 0;

    BSTNode* node = bst_search(dict->bst_root, indonesian);
    if (node == NULL) {
        printf("  [Error] Kata '%s' tidak ditemukan!\n", indonesian);
        return 0;
    }

    WordEntry old_word = node->word;

    dict->bst_root = bst_delete(dict->bst_root, indonesian);
    dict->bst_root = bst_insert(dict->bst_root, new_word);

    hash_delete(dict->hash_english, old_word.english);
    hash_insert(dict->hash_english, new_word);

    trie_destroy(dict->trie_root);
    dict->trie_root = trie_create_node();
    rebuild_trie(dict->trie_root, dict->bst_root);

    // Push undo action - store marker with @UPDATE: prefix + old_word data
    WordEntry undo_marker = old_word;
    char marker_prefix[MAX_WORD_LEN];
    snprintf(marker_prefix, sizeof(marker_prefix), "@UPDATE:%s", old_word.indonesian);
    strncpy(undo_marker.indonesian, marker_prefix, MAX_WORD_LEN - 1);
    undo_marker.indonesian[MAX_WORD_LEN - 1] = '\0';
    stack_push(dict->undo_stack, undo_marker);

    printf("  [Sukses] Kata '%s' berhasil diupdate!\n", indonesian);
    return 1;
}

// Menghapus kata dari dictionary
int dict_delete_word(DictionaryManager* dict, const char* indonesian) {
    if (dict == NULL || indonesian == NULL) return 0;

    BSTNode* node = bst_search(dict->bst_root, indonesian);
    if (node == NULL) {
        printf("  [Error] Kata '%s' tidak ditemukan!\n", indonesian);
        return 0;
    }

    WordEntry deleted_word = node->word;

    dict->bst_root = bst_delete(dict->bst_root, indonesian);
    hash_delete(dict->hash_english, deleted_word.english);

    trie_destroy(dict->trie_root);
    dict->trie_root = trie_create_node();
    rebuild_trie(dict->trie_root, dict->bst_root);

    dict->total_words--;

    // Push undo action - store marker with @DELETE: prefix
    WordEntry undo_marker = deleted_word;
    char marker_prefix[MAX_WORD_LEN];
    snprintf(marker_prefix, sizeof(marker_prefix), "@DELETE:%s", deleted_word.indonesian);
    strncpy(undo_marker.indonesian, marker_prefix, MAX_WORD_LEN - 1);
    undo_marker.indonesian[MAX_WORD_LEN - 1] = '\0';
    stack_push(dict->undo_stack, undo_marker);

    printf("  [Sukses] Kata '%s' berhasil dihapus!\n", indonesian);
    return 1;
}

// Delete word without adding to undo stack (used by undo operation)
int dict_delete_word_no_undo(DictionaryManager* dict, const char* indonesian) {
    if (dict == NULL || indonesian == NULL) return 0;

    BSTNode* node = bst_search(dict->bst_root, indonesian);
    if (node == NULL) {
        return 0;
    }

    WordEntry deleted_word = node->word;

    dict->bst_root = bst_delete(dict->bst_root, indonesian);
    hash_delete(dict->hash_english, deleted_word.english);

    trie_destroy(dict->trie_root);
    dict->trie_root = trie_create_node();
    rebuild_trie(dict->trie_root, dict->bst_root);

    dict->total_words--;

    return 1;
}

// Mencari kata dalam bahasa Indonesia (BST)
WordEntry* dict_search_indonesian(DictionaryManager* dict, const char* indonesian) {
    if (dict == NULL || indonesian == NULL) return NULL;

    BSTNode* node = bst_search(dict->bst_root, indonesian);
    if (node != NULL) {
        list_insert_front(dict->search_history, node->word);
        return &(node->word);
    }

    return NULL;
}

// Mencari kata dalam bahasa Inggris (Hash Table)
WordEntry* dict_search_english(DictionaryManager* dict, const char* english) {
    if (dict == NULL || english == NULL) return NULL;

    WordEntry* word = hash_search(dict->hash_english, english);
    if (word != NULL) {
        list_insert_front(dict->search_history, *word);
        return word;
    }

    return NULL;
}

// Mencari kata dengan prefix (autocomplete via Trie)
WordEntry* dict_search_prefix(DictionaryManager* dict, const char* prefix, int* count) {
    if (dict == NULL || prefix == NULL || count == NULL) return NULL;

    WordEntry* results = NULL;
    *count = 0;

    trie_get_all_with_prefix(dict->trie_root, prefix, &results, count);

    return results;
}

// Menambahkan kata ke riwayat pencarian
void dict_add_to_history(DictionaryManager* dict, WordEntry word) {
    if (dict == NULL) return;
    list_insert_front(dict->search_history, word);
}

// Menyimpan kata terakhir yang dicari
void dict_set_last_searched(DictionaryManager* dict, WordEntry word) {
    if (dict == NULL) return;
    dict->last_searched = word;
    dict->has_last_searched = 1;
}

// Mendapatkan kata terakhir yang dicari
WordEntry* dict_get_last_searched(DictionaryManager* dict) {
    if (dict == NULL || !dict->has_last_searched) return NULL;
    return &dict->last_searched;
}

// ============================================================================
// VIEW OPERATIONS
// ============================================================================

// Menampilkan semua kata dengan pagination dan filter alphabet
void dict_view_all(DictionaryManager* dict) {
    if (dict == NULL || dict->total_words == 0) {
        printf("\n  [Info] Dictionary kosong!\n");
        return;
    }

    int words_per_page = 20;
    int total_words = dict->total_words;
    int current_page = 1;
    char input[20];
    char selected_letter = '*';
    int show_all = 1;

    WordEntry* all_words = (WordEntry*)malloc(sizeof(WordEntry) * total_words);
    if (all_words == NULL) {
        printf("\n  [Error] Gagal alokasi memori!\n");
        return;
    }
    int count = 0;
    collect_all_words(dict->bst_root, &all_words, &count);

    WordEntry* filtered_words = NULL;
    int filtered_count = 0;

    while (1) {
        printf("\n");

        free(filtered_words);
        filtered_words = (WordEntry*)malloc(sizeof(WordEntry) * total_words);
        if (filtered_words == NULL) {
            free(all_words);
            printf("\n  [Error] Gagal alokasi memori!\n");
            return;
        }
        filtered_count = 0;

        if (show_all) {
            for (int i = 0; i < count; i++) {
                filtered_words[i] = all_words[i];
                filtered_count++;
            }
        } else {
            for (int i = 0; i < count; i++) {
                if (toupper(all_words[i].indonesian[0]) == selected_letter) {
                    filtered_words[filtered_count] = all_words[i];
                    filtered_count++;
                }
            }
        }

        int total_pages = (filtered_count + words_per_page - 1) / words_per_page;
        if (total_pages < 1) total_pages = 1;
        if (current_page > total_pages) current_page = 1;

        int start_idx = (current_page - 1) * words_per_page;
        int end_idx = start_idx + words_per_page;
        if (end_idx > filtered_count) end_idx = filtered_count;

        // Hitung lebar kolom berdasarkan SEMUA kata di dictionary (statis)
        int max_indonesian = 12;
        int max_english = 12;
        int max_pos = 4;
        int max_category = 8;

        for (int i = 0; i < count; i++) {
            int len_ind = strlen(all_words[i].indonesian);
            int len_eng = strlen(all_words[i].english);
            int len_pos = strlen(pos_to_string(all_words[i].pos));
            int len_cat = strlen(category_to_string(all_words[i].category));

            if (len_ind > max_indonesian) max_indonesian = len_ind;
            if (len_eng > max_english) max_english = len_eng;
            if (len_pos > max_pos) max_pos = len_pos;
            if (len_cat > max_category) max_category = len_cat;
        }

        if (max_indonesian > 25) max_indonesian = 25;
        if (max_english > 25) max_english = 25;
        if (max_pos > 8) max_pos = 8;
        if (max_category > 12) max_category = 12;

        int no_width = 4;
        int total_table_width = no_width + max_indonesian + max_english + max_pos + max_category + 9;

        printf("  +");
        for (int i = 0; i < total_table_width; i++) printf("-");
        printf("+\n");
        printf("  |                         DAFTAR SEMUA KATA                           |\n");
        printf("  +");
        for (int i = 0; i < total_table_width; i++) printf("-");
        printf("+\n");
        printf("  |  FILTER: ");
        for (int i = 'A'; i <= 'Z'; i++) {
            if (!show_all && i == selected_letter) {
                printf("[*] ");
            } else {
                printf("[%c] ", i);
            }
            if (i == 'M') printf("\n  |         ");
        }
        printf("\n");
        printf("  +");
        for (int i = 0; i < total_table_width; i++) printf("-");
        printf("+\n");

        printf("\n      Filter: %c      |      Halaman %d/%d      |      Total: %d kata\n",
               show_all ? '*' : selected_letter, current_page, total_pages, filtered_count);

        // Header border - proportional with table content
        printf("  +");
        for (int i = 0; i < no_width - 1; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_indonesian + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_english + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_pos + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_category; i++) printf("-");
        printf("+\n");

        // Table header row
        printf("  %-4s | %-*s | %-*s | %-*s | %s\n",
               "No.", max_indonesian, "Indonesian", max_english, "English", max_pos, "POS", "Category");

        // Header border (bottom)
        printf("  +");
        for (int i = 0; i < no_width - 1; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_indonesian + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_english + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_pos + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_category; i++) printf("-");
        printf("+\n");

        // Data rows
        for (int i = start_idx; i < end_idx; i++) {
            printf("  %-4d | %-*s | %-*s | %-*s | %s\n",
                   i + 1,
                   max_indonesian, filtered_words[i].indonesian,
                   max_english, filtered_words[i].english,
                   max_pos, pos_to_string(filtered_words[i].pos),
                   category_to_string(filtered_words[i].category));
        }

        // Footer border
        printf("  +");
        for (int i = 0; i < no_width - 1; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_indonesian + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_english + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_pos + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_category; i++) printf("-");
        printf("+\n");

        printf("\n  +");
        for (int i = 0; i < total_table_width; i++) printf("-");
        printf("+\n");
        printf("  |     [1-%d] Pilih Kata    |  [A-Z] Awalan  |  [/1-%d] Halaman      |\n", filtered_count, total_pages);
        printf("  |     [0] Tampilkan Semua   |  [<] Menu Utama                          |\n");
        printf("  +");
        for (int i = 0; i < total_table_width; i++) printf("-");
        printf("+\n");
        printf("\n  Pilihan: ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            free(all_words);
            free(filtered_words);
            return;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {
            continue;
        }

        char first_char = input[0];

        if (first_char == '<') {
            printf("\n  [Info] Kembali ke menu utama.\n");
            break;
        }

        if (first_char == '/') {
            int page_num = atoi(input + 1);
            if (page_num >= 1 && page_num <= total_pages) {
                current_page = page_num;
            } else {
                printf("\n  [Error] Halaman tidak valid. Pilih 1-%d\n", total_pages);
            }
            continue;
        }

        if (first_char == '0') {
            show_all = 1;
            current_page = 1;
            continue;
        }

        if ((first_char >= 'a' && first_char <= 'z') || (first_char >= 'A' && first_char <= 'Z')) {
            selected_letter = toupper(first_char);
            show_all = 0;
            current_page = 1;
            continue;
        }

        if (first_char >= '1' && first_char <= '9') {
            int word_num = atoi(input);
            if (word_num >= 1 && word_num <= filtered_count) {
                WordEntry* word = &filtered_words[word_num - 1];

                // Tambahkan ke riwayat pencarian
                list_insert_front(dict->search_history, *word);
                dict_set_last_searched(dict, *word);

                // Hitung lebar box berdasarkan kata terpanjang
                int max_len = strlen(word->indonesian);
                if ((int)strlen(word->english) > max_len) max_len = strlen(word->english);
                if ((int)strlen(word->example) > max_len) max_len = strlen(word->example);
                if ((int)strlen(word->pronunciation) > max_len) max_len = strlen(word->pronunciation);
                if ((int)strlen(word->meanings[0].text) > max_len) max_len = strlen(word->meanings[0].text);

                int box_width = max_len + 18;
                if (box_width < 45) box_width = 45;
                if (box_width > 80) box_width = 80;

                // Hitung padding untuk centering judul
                int title_len = 11;  // strlen("DETAIL KATA")
                int pad_left = (box_width - title_len) / 2;
                int pad_right = box_width - title_len - pad_left;

                printf("\n");
                printf("  +");
                for (int i = 0; i < box_width; i++) printf("-");
                printf("+\n");
                printf("  |%*sDETAIL KATA%*s|\n", pad_left, "", pad_right, "");
                printf("  +");
                for (int i = 0; i < box_width; i++) printf("-");
                printf("+\n");

                // Label width
                int label_w = 13;

                // Indonesian
                printf("  | %-*s : %s\n", label_w - 2, "Indonesian", word->indonesian);
                // English
                printf("  | %-*s : %s\n", label_w - 2, "English", word->english);
                // POS
                printf("  | %-*s : %s\n", label_w - 2, "POS", pos_to_string(word->pos));
                // Category
                printf("  | %-*s : %s\n", label_w - 2, "Category", category_to_string(word->category));
                // Pronounce
                printf("  | %-*s : %s\n", label_w - 2, "Pronounce", word->pronunciation);

                // Separator
                printf("  +");
                for (int i = 0; i < box_width; i++) printf("-");
                printf("+\n");

                // Definitions
                printf("  | %-*s:\n", label_w - 2, "Definition(s)");
                for (int i = 0; i < word->meaning_count; i++) {
                    printf("  | %*d. %s\n", label_w - 3, i + 1, word->meanings[i].text);
                }

                // Separator
                printf("  +");
                for (int i = 0; i < box_width; i++) printf("-");
                printf("+\n");

                // Example
                printf("  | %-*s : %s\n", label_w - 2, "Example", word->example);

                printf("  +");
                for (int i = 0; i < box_width; i++) printf("-");
                printf("+\n");

                printf("\n  [Tekan Enter untuk kembali ke daftar kata]\n");
                printf("  Pilihan: ");
                if (fgets(input, sizeof(input), stdin) == NULL) {
                    // User pressed EOF, continue anyway
                }

                continue;
            } else {
                printf("\n  [Error] Nomor tidak valid. Pilih 1-%d\n", filtered_count);
            }
            continue;
        }

        printf("\n  [Info] Pilihan tidak valid.\n");
    }

    free(all_words);
    free(filtered_words);
}

// Menampilkan riwayat pencarian
void dict_display_history(DictionaryManager* dict) {
    if (dict == NULL) return;

    printf("\n");
    printf("  +========================================================================+\n");
    printf("  |                       RIWAYAT PENCARIAN TERAKHIR                       |\n");
    printf("  +========================================================================+\n");

    list_display(dict->search_history);

    printf("  +========================================================================+\n");
}

// Menampilkan kata acak (Word of the Day)
void dict_word_of_day(DictionaryManager* dict) {
    if (dict == NULL || queue_is_empty(dict->word_queue)) {
        printf("  [Error] Queue kosong!\n");
        return;
    }

    queue_shuffle(dict->word_queue);

    WordEntry* word = queue_get_random(dict->word_queue);
    char input[20];

    if (word != NULL) {
        // Tambahkan ke riwayat pencarian
        list_insert_front(dict->search_history, *word);

        // Simpan sebagai kata terakhir
        dict_set_last_searched(dict, *word);

        printf("\n");
        printf("  +========================================================================+\n");
        printf("  |                           WORD OF THE DAY                              |\n");
        printf("  +========================================================================+\n");
        print_word_entry(word);
        printf("  +========================================================================+\n");

        printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
        printf("  Pilihan: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // User pressed EOF, continue anyway
        }
    }
}

// ============================================================================
// UNDO OPERATIONS
// ============================================================================

// Membatalkan aksi terakhir (add/delete/update)
int dict_undo(DictionaryManager* dict) {
    if (dict == NULL) return 0;

    if (stack_is_empty(dict->undo_stack)) {
        printf("  [Info] Tidak ada aksi yang dapat di-undo.\n");
        printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
        printf("  Pilihan: ");
        char input[20];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // User pressed EOF, continue anyway
        }
        return 0;
    }

    WordEntry word;
    if (stack_pop(dict->undo_stack, &word)) {
        char action_type[20];
        char word_name[MAX_WORD_LEN];

        // Parse the marker prefix
        if (strncmp(word.indonesian, "@ADD:", 5) == 0) {
            strcpy(action_type, "Menambahkan kata");
            strcpy(word_name, word.indonesian + 5);
        } else if (strncmp(word.indonesian, "@UPDATE:", 8) == 0) {
            strcpy(action_type, "Mengupdate kata");
            strcpy(word_name, word.indonesian + 8);
        } else if (strncmp(word.indonesian, "@DELETE:", 8) == 0) {
            strcpy(action_type, "Menghapus kata");
            strcpy(word_name, word.indonesian + 8);
        } else {
            // Fallback for old format - try to determine action
            if (bst_search(dict->bst_root, word.indonesian) != NULL) {
                strcpy(action_type, "Menghapus kata");
                strcpy(word_name, word.indonesian);
            } else {
                strcpy(action_type, "Menambahkan kata");
                strcpy(word_name, word.indonesian);
            }
        }

        // Perform the undo action
        char success_msg[150];
        if (strncmp(word.indonesian, "@ADD:", 5) == 0) {
            // Undo ADD = delete the word that was added
            dict_delete_word_no_undo(dict, word_name);
            snprintf(success_msg, sizeof(success_msg), "  [Sukses] Undo berhasil! %s '%s' telah dihapus.", action_type, word_name);
        } else if (strncmp(word.indonesian, "@UPDATE:", 8) == 0) {
            // Undo UPDATE = restore the old word
            dict_add_word(dict, word);
            snprintf(success_msg, sizeof(success_msg), "  [Sukses] Undo berhasil! %s '%s' telah dikembalikan.", action_type, word_name);
        } else if (strncmp(word.indonesian, "@DELETE:", 8) == 0) {
            // Undo DELETE = restore the deleted word
            dict_add_word(dict, word);
            snprintf(success_msg, sizeof(success_msg), "  [Sukses] Undo berhasil! %s '%s' telah dikembalikan.", action_type, word_name);
        } else {
            // Fallback
            if (bst_search(dict->bst_root, word.indonesian) != NULL) {
                dict_delete_word(dict, word.indonesian);
            } else {
                dict_add_word(dict, word);
            }
            snprintf(success_msg, sizeof(success_msg), "  [Sukses] Undo berhasil!");
        }

        printf("%s\n", success_msg);
        printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
        printf("  Pilihan: ");
        char input[20];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // User pressed EOF, continue anyway
        }
        return 1;
    }

    return 0;
}

// ============================================================================
// STATISTICS
// ============================================================================

// Menampilkan statistik dictionary
void dict_statistics(DictionaryManager* dict) {
    if (dict == NULL) return;

    int pos_count[10] = {0};
    int cat_count[4] = {0};

    if (dict->bst_root != NULL) {
        count_bst_stats(dict->bst_root, pos_count, cat_count);
    }

    printf("\n");
    printf("  +========================================================================+\n");
    printf("  |                            STATISTIK KAMUS                             |\n");
    printf("  +========================================================================+\n");
    printf("  |  Total Kata              : %d kata\n", dict->total_words);
    printf("  |  BST Nodes               : %d nodes\n", bst_count(dict->bst_root));
    printf("  |  Hash Table Size         : %d entries\n", hash_size(dict->hash_english));
    printf("  |  Search History Size     : %d entries\n", dict->search_history->size);
    printf("  |  Undo Stack Size         : %d entries\n", dict->undo_stack->top + 1);
    printf("  +========================================================================+\n");
    printf("  |  DISTRIBUSI PART OF SPEECH:\n");
    printf("  |    - Noun                : %d\n", pos_count[NOUN]);
    printf("  |    - Verb                : %d\n", pos_count[VERB]);
    printf("  |    - Adjective           : %d\n", pos_count[ADJECTIVE]);
    printf("  |    - Adverb              : %d\n", pos_count[ADVERB]);
    printf("  |    - Lainnya             : %d\n", pos_count[PRONOUN] + pos_count[PREPOSITION] +
                                                  pos_count[CONJUNCTION] + pos_count[INTERJECTION] +
                                                  pos_count[DETERMINER]);
    printf("  +========================================================================+\n");
    printf("  |  DISTRIBUSI KATEGORI     :\n");
    printf("  |    - Everyday            : %d\n", cat_count[EVERYDAY]);
    printf("  |    - Formal              : %d\n", cat_count[FORMAL]);
    printf("  |    - Slang               : %d\n", cat_count[SLANG]);
    printf("  |    - Technical           : %d\n", cat_count[TECHNICAL]);
    printf("  +========================================================================+\n");

    printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
    printf("  Pilihan: ");
    char input[20];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        // User pressed EOF, continue anyway
    }
}

// ============================================================================
// DATA PERSISTENCE
// ============================================================================

// Menyimpan dictionary ke file
void dict_save_to_file(DictionaryManager* dict, const char* filename) {
    if (dict == NULL || filename == NULL) return;

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("  [Error] Gagal menyimpan ke file!\n");
        return;
    }

    save_bst_node(file, dict->bst_root);
    fclose(file);

    printf("  [Sukses] Data disimpan ke '%s'\n", filename);
}