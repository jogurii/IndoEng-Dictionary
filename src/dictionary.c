#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

// ============================================================================
// Helper functions
// ============================================================================

// Collect BST to array
static void collect_all_words(BSTNode* node, WordEntry** results, int* count) {
    if (node == NULL) return;

    collect_all_words(node->left, results, count);
    (*results)[*count] = node->word;
    (*count)++;
    collect_all_words(node->right, results, count);
}

// Calc BST stats
static void count_bst_stats(BSTNode* n, int* pos_count, int* cat_count) {
    if (n == NULL) return;
    int pos_idx = (int)n->word.pos;
    int cat_idx = (int)n->word.category;
    if (pos_idx >= 0 && pos_idx < 10) pos_count[pos_idx]++;
    if (cat_idx >= 0 && cat_idx < 4) cat_count[cat_idx]++;
    count_bst_stats(n->left, pos_count, cat_count);
    count_bst_stats(n->right, pos_count, cat_count);
}

// Save BST node
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
// Init & Cleanup
// ============================================================================

// Create DictionaryManager
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
    dict->has_last_searched = 0;
    memset(&dict->last_searched, 0, sizeof(WordEntry));

    return dict;
}

// Free DictionaryManager
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
// CRUD operations OPERATIONS
// ============================================================================

// Add word
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

// Update word
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

    hash_remove(dict->hash_english, old_word.english);
    hash_insert(dict->hash_english, new_word);

    // Update trie in-place instead of full rebuild
    trie_update(dict->trie_root, old_word.indonesian, new_word.indonesian, new_word);

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

// Delete word
int dict_delete_word(DictionaryManager* dict, const char* indonesian) {
    if (dict == NULL || indonesian == NULL) return 0;

    BSTNode* node = bst_search(dict->bst_root, indonesian);
    if (node == NULL) {
        printf("  [Error] Kata '%s' tidak ditemukan!\n", indonesian);
        return 0;
    }

    WordEntry deleted_word = node->word;

    dict->bst_root = bst_delete(dict->bst_root, indonesian);
    hash_remove(dict->hash_english, deleted_word.english);

    // Update trie in-place instead of full rebuild
    trie_delete_word(dict->trie_root, deleted_word.indonesian);

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

// Delete word (no undo)
int dict_delete_word_no_undo(DictionaryManager* dict, const char* indonesian) {
    if (dict == NULL || indonesian == NULL) return 0;

    BSTNode* node = bst_search(dict->bst_root, indonesian);
    if (node == NULL) {
        return 0;
    }

    WordEntry deleted_word = node->word;

    dict->bst_root = bst_delete(dict->bst_root, indonesian);
    hash_remove(dict->hash_english, deleted_word.english);

    // Update trie in-place instead of full rebuild
    trie_delete_word(dict->trie_root, deleted_word.indonesian);

    dict->total_words--;

    return 1;
}

// Search Indonesian (BST)
WordEntry* dict_search_indonesian(DictionaryManager* dict, const char* indonesian) {
    if (dict == NULL || indonesian == NULL) return NULL;

    BSTNode* node = bst_search(dict->bst_root, indonesian);
    if (node != NULL) {
        list_insert_front(dict->search_history, node->word);
        return &(node->word);
    }

    return NULL;
}

// Search English (Hash Table)
WordEntry* dict_search_english(DictionaryManager* dict, const char* english) {
    if (dict == NULL || english == NULL) return NULL;

    WordEntry* word = hash_search(dict->hash_english, english);
    if (word != NULL) {
        list_insert_front(dict->search_history, *word);
        return word;
    }

    return NULL;
}

// Search prefix (Trie)
WordEntry* dict_search_prefix(DictionaryManager* dict, const char* prefix, int* count) {
    if (dict == NULL || prefix == NULL || count == NULL) return NULL;

    WordEntry* results = NULL;
    *count = 0;

    trie_get_all_with_prefix(dict->trie_root, prefix, &results, count);

    return results;
}

// Add to history
void dict_add_to_history(DictionaryManager* dict, WordEntry word) {
    if (dict == NULL) return;
    list_insert_front(dict->search_history, word);
}

// Set last searched
void dict_set_last_searched(DictionaryManager* dict, WordEntry word) {
    if (dict == NULL) return;
    dict->last_searched = word;
    dict->has_last_searched = 1;
}

// Get last searched
WordEntry* dict_get_last_searched(DictionaryManager* dict) {
    if (dict == NULL || !dict->has_last_searched) return NULL;
    return &dict->last_searched;
}

// ============================================================================
// View Operations
// ============================================================================

// View all words
void dict_view_all(DictionaryManager* dict) {
    if (dict == NULL || dict->total_words == 0) {
        printf("\n  " CLR_CYAN "[Info]" CLR_RESET " Dictionary kosong!\n");
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
        printf("\n  " CLR_RED "[Error]" CLR_RESET " Gagal alokasi memori!\n");
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
            printf("\n  " CLR_RED "[Error]" CLR_RESET " Gagal alokasi memori!\n");
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

        printf(CLR_CYAN "  +");
        for (int i = 0; i < total_table_width; i++) printf("-");
        printf("+\n");
        printf("  |                         DAFTAR SEMUA KATA                           |\n");
        printf("  +");
        for (int i = 0; i < total_table_width; i++) printf("-");
        printf("+\n");
        printf("  |  " CLR_BOLD CLR_YELLOW "FILTER: " CLR_RESET CLR_CYAN);
        for (int i = 'A'; i <= 'Z'; i++) {
            if (!show_all && i == selected_letter) {
                printf(CLR_BOLD CLR_YELLOW "[*] " CLR_RESET CLR_CYAN);
            } else {
                printf("[%c] ", i);
            }
            if (i == 'M') printf("\n  |         ");
        }
        printf("\n");
        printf("  +");
        for (int i = 0; i < total_table_width; i++) printf("-");
        printf("+\n" CLR_RESET);

        printf("\n      Filter: " CLR_BOLD CLR_YELLOW "%c" CLR_RESET "      |      Halaman " CLR_BOLD CLR_YELLOW "%d/%d" CLR_RESET "      |      Total: " CLR_BOLD CLR_GREEN "%d" CLR_RESET " kata\n",
               show_all ? '*' : selected_letter, current_page, total_pages, filtered_count);

        // Header border - proportional with table content
        printf(CLR_CYAN "  +");
        for (int i = 0; i < no_width - 1; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_indonesian + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_english + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_pos + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_category; i++) printf("-");
        printf("+\n" CLR_RESET);

        // Table header row
        printf("  " CLR_BOLD CLR_WHITE "%-4s" CLR_RESET CLR_CYAN " | " CLR_BOLD CLR_WHITE "%-*s" CLR_RESET CLR_CYAN " | " CLR_BOLD CLR_WHITE "%-*s" CLR_RESET CLR_CYAN " | " CLR_BOLD CLR_WHITE "%-*s" CLR_RESET CLR_CYAN " | " CLR_BOLD CLR_WHITE "%s" CLR_RESET "\n",
               "No.", max_indonesian, "Indonesian", max_english, "English", max_pos, "POS", "Category");

        // Header border (bottom)
        printf(CLR_CYAN "  +");
        for (int i = 0; i < no_width - 1; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_indonesian + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_english + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_pos + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_category; i++) printf("-");
        printf("+\n" CLR_RESET);

        // Data rows
        for (int i = start_idx; i < end_idx; i++) {
            printf("  %-4d | " CLR_GREEN "%-*s" CLR_RESET " | " CLR_YELLOW "%-*s" CLR_RESET " | " CLR_MAGENTA "%-*s" CLR_RESET " | %s\n",
                   i + 1,
                   max_indonesian, filtered_words[i].indonesian,
                   max_english, filtered_words[i].english,
                   max_pos, pos_to_string(filtered_words[i].pos),
                   category_to_string(filtered_words[i].category));
        }

        // Footer border
        printf(CLR_CYAN "  +");
        for (int i = 0; i < no_width - 1; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_indonesian + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_english + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_pos + 2; i++) printf("-");
        printf("+");
        for (int i = 0; i < max_category; i++) printf("-");
        printf("+\n" CLR_RESET);

        printf("\n");
        printf(CLR_CYAN "  +");
        for (int i = 0; i < total_table_width; i++) printf("-");
        printf("+\n");
        printf("  |     [1-%d] Pilih Kata    |  [A-Z] Awalan   |  [/1-%d] Halaman      |\n", filtered_count, total_pages);
        printf("  |     [0] Tampilkan Semua   |  [<] Menu Utama                         |\n");
        printf("  +");
        for (int i = 0; i < total_table_width; i++) printf("-");
        printf("+\n" CLR_RESET);
        printf("\n  Pilihan: " CLR_BOLD CLR_YELLOW);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf(CLR_RESET);
            free(all_words);
            free(filtered_words);
            return;
        }
        printf(CLR_RESET);

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {
            continue;
        }

        char first_char = input[0];

        if (first_char == '<') {
            printf("\n  " CLR_CYAN "[Info]" CLR_RESET " Kembali ke menu utama.\n");
            break;
        }

        if (first_char == '/') {
            int page_num = atoi(input + 1);
            if (page_num >= 1 && page_num <= total_pages) {
                current_page = page_num;
            } else {
                printf("\n  " CLR_RED "[Error]" CLR_RESET " Halaman tidak valid. Pilih 1-%d\n", total_pages);
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
                printf(CLR_BLUE "  +");
                for (int i = 0; i < box_width; i++) printf("-");
                printf("+\n" CLR_RESET);
                printf(CLR_BLUE "  |" CLR_RESET "%*s" CLR_BOLD CLR_YELLOW "DETAIL KATA" CLR_RESET "%*s" CLR_BLUE "|\n" CLR_RESET, pad_left, "", pad_right, "");
                printf(CLR_BLUE "  +");
                for (int i = 0; i < box_width; i++) printf("-");
                printf("+\n" CLR_RESET);

                // Label width
                int label_w = 13;

                // Indonesian
                printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET " : " CLR_GREEN "%s" CLR_RESET "\n", label_w - 2, "Indonesian", word->indonesian);
                // English
                printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET " : " CLR_GREEN "%s" CLR_RESET "\n", label_w - 2, "English", word->english);
                // POS
                printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET " : " CLR_MAGENTA "%s" CLR_RESET "\n", label_w - 2, "POS", pos_to_string(word->pos));
                // Category
                printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET " : " CLR_MAGENTA "%s" CLR_RESET "\n", label_w - 2, "Category", category_to_string(word->category));
                // Pronounce
                printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET " : " CLR_YELLOW "%s" CLR_RESET "\n", label_w - 2, "Pronounce", word->pronunciation);

                // Separator
                printf(CLR_BLUE "  +");
                for (int i = 0; i < box_width; i++) printf("-");
                printf("+\n" CLR_RESET);

                // Definitions
                printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET ":\n", label_w - 2, "Definition(s)");
                for (int i = 0; i < word->meaning_count; i++) {
                    printf(CLR_BLUE "  |" CLR_RESET " %*d. " CLR_WHITE "%s" CLR_RESET "\n", label_w - 3, i + 1, word->meanings[i].text);
                }

                // Separator
                printf(CLR_BLUE "  +");
                for (int i = 0; i < box_width; i++) printf("-");
                printf("+\n" CLR_RESET);

                // Example
                printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET " : " CLR_WHITE "%s" CLR_RESET "\n", label_w - 2, "Example", word->example);

                printf(CLR_BLUE "  +");
                for (int i = 0; i < box_width; i++) printf("-");
                printf("+\n" CLR_RESET);

                printf("\n  [Tekan Enter untuk kembali ke daftar kata]\n");
                printf("  Pilihan: ");
                if (fgets(input, sizeof(input), stdin) == NULL) {
                    // User pressed EOF, continue anyway
                }

                continue;
            } else {
                printf("\n  " CLR_RED "[Error]" CLR_RESET " Nomor tidak valid. Pilih 1-%d\n", filtered_count);
            }
            continue;
        }

        printf("\n  " CLR_CYAN "[Info]" CLR_RESET " Pilihan tidak valid.\n");
    }

    free(all_words);
    free(filtered_words);
}

// Display history
void dict_display_history(DictionaryManager* dict) {
    if (dict == NULL) return;

    printf("\n");
    printf(CLR_CYAN "  +========================================================================+\n");
    printf("  |                       " CLR_BOLD CLR_YELLOW "RIWAYAT PENCARIAN TERAKHIR" CLR_RESET CLR_CYAN "                       |\n");
    printf("  +========================================================================+\n" CLR_RESET);

    list_display(dict->search_history);

    printf(CLR_CYAN "  +========================================================================+\n" CLR_RESET);

    printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
    printf("  Pilihan: ");
    char input[20];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        // User pressed EOF, continue anyway
    }
}

// Word of the day
void dict_word_of_day(DictionaryManager* dict) {
    if (dict == NULL || queue_is_empty(dict->word_queue)) {
        printf("  " CLR_RED "[Error]" CLR_RESET " Queue kosong!\n");
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
        printf(CLR_CYAN "  +========================================================================+\n");
        printf("  |                           " CLR_BOLD CLR_YELLOW "WORD OF THE DAY" CLR_RESET CLR_CYAN "                              |\n");
        printf("  +========================================================================+\n" CLR_RESET);
        print_word_entry(word);
        printf(CLR_CYAN "  +========================================================================+\n" CLR_RESET);

        printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
        printf("  Pilihan: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // User pressed EOF, continue anyway
        }
    }
}

// ============================================================================
// Undo Operations
// ============================================================================

// Undo last action
int dict_undo(DictionaryManager* dict) {
    if (dict == NULL) return 0;

    if (stack_is_empty(dict->undo_stack)) {
        printf("  " CLR_CYAN "[Info]" CLR_RESET " Tidak ada aksi yang dapat di-undo.\n");
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
        char action_type[40];
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
        char success_msg[200];
        if (strncmp(word.indonesian, "@ADD:", 5) == 0) {
            // Undo operations ADD = delete the word that was added
            dict_delete_word_no_undo(dict, word_name);
            snprintf(success_msg, sizeof(success_msg), "  " CLR_GREEN "[Sukses]" CLR_RESET " Undo berhasil! %s '" CLR_BOLD "%s" CLR_RESET "' telah dihapus.", action_type, word_name);
        } else if (strncmp(word.indonesian, "@UPDATE:", 8) == 0) {
            // Undo operations UPDATE = restore the old word
            dict_add_word(dict, word);
            snprintf(success_msg, sizeof(success_msg), "  " CLR_GREEN "[Sukses]" CLR_RESET " Undo berhasil! %s '" CLR_BOLD "%s" CLR_RESET "' telah dikembalikan.", action_type, word_name);
        } else if (strncmp(word.indonesian, "@DELETE:", 8) == 0) {
            // Undo operations DELETE = restore the deleted word
            dict_add_word(dict, word);
            snprintf(success_msg, sizeof(success_msg), "  " CLR_GREEN "[Sukses]" CLR_RESET " Undo berhasil! %s '" CLR_BOLD "%s" CLR_RESET "' telah dikembalikan.", action_type, word_name);
        } else {
            // Fallback
            if (bst_search(dict->bst_root, word.indonesian) != NULL) {
                dict_delete_word(dict, word.indonesian);
            } else {
                dict_add_word(dict, word);
            }
            snprintf(success_msg, sizeof(success_msg), "  " CLR_GREEN "[Sukses]" CLR_RESET " Undo berhasil!");
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
// Statistics
// ============================================================================

// Display statistics
void dict_statistics(DictionaryManager* dict) {
    if (dict == NULL) return;

    int pos_count[10] = {0};
    int cat_count[4] = {0};

    if (dict->bst_root != NULL) {
        count_bst_stats(dict->bst_root, pos_count, cat_count);
    }

    printf("\n");
    printf(CLR_CYAN "  +========================================================================+\n");
    printf("  |                            " CLR_BOLD CLR_YELLOW "STATISTIK KAMUS" CLR_RESET CLR_CYAN "                             |\n");
    printf("  +========================================================================+\n");
    printf("  |  " CLR_BOLD CLR_WHITE "Total Kata" CLR_RESET CLR_CYAN "              : " CLR_GREEN "%d kata\n" CLR_CYAN, dict->total_words);
    printf("  |  " CLR_BOLD CLR_WHITE "BST Nodes" CLR_RESET CLR_CYAN "               : %d nodes\n", bst_count(dict->bst_root));
    printf("  |  " CLR_BOLD CLR_WHITE "Hash Table Size" CLR_RESET CLR_CYAN "         : %d entries\n", hash_size(dict->hash_english));
    printf("  |  " CLR_BOLD CLR_WHITE "Search History Size" CLR_RESET CLR_CYAN "     : %d entries\n", dict->search_history->size);
    printf("  |  " CLR_BOLD CLR_WHITE "Undo Stack Size" CLR_RESET CLR_CYAN "         : %d entries\n", dict->undo_stack->top + 1);
    printf("  +========================================================================+\n");
    printf("  |  " CLR_BOLD CLR_YELLOW "DISTRIBUSI PART OF SPEECH:" CLR_RESET CLR_CYAN "\n");
    printf("  |    - Noun                : %d\n", pos_count[NOUN]);
    printf("  |    - Verb                : %d\n", pos_count[VERB]);
    printf("  |    - Adjective           : %d\n", pos_count[ADJECTIVE]);
    printf("  |    - Adverb              : %d\n", pos_count[ADVERB]);
    printf("  |    - Lainnya             : %d\n", pos_count[PRONOUN] + pos_count[PREPOSITION] +
                                                  pos_count[CONJUNCTION] + pos_count[INTERJECTION] +
                                                  pos_count[DETERMINER]);
    printf("  +========================================================================+\n");
    printf("  |  " CLR_BOLD CLR_YELLOW "DISTRIBUSI KATEGORI     :" CLR_RESET CLR_CYAN "\n");
    printf("  |    - Everyday            : %d\n", cat_count[EVERYDAY]);
    printf("  |    - Formal              : %d\n", cat_count[FORMAL]);
    printf("  |    - Slang               : %d\n", cat_count[SLANG]);
    printf("  |    - Technical           : %d\n", cat_count[TECHNICAL]);
    printf("  +========================================================================+\n" CLR_RESET);

    printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
    printf("  Pilihan: ");
    char input[20];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        // User pressed EOF, continue anyway
    }
}

// ============================================================================
// Data Persistence
// ============================================================================

// Save dict to file
void dict_save_to_file(DictionaryManager* dict, const char* filename) {
    if (dict == NULL || filename == NULL) return;

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("  " CLR_RED "[Error]" CLR_RESET " Gagal menyimpan ke file!\n");
        return;
    }

    save_bst_node(file, dict->bst_root);
    fclose(file);

    printf("  " CLR_GREEN "[Sukses]" CLR_RESET " Data disimpan ke '" CLR_BOLD "%s" CLR_RESET "'\n", filename);
}