#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "dictionary.h"
#include "data_structures.h"

// ============================================================================
// DEKLARASI FUNGSI STATIC
// ============================================================================

static void display_main_menu(void);
static void clear_input_buffer(void);
static void handle_search_indonesian(DictionaryManager* dict);
static void handle_search_english(DictionaryManager* dict);
static void handle_prefix_search(DictionaryManager* dict);
static void handle_add_word(DictionaryManager* dict);
static void handle_update_word(DictionaryManager* dict);
static void handle_delete_word(DictionaryManager* dict);
static PartOfSpeech get_pos_from_input(void);
static WordCategory get_category_from_input(void);
static char* get_string_input(char* prompt, char* buffer, int max_len);

// ============================================================================
// FUNGSI UTAMA
// ============================================================================

// Helper function to print last searched word at top of menu
static void print_last_searched_if_exists(DictionaryManager* dict) {
    WordEntry* last = dict_get_last_searched(dict);
    if (last != NULL) {
        // Hitung lebar box
        int max_len = strlen(last->indonesian);
        if ((int)strlen(last->english) > max_len) max_len = strlen(last->english);
        if ((int)strlen(last->example) > max_len) max_len = strlen(last->example);
        if ((int)strlen(last->pronunciation) > max_len) max_len = strlen(last->pronunciation);
        if ((int)strlen(last->meanings[0].text) > max_len) max_len = strlen(last->meanings[0].text);

        int box_width = max_len + 20;
        if (box_width < 50) box_width = 50;
        if (box_width > 80) box_width = 80;

        int title_len = 11;  // strlen("KATA TERAKHIR")
        int pad_left = (box_width - title_len) / 2;
        int pad_right = box_width - title_len - pad_left;
        int label_w = 14;

        printf("\n");
        printf("  +");
        for (int i = 0; i < box_width; i++) printf("-");
        printf("+\n");
        printf("  |%*sKATA TERAKHIR%*s\n", pad_left, "", pad_right, "");
        printf("  +");
        for (int i = 0; i < box_width; i++) printf("-");
        printf("+\n");
        printf("  | %-*s : %s\n", label_w - 2, "Indonesian", last->indonesian);
        printf("  | %-*s : %s\n", label_w - 2, "English", last->english);
        printf("  | %-*s : %s\n", label_w - 2, "POS", pos_to_string(last->pos));
        printf("  | %-*s : %s\n", label_w - 2, "Category", category_to_string(last->category));
        printf("  | %-*s : %s\n", label_w - 2, "Pronounce", last->pronunciation);
        printf("  +");
        for (int i = 0; i < box_width; i++) printf("-");
        printf("+\n");
        printf("  | %-*s:\n", label_w - 2, "Definition(s)");
        for (int i = 0; i < last->meaning_count; i++) {
            printf("  | %*d. %s\n", label_w - 3, i + 1, last->meanings[i].text);
        }
        printf("  +");
        for (int i = 0; i < box_width; i++) printf("-");
        printf("+\n");
        printf("  | %-*s : %s\n", label_w - 2, "Example", last->example);
        printf("  +");
        for (int i = 0; i < box_width; i++) printf("-");
        printf("+\n\n");
    }
}

int main(void) {
    printf("\n");
    printf("  ============================================================\n");
    printf("  =                                                          =\n");
    printf("  =            INDONESIAN - ENGLISH DICTIONARY               =\n");
    printf("  =                    (IndoEng v1.0)                        =\n");
    printf("  =                                                          =\n");
    printf("  ============================================================\n");
    printf("  Loading dictionary data...\n\n");

    srand((unsigned int)time(NULL));

    DictionaryManager* dict = dict_create();
    if (dict == NULL) {
        fprintf(stderr, "  [Error] Gagal membuat dictionary!\n");
        return 1;
    }

    dict_load_initial_data(dict);

    printf("  Dictionary loaded successfully!\n");
    printf("  Total words: %d\n\n", dict->total_words);

    int choice;
    int running = 1;

    while (running) {
        // Bersihkan layar
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        // Tampilkan kata terakhir jika ada
        print_last_searched_if_exists(dict);

        display_main_menu();
        printf("\n  Masukkan pilihan: ");

        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("\n  [Error] Input tidak valid. Masukkan angka.\n");
            printf("  Tekan Enter untuk melanjutkan...");
            getchar();
            continue;
        }
        clear_input_buffer();

        // Bersihkan layar sebelum masuk ke fitur search
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        switch (choice) {
            case VIEW_ALL:
                dict_view_all(dict);
                break;
            case SEARCH_INDONESIAN:
                handle_search_indonesian(dict);
                break;
            case SEARCH_ENGLISH:
                handle_search_english(dict);
                break;
            case SEARCH_PREFIX:
                handle_prefix_search(dict);
                break;
            case ADD_WORD:
                handle_add_word(dict);
                break;
            case UPDATE_WORD:
                handle_update_word(dict);
                break;
            case DELETE_WORD:
                handle_delete_word(dict);
                break;
            case WORD_OF_DAY:
                dict_word_of_day(dict);
                break;
            case VIEW_HISTORY:
                dict_display_history(dict);
                break;
            case UNDO:
                dict_undo(dict);
                break;
            case STATISTICS:
                dict_statistics(dict);
                break;
            case EXIT_PROGRAM:
                running = 0;
                printf("\n");
                printf("  ============================================================\n");
                printf("  =                     TERIMA KASIH!                        =\n");
                printf("  =          Terima kasih sudah menggunakan IndoEng            =\n");
                printf("  ============================================================\n\n");
                break;
            default:
                printf("\n  [Error] Pilihan tidak valid.\n");
                break;
        }

        // Loop continues - screen will be cleared at start of next iteration
    }

    dict_destroy(dict);

    return 0;
}

// ============================================================================
// TAMPILAN MENU
// ============================================================================

// Menampilkan menu utama aplikasi
static void display_main_menu(void) {
    printf("\n");
    printf("  +----------------------------------------------------------+\n");
    printf("  |              INDONESIAN - ENGLISH DICTIONARY             |\n");
    printf("  |                       (IndoEng)                          |\n");
    printf("  +----------------------------------------------------------+\n");
    printf("  |                                                          |\n");
    printf("  |  1. View All Words                - Paginated A-Z        |\n");
    printf("  |  2. Search Word (Indonesian)      - BST Lookup           |\n");
    printf("  |  3. Search Word (English)         - Hash Table Lookup    |\n");
    printf("  |  4. Prefix Search (Autocomplete)  - Trie Lookup          |\n");
    printf("  |  5. Add New Word                  - Create               |\n");
    printf("  |  6. Update Word                   - Update               |\n");
    printf("  |  7. Delete Word                   - Delete               |\n");
    printf("  |  8. Word of the Day               - Random Pick          |\n");
    printf("  |  9. Search History                - Recent Searches      |\n");
    printf("  | 10. Undo Last Action              - Stack-based          |\n");
    printf("  | 11. Statistics                    - Dictionary Stats     |\n");
    printf("  |                                                          |\n");
    printf("  |  0. Exit                                                 |\n");
    printf("  |                                                          |\n");
    printf("  +----------------------------------------------------------+\n");
}

// ============================================================================
// FUNGSI INPUT
// ============================================================================

// Membersihkan buffer input
static void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Meminta input string dari user
static char* get_string_input(char* prompt, char* buffer, int max_len) {
    printf("  %s: ", prompt);
    if (fgets(buffer, max_len, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        int i = 0;
        while (isspace(buffer[i])) i++;
        if (i > 0) {
            memmove(buffer, buffer + i, strlen(buffer) - i + 1);
        }
        return buffer;
    }
    return NULL;
}

// Meminta input Part of Speech
static PartOfSpeech get_pos_from_input(void) {
    printf("\n  Part of Speech:\n");
    printf("    0 - Noun\n");
    printf("    1 - Verb\n");
    printf("    2 - Adjective\n");
    printf("    3 - Adverb\n");
    printf("    4 - Pronoun\n");
    printf("    5 - Preposition\n");
    printf("    6 - Conjunction\n");
    printf("    7 - Interjection\n");
    printf("    8 - Determiner\n");
    printf("  Masukkan pilihan (0-8): ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        return NOUN;
    }
    clear_input_buffer();

    switch (choice) {
        case 0: return NOUN;
        case 1: return VERB;
        case 2: return ADJECTIVE;
        case 3: return ADVERB;
        case 4: return PRONOUN;
        case 5: return PREPOSITION;
        case 6: return CONJUNCTION;
        case 7: return INTERJECTION;
        case 8: return DETERMINER;
        default: return NOUN;
    }
}

// Meminta input Kategori kata
static WordCategory get_category_from_input(void) {
    printf("\n  Category:\n");
    printf("    0 - Everyday\n");
    printf("    1 - Formal\n");
    printf("    2 - Slang\n");
    printf("    3 - Technical\n");
    printf("  Masukkan pilihan (0-3): ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        return EVERYDAY;
    }
    clear_input_buffer();

    switch (choice) {
        case 0: return EVERYDAY;
        case 1: return FORMAL;
        case 2: return SLANG;
        case 3: return TECHNICAL;
        default: return EVERYDAY;
    }
}

// ============================================================================
// HANDLER PENCARIAN
// ============================================================================

// Handler pencarian kata Indonesia (BST)
static void handle_search_indonesian(DictionaryManager* dict) {
    char input[MAX_WORD_LEN];
    char choice[20];

    while (1) {
        // Cek apakah ada kata terakhir yang disimpan
        WordEntry* saved_last = dict_get_last_searched(dict);

        printf("\n");
        printf("  +----------------------------------------------------------+\n");
        printf("  |              CARI KATA INDONESIA (BST)                   |\n");
        printf("  +----------------------------------------------------------+\n");
        if (saved_last != NULL) {
            printf("  |  Kata terakhir: %-36s |\n", saved_last->indonesian);
        } else {
            printf("  |  Pencarian berdasarkan kata Indonesia         |\n");
        }
        printf("  +----------------------------------------------------------+\n");

        if (get_string_input("Masukkan kata Indonesia", input, MAX_WORD_LEN) == NULL) {
            return;
        }

        if (strlen(input) == 0) {
            if (saved_last != NULL) {
                printf("  [Info] Kata terakhir: %s\n", saved_last->indonesian);
            } else {
                printf("  [Error] Masukkan kata yang ingin dicari.\n");
            }
            continue;
        }

        printf("\n  Mencari...\n");

        WordEntry* result = dict_search_indonesian(dict, input);

        if (result != NULL) {
            // Simpan sebagai kata terakhir
            dict_set_last_searched(dict, *result);

            // Hitung lebar box
            int max_len = strlen(result->indonesian);
            if ((int)strlen(result->english) > max_len) max_len = strlen(result->english);
            if ((int)strlen(result->example) > max_len) max_len = strlen(result->example);
            if ((int)strlen(result->pronunciation) > max_len) max_len = strlen(result->pronunciation);
            if ((int)strlen(result->meanings[0].text) > max_len) max_len = strlen(result->meanings[0].text);

            int box_width = max_len + 20;
            if (box_width < 50) box_width = 50;
            if (box_width > 70) box_width = 70;

            int title_len = 11;  // strlen("DETAIL KATA")
            int pad_left = (box_width - title_len) / 2;
            int pad_right = box_width - title_len - pad_left;
            int label_w = 14;

            printf("\n  +");
            for (int i = 0; i < box_width; i++) printf("-");
            printf("+\n");
            printf("  |%*sDETAIL KATA%*s|\n", pad_left, "", pad_right, "");
            printf("  +");
            for (int i = 0; i < box_width; i++) printf("-");
            printf("+\n");
            printf("  | %-*s : %s\n", label_w - 2, "Indonesian", result->indonesian);
            printf("  | %-*s : %s\n", label_w - 2, "English", result->english);
            printf("  | %-*s : %s\n", label_w - 2, "POS", pos_to_string(result->pos));
            printf("  | %-*s : %s\n", label_w - 2, "Category", category_to_string(result->category));
            printf("  | %-*s : %s\n", label_w - 2, "Pronounce", result->pronunciation);
            printf("  +");
            for (int i = 0; i < box_width; i++) printf("-");
            printf("+\n");
            printf("  | %-*s:\n", label_w - 2, "Definition(s)");
            for (int i = 0; i < result->meaning_count; i++) {
                printf("  | %*d. %s\n", label_w - 3, i + 1, result->meanings[i].text);
            }
            printf("  +");
            for (int i = 0; i < box_width; i++) printf("-");
            printf("+\n");
            printf("  | %-*s : %s\n", label_w - 2, "Example", result->example);
            printf("  +");
            for (int i = 0; i < box_width; i++) printf("-");
            printf("+\n");
        } else {
            printf("\n  [Tidak Ditemukan] Kata '%s' tidak ada dalam dictionary.\n", input);
            printf("  Gunakan prefix search (menu 4) untuk autocomplete.\n");
        }

        printf("\n  +----------------------------------------------------------+\n");
        printf("  |     [Enter] Cari kata lagi    |  [<] Kembali ke Menu Utama |\n");
        printf("  +----------------------------------------------------------+\n");
        printf("\n  Pilihan: ");

        if (fgets(choice, sizeof(choice), stdin) == NULL) {
            return;
        }
        choice[strcspn(choice, "\n")] = '\0';

        if (strlen(choice) == 0) {
            continue;  // Cari kata lagi
        }

        if (choice[0] == '<') {
            // Bersihkan layar dan langsung kembali
            break;
        }

        printf("\n  [Info] Pilihan tidak valid.\n");
    }
}

// Handler pencarian kata Inggris (Hash Table)
static void handle_search_english(DictionaryManager* dict) {
    char input[MAX_WORD_LEN];
    char choice[20];

    while (1) {
        // Cek apakah ada kata terakhir yang disimpan
        WordEntry* saved_last = dict_get_last_searched(dict);

        printf("\n");
        printf("  +----------------------------------------------------------+\n");
        printf("  |              CARI KATA INGGRIS (Hash Table)              |\n");
        printf("  +----------------------------------------------------------+\n");
        if (saved_last != NULL) {
            printf("  |  Kata terakhir: %-36s |\n", saved_last->indonesian);
        } else {
            printf("  |  Pencarian berdasarkan terjemahan Inggris          |\n");
        }
        printf("  +----------------------------------------------------------+\n");

        if (get_string_input("Masukkan kata Inggris", input, MAX_WORD_LEN) == NULL) {
            return;
        }

        if (strlen(input) == 0) {
            if (saved_last != NULL) {
                printf("  [Info] Kata terakhir: %s\n", saved_last->indonesian);
            } else {
                printf("  [Error] Masukkan kata yang ingin dicari.\n");
            }
            continue;
        }

        printf("\n  Mencari...\n");

        WordEntry* result = dict_search_english(dict, input);

        if (result != NULL) {
            // Simpan sebagai kata terakhir
            dict_set_last_searched(dict, *result);

            // Hitung lebar box
            int max_len = strlen(result->indonesian);
            if ((int)strlen(result->english) > max_len) max_len = strlen(result->english);
            if ((int)strlen(result->example) > max_len) max_len = strlen(result->example);
            if ((int)strlen(result->pronunciation) > max_len) max_len = strlen(result->pronunciation);
            if ((int)strlen(result->meanings[0].text) > max_len) max_len = strlen(result->meanings[0].text);

            int box_width = max_len + 20;
            if (box_width < 50) box_width = 50;
            if (box_width > 70) box_width = 70;

            int title_len = 11;  // strlen("DETAIL KATA")
            int pad_left = (box_width - title_len) / 2;
            int pad_right = box_width - title_len - pad_left;
            int label_w = 14;

            printf("\n  +");
            for (int i = 0; i < box_width; i++) printf("-");
            printf("+\n");
            printf("  |%*sDETAIL KATA%*s|\n", pad_left, "", pad_right, "");
            printf("  +");
            for (int i = 0; i < box_width; i++) printf("-");
            printf("+\n");
            printf("  | %-*s : %s\n", label_w - 2, "Indonesian", result->indonesian);
            printf("  | %-*s : %s\n", label_w - 2, "English", result->english);
            printf("  | %-*s : %s\n", label_w - 2, "POS", pos_to_string(result->pos));
            printf("  | %-*s : %s\n", label_w - 2, "Category", category_to_string(result->category));
            printf("  | %-*s : %s\n", label_w - 2, "Pronounce", result->pronunciation);
            printf("  +");
            for (int i = 0; i < box_width; i++) printf("-");
            printf("+\n");
            printf("  | %-*s:\n", label_w - 2, "Definition(s)");
            for (int i = 0; i < result->meaning_count; i++) {
                printf("  | %*d. %s\n", label_w - 3, i + 1, result->meanings[i].text);
            }
            printf("  +");
            for (int i = 0; i < box_width; i++) printf("-");
            printf("+\n");
            printf("  | %-*s : %s\n", label_w - 2, "Example", result->example);
            printf("  +");
            for (int i = 0; i < box_width; i++) printf("-");
            printf("+\n");
        } else {
            printf("\n  [Tidak Ditemukan] Kata '%s' tidak ada dalam dictionary.\n", input);
        }

        printf("\n  +----------------------------------------------------------+\n");
        printf("  |     [Enter] Cari kata lagi    |  [<] Kembali ke Menu Utama |\n");
        printf("  +----------------------------------------------------------+\n");
        printf("\n  Pilihan: ");

        if (fgets(choice, sizeof(choice), stdin) == NULL) {
            return;
        }
        choice[strcspn(choice, "\n")] = '\0';

        if (strlen(choice) == 0) {
            continue;  // Cari kata lagi
        }

        if (choice[0] == '<') {
            // Bersihkan layar dan langsung kembali
            break;
        }

        printf("\n  [Info] Pilihan tidak valid.\n");
    }
}

// Handler pencarian prefix (autocomplete via Trie)
static void handle_prefix_search(DictionaryManager* dict) {
    char input[MAX_WORD_LEN];
    char nav_input[20];
    int count = 0;

    printf("\n");
    printf("  +----------------------------------------------------------+\n");
    printf("  |              PREFIX SEARCH (Autocomplete - Trie)        |\n");
    printf("  +----------------------------------------------------------+\n");

    if (get_string_input("Masukkan awalan kata", input, MAX_WORD_LEN) == NULL) {
        return;
    }

    if (strlen(input) == 0) {
        printf("  [Error] Masukkan awalan kata.\n");
        return;
    }

    // Convert input to lowercase for prefix matching
    for (int i = 0; input[i]; i++) {
        input[i] = tolower(input[i]);
    }

    printf("\n  Mencari kata yang diawali '%s'...\n", input);

    WordEntry* all_results = dict_search_prefix(dict, input, &count);

    if (all_results == NULL || count == 0) {
        printf("\n  [Tidak Ditemukan] Tidak ada kata yang diawali '%s'.\n", input);
        return;
    }

    printf("\n  [Ditemukan] %d kata ditemukan!\n", count);

    // Pagination settings
    int words_per_page = 20;
    int current_page = 1;
    int total_pages = (count + words_per_page - 1) / words_per_page;
    if (total_pages < 1) total_pages = 1;

    while (1) {
        int start_idx = (current_page - 1) * words_per_page;
        int end_idx = start_idx + words_per_page;
        if (end_idx > count) end_idx = count;

        // Tampilkan header
        printf("\n");
        printf("  +----------------------------------------------------------+\n");
        printf("  |              PREFIX SEARCH (Autocomplete - Trie)        |\n");
        printf("  +----------------------------------------------------------+\n");
        printf("  |  Prefix: %-50s |\n", input);
        printf("  |  Halaman %d/%d (%d kata)                                  |\n", current_page, total_pages, count);
        printf("  +----------------------------------------------------------+\n");

        // Tampilkan tabel
        printf("\n  %-5s | %-25s | %-25s\n", "No.", "Indonesian", "English");
        printf("  ");
        for (int i = 0; i < 65; i++) printf("-");
        printf("\n");

        for (int i = start_idx; i < end_idx; i++) {
            printf("  %-5d | %-25s | %-25s\n", i + 1, all_results[i].indonesian, all_results[i].english);
        }

        printf("\n  +----------------------------------------------------------+\n");
        printf("  |     [1-%d] Pilih Kata   |  [/1-%d] Pindah Halaman        |\n", count, total_pages);
        printf("  |     [<] Kembali ke Menu Utama                            |\n");
        printf("  +----------------------------------------------------------+\n");
        printf("\n  Pilihan: ");

        if (fgets(nav_input, sizeof(nav_input), stdin) == NULL) {
            free(all_results);
            return;
        }

        nav_input[strcspn(nav_input, "\n")] = '\0';

        if (strlen(nav_input) == 0) {
            continue;
        }

        char first_char = nav_input[0];

        // Kembali ke menu utama
        if (first_char == '<') {
            printf("\n  [Info] Kembali ke menu utama.\n");
            break;
        }

        // Navigasi halaman dengan /1, /2, /3, dst
        if (first_char == '/') {
            int page_num = atoi(nav_input + 1);
            if (page_num >= 1 && page_num <= total_pages) {
                current_page = page_num;
            } else {
                printf("\n  [Error] Halaman tidak valid. Pilih 1-%d\n", total_pages);
            }
            continue;
        }

        // Pilih kata dengan nomor
        if (first_char >= '1' && first_char <= '9') {
            int word_num = atoi(nav_input);
            if (word_num >= 1 && word_num <= count) {
                WordEntry* word = &all_results[word_num - 1];

                // Tambahkan ke riwayat pencarian
                dict_add_to_history(dict, *word);

                // Hitung lebar box
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

                // Kembalikan ke halaman yang sesuai
                current_page = (word_num - 1) / words_per_page + 1;
            } else {
                printf("\n  [Error] Nomor tidak valid. Pilih 1-%d\n", count);
            }
            continue;
        }

        printf("\n  [Info] Pilihan tidak valid.\n");
    }

    free(all_results);
}

// ============================================================================
// HANDLER CRUD
// ============================================================================

// Handler menambah kata baru
static void handle_add_word(DictionaryManager* dict) {
    char indonesian[MAX_WORD_LEN];
    char english[MAX_WORD_LEN];
    char definition[MAX_DEF_LEN];
    char example[MAX_EXAMPLE_LEN];
    char pronunciation[MAX_WORD_LEN];

    printf("\n");
    printf("  +----------------------------------------------------------+\n");
    printf("  |                    TAMBAH KATA BARU                      |\n");
    printf("  +----------------------------------------------------------+\n");

    if (get_string_input("Kata Indonesia", indonesian, MAX_WORD_LEN) == NULL) return;
    if (strlen(indonesian) == 0) {
        printf("  [Error] Kata Indonesia tidak boleh kosong.\n");
        return;
    }

    // Check if word already exists
    if (dict_search_indonesian(dict, indonesian) != NULL) {
        printf("\n  [Info] Kata '%s' sudah ada dalam dictionary!\n", indonesian);
        printf("  Silakan input kata lain atau kembali ke menu utama.\n");
        printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
        printf("  Pilihan: ");
        char input[20];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // User pressed EOF, continue anyway
        }
        return;
    }

    if (get_string_input("Terjemahan Inggris", english, MAX_WORD_LEN) == NULL) return;
    if (strlen(english) == 0) {
        printf("  [Error] Terjemahan Inggris tidak boleh kosong.\n");
        return;
    }

    PartOfSpeech pos = get_pos_from_input();

    if (get_string_input("Definisi", definition, MAX_DEF_LEN) == NULL) return;
    if (strlen(definition) == 0) {
        printf("  [Warning] Definisi kosong.\n");
    }

    printf("  Contoh kalimat (opsional, Enter untuk skip): ");
    if (fgets(example, MAX_EXAMPLE_LEN, stdin) != NULL) {
        example[strcspn(example, "\n")] = '\0';
    }

    printf("  Panduan pengucapan (opsional, Enter untuk skip): ");
    if (fgets(pronunciation, MAX_WORD_LEN, stdin) != NULL) {
        pronunciation[strcspn(pronunciation, "\n")] = '\0';
    }

    WordCategory category = get_category_from_input();

    WordEntry word = create_word_entry(indonesian, english, pos, definition,
                                        example, pronunciation, category);

    if (dict_add_word(dict, word)) {
        printf("\n  [Sukses] Kata '%s' berhasil ditambahkan!\n", indonesian);
        printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
        printf("  Pilihan: ");
        char input[20];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // User pressed EOF, continue anyway
        }
    }
}

// Handler mengupdate kata
static void handle_update_word(DictionaryManager* dict) {
    char indonesian[MAX_WORD_LEN];
    char input[20];

    printf("\n");
    printf("  +----------------------------------------------------------+\n");
    printf("  |                        UPDATE KATA                       |\n");
    printf("  +----------------------------------------------------------+\n");

    if (get_string_input("Kata Indonesia yang akan diupdate", indonesian, MAX_WORD_LEN) == NULL) return;
    if (strlen(indonesian) == 0) {
        printf("  [Error] Masukkan kata yang akan diupdate.\n");
        return;
    }

    WordEntry* existing = dict_search_indonesian(dict, indonesian);
    if (existing == NULL) {
        printf("\n  [Error] Kata '%s' tidak ditemukan!\n", indonesian);
        printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
        printf("  Pilihan: ");
        char input[20];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // User pressed EOF, continue anyway
        }
        return;
    }

    // Loop untuk update multiple fields
    while (1) {
        printf("\n");
        printf("  +----------------------------------------------------------+\n");
        printf("  |                    UPDATE: %-28s |\n", indonesian);
        printf("  +----------------------------------------------------------+\n");
        printf("  |  1. Indonesian   : %-35s |\n", existing->indonesian);
        printf("  |  2. English      : %-35s |\n", existing->english);
        printf("  |  3. POS          : %-35s |\n", pos_to_string(existing->pos));
        printf("  |  4. Category     : %-35s |\n", category_to_string(existing->category));
        printf("  |  5. Pronounce    : %-35s |\n", existing->pronunciation);
        printf("  |  6. Definition   : %-35s |\n", existing->meanings[0].text);
        printf("  |  7. Example      : %-35s |\n", existing->example);
        printf("  +----------------------------------------------------------+\n");
        printf("  |                                                          |\n");
        printf("  |  [1-7] Pilih field yang akan diubah                       |\n");
        printf("  |  [0]   Simpan dan keluar                                  |\n");
        printf("  |                                                          |\n");
        printf("  +----------------------------------------------------------+\n");
        printf("\n  Pilihan: ");

        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) continue;

        char first_char = input[0];

        if (first_char == '0') {
            printf("\n  [Success] Update kata '%s' disimpan!\n", indonesian);
            break;
        }

        if (first_char == '1') {
            // Update Indonesian
            char new_indonesian[MAX_WORD_LEN];
            printf("\n  Indonesian saat ini: %s\n", existing->indonesian);
            printf("  Masukkan Indonesian baru: ");
            if (fgets(new_indonesian, MAX_WORD_LEN, stdin) != NULL) {
                new_indonesian[strcspn(new_indonesian, "\n")] = '\0';
                if (strcmp(new_indonesian, "-") == 0) {
                    // Kosongkan field
                    existing->indonesian[0] = '\0';
                    printf("  [Success] Indonesian berhasil dikosongkan!\n");
                } else if (strlen(new_indonesian) > 0) {
                    char old_indonesian[MAX_WORD_LEN];
                    strncpy(old_indonesian, indonesian, MAX_WORD_LEN - 1);
                    strncpy(existing->indonesian, new_indonesian, MAX_WORD_LEN - 1);
                    existing->indonesian[MAX_WORD_LEN - 1] = '\0';
                    strncpy(indonesian, new_indonesian, MAX_WORD_LEN - 1);
                    indonesian[MAX_WORD_LEN - 1] = '\0';
                    // Update key in BST
                    bst_update_key(dict->bst_root, old_indonesian, new_indonesian);
                    printf("  [Success] Indonesian berhasil diubah!\n");
                } else {
                    printf("  [Info] Indonesian tidak diubah.\n");
                }
            }
        }
        else if (first_char == '2') {
            // Update English
            char new_english[MAX_WORD_LEN];
            printf("\n  English saat ini: %s\n", existing->english);
            printf("  Masukkan English baru: ");
            if (fgets(new_english, MAX_WORD_LEN, stdin) != NULL) {
                new_english[strcspn(new_english, "\n")] = '\0';
                if (strcmp(new_english, "-") == 0) {
                    // Kosongkan field
                    existing->english[0] = '\0';
                    printf("  [Success] English berhasil dikosongkan!\n");
                } else if (strlen(new_english) > 0) {
                    strncpy(existing->english, new_english, MAX_WORD_LEN - 1);
                    existing->english[MAX_WORD_LEN - 1] = '\0';
                    // Update hash table
                    hash_remove(dict->hash_english, existing->english);
                    hash_insert(dict->hash_english, *existing);
                    printf("  [Success] English berhasil diubah!\n");
                } else {
                    printf("  [Info] English tidak diubah.\n");
                }
            }
        }
        else if (first_char == '3') {
            // Update POS
            printf("\n  POS saat ini: %s\n", pos_to_string(existing->pos));
            printf("  Masukkan POS baru:\n");
            printf("    1. Noun (Kata Benda)\n");
            printf("    2. Verb (Kata Kerja)\n");
            printf("    3. Adjective (Kata Sifat)\n");
            printf("    4. Adverb (Kata Keterangan)\n");
            printf("    5. Preposition (Kata Depan)\n");
            printf("    6. Conjunction (Kata Hubung)\n");
            printf("    7. Pronoun (Kata Ganti)\n");
            printf("    8. Interjection (Kata Seru)\n");
            printf("    9. Number (Angka)\n");
            printf("\n  Pilihan (1-9): ");
            if (fgets(input, sizeof(input), stdin) != NULL) {
                int choice = atoi(input);
                if (choice >= 1 && choice <= 9) {
                    PartOfSpeech pos_array[] = {NOUN, VERB, ADJECTIVE, ADVERB,
                                                 PREPOSITION, CONJUNCTION, PRONOUN,
                                                 INTERJECTION, DETERMINER};
                    existing->pos = pos_array[choice - 1];
                    printf("  [Success] POS berhasil diubah!\n");
                } else {
                    printf("  [Error] Pilihan tidak valid.\n");
                }
            }
        }
        else if (first_char == '4') {
            // Update Category
            printf("\n  Category saat ini: %s\n", category_to_string(existing->category));
            printf("  Masukkan Category baru:\n");
            printf("    1. Everyday\n");
            printf("    2. Formal\n");
            printf("    3. Slang\n");
            printf("    4. Technical\n");
            printf("\n  Pilihan (1-4): ");
            if (fgets(input, sizeof(input), stdin) != NULL) {
                int choice = atoi(input);
                if (choice >= 1 && choice <= 4) {
                    WordCategory cat_array[] = {EVERYDAY, FORMAL, SLANG, TECHNICAL};
                    existing->category = cat_array[choice - 1];
                    printf("  [Success] Category berhasil diubah!\n");
                } else {
                    printf("  [Error] Pilihan tidak valid.\n");
                }
            }
        }
        else if (first_char == '5') {
            // Update Pronounce
            char new_pronunciation[MAX_WORD_LEN];
            printf("\n  Pronounce saat ini: %s\n", existing->pronunciation);
            printf("  Masukkan Pronounce baru: ");
            if (fgets(new_pronunciation, MAX_WORD_LEN, stdin) != NULL) {
                new_pronunciation[strcspn(new_pronunciation, "\n")] = '\0';
                if (strcmp(new_pronunciation, "-") == 0) {
                    // Kosongkan field
                    existing->pronunciation[0] = '\0';
                    printf("  [Success] Pronounce berhasil dikosongkan!\n");
                } else if (strlen(new_pronunciation) > 0) {
                    strncpy(existing->pronunciation, new_pronunciation, MAX_WORD_LEN - 1);
                    existing->pronunciation[MAX_WORD_LEN - 1] = '\0';
                    printf("  [Success] Pronounce berhasil diubah!\n");
                } else {
                    printf("  [Info] Pronounce tidak diubah.\n");
                }
            }
        }
        else if (first_char == '6') {
            // Update Definition
            char new_definition[MAX_DEF_LEN];
            printf("\n  Definition saat ini: %s\n", existing->meanings[0].text);
            printf("  Masukkan Definition baru: ");
            if (fgets(new_definition, MAX_DEF_LEN, stdin) != NULL) {
                new_definition[strcspn(new_definition, "\n")] = '\0';
                if (strcmp(new_definition, "-") == 0) {
                    // Kosongkan field
                    existing->meanings[0].text[0] = '\0';
                    printf("  [Success] Definition berhasil dikosongkan!\n");
                } else if (strlen(new_definition) > 0) {
                    strncpy(existing->meanings[0].text, new_definition, MAX_DEF_LEN - 1);
                    existing->meanings[0].text[MAX_DEF_LEN - 1] = '\0';
                    printf("  [Success] Definition berhasil diubah!\n");
                } else {
                    printf("  [Info] Definition tidak diubah.\n");
                }
            }
        }
        else if (first_char == '7') {
            // Update Example
            char new_example[MAX_EXAMPLE_LEN];
            printf("\n  Example saat ini: %s\n", existing->example);
            printf("  Masukkan Example baru: ");
            if (fgets(new_example, MAX_EXAMPLE_LEN, stdin) != NULL) {
                new_example[strcspn(new_example, "\n")] = '\0';
                if (strcmp(new_example, "-") == 0) {
                    // Kosongkan field
                    existing->example[0] = '\0';
                    printf("  [Success] Example berhasil dikosongkan!\n");
                } else if (strlen(new_example) > 0) {
                    strncpy(existing->example, new_example, MAX_EXAMPLE_LEN - 1);
                    existing->example[MAX_EXAMPLE_LEN - 1] = '\0';
                    printf("  [Success] Example berhasil diubah!\n");
                } else {
                    printf("  [Info] Example tidak diubah.\n");
                }
            }
        }
        else {
            printf("\n  [Error] Pilihan tidak valid. Pilih 1-7 atau 0 untuk keluar.\n");
        }
    }
}

// Handler menghapus kata
static void handle_delete_word(DictionaryManager* dict) {
    char indonesian[MAX_WORD_LEN];
    char confirm;

    printf("\n");
    printf("  +----------------------------------------------------------+\n");
    printf("  |                        HAPUS KATA                        |\n");
    printf("  +----------------------------------------------------------+\n");

    if (get_string_input("Kata Indonesia yang akan dihapus", indonesian, MAX_WORD_LEN) == NULL) return;
    if (strlen(indonesian) == 0) {
        printf("  [Error] Masukkan kata yang akan dihapus.\n");
        return;
    }

    WordEntry* existing = dict_search_indonesian(dict, indonesian);
    if (existing == NULL) {
        printf("\n  [Error] Kata '%s' tidak ditemukan!\n", indonesian);
        printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
        printf("  Pilihan: ");
        char input[20];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // User pressed EOF, continue anyway
        }
        return;
    }

    printf("\n  Kata yang akan dihapus:\n");
    print_word_entry(existing);

    printf("\n  Yakin ingin menghapus kata ini? (y/n): ");
    scanf(" %c", &confirm);
    clear_input_buffer();

    if (tolower(confirm) == 'y') {
        dict_delete_word(dict, indonesian);
        printf("\n  [Success] Kata '%s' berhasil dihapus!\n", indonesian);
    } else {
        printf("\n  [Dibatalkan] Penghapusan dibatalkan.\n");
    }

    printf("\n  [Tekan Enter untuk kembali ke menu utama]\n");
    printf("  Pilihan: ");
    char input[20];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        // User pressed EOF, continue anyway
    }
}