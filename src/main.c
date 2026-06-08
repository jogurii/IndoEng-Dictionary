#include "data_structures.h"
#include "dictionary.h"
#include "ui_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

// ============================================================================
// Deklarasi static
// ============================================================================

static void display_main_menu(void);
static void clear_input_buffer(void);
static void handle_search_indonesian(DictionaryManager *dict);
static void handle_search_english(DictionaryManager *dict);
static void handle_prefix_search(DictionaryManager *dict);
static void handle_add_word(DictionaryManager *dict);
static void handle_update_word(DictionaryManager *dict);
static void handle_delete_word(DictionaryManager *dict);
static PartOfSpeech get_pos_from_input(void);
static WordCategory get_category_from_input(void);
static char *get_string_input(char *prompt, char *buffer, int max_len);

// ============================================================================
// Fungsi utama
// ============================================================================

// Tampilkan yang terakhir dicari
static void print_last_searched_if_exists(DictionaryManager *dict) {
  WordEntry *last = dict_get_last_searched(dict);
  if (last != NULL) {
    printf("\n");
    ui_display_word_box(last, "KATA TERAKHIR");
    printf("\n");
  }
}

int main(void) {
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut != INVALID_HANDLE_VALUE) {
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
      dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(hOut, dwMode);
    }
  }
#endif

  printf("\n");
  printf(CLR_CYAN
         "  ============================================================\n");
  printf("  =                                                          =\n");
  printf("  =            " CLR_BOLD CLR_YELLOW
         "INDONESIAN - ENGLISH DICTIONARY" CLR_RESET CLR_CYAN
         "               =\n");
  printf("  =                    (" CLR_GREEN "IndoEng v1.0" CLR_RESET CLR_CYAN
         ")                        =\n");
  printf("  =                                                          =\n");
  printf("  "
         "============================================================"
         "\n" CLR_RESET);
  printf("  " CLR_WHITE "Loading dictionary data..." CLR_RESET "\n\n");

  srand((unsigned int)time(NULL));

  DictionaryManager *dict = dict_create();
  if (dict == NULL) {
    fprintf(stderr, CLR_RED "  [Error] Gagal membuat dictionary!\n" CLR_RESET);
    return 1;
  }

  dict_load_initial_data(dict);

  printf("  " CLR_GREEN "Dictionary loaded successfully!" CLR_RESET "\n");
  printf("  Total words: " CLR_BOLD CLR_YELLOW "%d" CLR_RESET "\n\n",
         dict->total_words);

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
    printf("\n  Masukkan pilihan: " CLR_BOLD CLR_YELLOW);

    if (scanf("%d", &choice) != 1) {
      printf(CLR_RESET);
      clear_input_buffer();
      printf("\n" CLR_RED
             "  [Error] Input tidak valid. Masukkan angka.\n" CLR_RESET);
      printf("  Tekan Enter untuk melanjutkan...");
      getchar();
      continue;
    }
    printf(CLR_RESET);
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
      printf(
          CLR_CYAN
          "  ============================================================\n");
      printf("  =                     " CLR_BOLD CLR_YELLOW
             "TERIMA KASIH!" CLR_RESET CLR_CYAN "                        =\n");
      printf("  =          " CLR_GREEN
             "Terima kasih sudah menggunakan IndoEng" CLR_RESET CLR_CYAN
             "            =\n");
      printf("  "
             "============================================================"
             "\n\n" CLR_RESET);
      break;
    default:
      printf("\n" CLR_RED "  [Error] Pilihan tidak valid.\n" CLR_RESET);
      break;
    }

    // Loop continues - screen will be cleared at start of next iteration
  }

  dict_destroy(dict);

  return 0;
}

// ============================================================================
// Tampilan menu
// ============================================================================

// Tampilkan menu utama
static void display_main_menu(void) {
  printf("\n");
  printf(CLR_CYAN
         "  +----------------------------------------------------------+\n");
  printf("  |              " CLR_BOLD CLR_YELLOW
         "INDONESIAN - ENGLISH DICTIONARY" CLR_RESET CLR_CYAN
         "             |\n");
  printf("  |                       (" CLR_GREEN "IndoEng" CLR_RESET CLR_CYAN
         ")                          |\n");
  printf("  +----------------------------------------------------------+\n");
  printf("  |                                                          |\n");
  printf("  |  " CLR_BOLD CLR_YELLOW "1." CLR_RESET
         " View All Words                - " CLR_WHITE
         "Paginated A-Z" CLR_RESET CLR_CYAN "        |\n");
  printf("  |  " CLR_BOLD CLR_YELLOW "2." CLR_RESET
         " Search Word (Indonesian)      - " CLR_WHITE
         "BST Lookup" CLR_RESET CLR_CYAN "           |\n");
  printf("  |  " CLR_BOLD CLR_YELLOW "3." CLR_RESET
         " Search Word (English)         - " CLR_WHITE
         "Hash Table Lookup" CLR_RESET CLR_CYAN "    |\n");
  printf("  |  " CLR_BOLD CLR_YELLOW "4." CLR_RESET
         " Prefix Search (Autocomplete)  - " CLR_WHITE
         "Trie Lookup" CLR_RESET CLR_CYAN "          |\n");
  printf("  |  " CLR_BOLD CLR_YELLOW "5." CLR_RESET
         " Add New Word                  - " CLR_WHITE
         "Create" CLR_RESET CLR_CYAN "               |\n");
  printf("  |  " CLR_BOLD CLR_YELLOW "6." CLR_RESET
         " Update Word                   - " CLR_WHITE
         "Update" CLR_RESET CLR_CYAN "               |\n");
  printf("  |  " CLR_BOLD CLR_YELLOW "7." CLR_RESET
         " Delete Word                   - " CLR_WHITE
         "Delete" CLR_RESET CLR_CYAN "               |\n");
  printf("  |  " CLR_BOLD CLR_YELLOW "8." CLR_RESET
         " Word of the Day               - " CLR_WHITE
         "Random Pick" CLR_RESET CLR_CYAN "          |\n");
  printf("  |  " CLR_BOLD CLR_YELLOW "9." CLR_RESET
         " Search History                - " CLR_WHITE
         "Recent Searches" CLR_RESET CLR_CYAN "      |\n");
  printf("  | " CLR_BOLD CLR_YELLOW "10." CLR_RESET
         " Undo Last Action              - " CLR_WHITE
         "Stack-based" CLR_RESET CLR_CYAN "          |\n");
  printf("  | " CLR_BOLD CLR_YELLOW "11." CLR_RESET
         " Statistics                    - " CLR_WHITE
         "Dictionary Stats" CLR_RESET CLR_CYAN "     |\n");
  printf("  |                                                          |\n");
  printf("  |  " CLR_BOLD CLR_YELLOW "0." CLR_RESET
         " Exit                                                 |\n");
  printf("  |                                                          |\n");
  printf("  "
         "+----------------------------------------------------------+"
         "\n" CLR_RESET);
}

// ============================================================================
// Fungsi input
// ============================================================================

// Bersihkan buffer input
static void clear_input_buffer(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

// Dapatkan input string
static char *get_string_input(char *prompt, char *buffer, int max_len) {
  printf("  %s: ", prompt);
  if (fgets(buffer, max_len, stdin) != NULL) {
    buffer[strcspn(buffer, "\n")] = '\0';
    int i = 0;
    while (isspace(buffer[i]))
      i++;
    if (i > 0) {
      memmove(buffer, buffer + i, strlen(buffer) - i + 1);
    }
    return buffer;
  }
  return NULL;
}

// Dapatkan input POS
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
  case 0:
    return NOUN;
  case 1:
    return VERB;
  case 2:
    return ADJECTIVE;
  case 3:
    return ADVERB;
  case 4:
    return PRONOUN;
  case 5:
    return PREPOSITION;
  case 6:
    return CONJUNCTION;
  case 7:
    return INTERJECTION;
  case 8:
    return DETERMINER;
  default:
    return NOUN;
  }
}

// Dapatkan input kategori
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
  case 0:
    return EVERYDAY;
  case 1:
    return FORMAL;
  case 2:
    return SLANG;
  case 3:
    return TECHNICAL;
  default:
    return EVERYDAY;
  }
}

// ============================================================================
// Handler pencarian
// ============================================================================

// Handler pencarian Indonesia
static void handle_search_indonesian(DictionaryManager *dict) {
  char input[MAX_WORD_LEN];
  char choice[20];

  while (1) {
    // Cek apakah ada kata terakhir yang disimpan
    WordEntry *saved_last = dict_get_last_searched(dict);

    printf("\n");
    printf(CLR_CYAN
           "  +----------------------------------------------------------+\n");
    printf("  |              CARI KATA INDONESIA (BST)                   |\n");
    printf("  +----------------------------------------------------------+\n");
    if (saved_last != NULL) {
      printf("  |  Kata terakhir: " CLR_BOLD CLR_YELLOW
             "%-36s" CLR_RESET CLR_CYAN "     |\n",
             saved_last->indonesian);
    } else {
      printf("  |           Pencarian berdasarkan kata Indonesia           |\n");
    }
    printf("  "
           "+----------------------------------------------------------+"
           "\n" CLR_RESET);

    if (get_string_input("Masukkan kata Indonesia", input, MAX_WORD_LEN) ==
        NULL) {
      return;
    }

    if (strlen(input) == 0) {
      if (saved_last != NULL) {
        printf("  " CLR_CYAN "[Info]" CLR_RESET " Kata terakhir: " CLR_GREEN
               "%s" CLR_RESET "\n",
               saved_last->indonesian);
      } else {
        printf("  " CLR_RED "[Error]" CLR_RESET
               " Masukkan kata yang ingin dicari.\n");
      }
      continue;
    }

    printf("\n  Mencari...\n");

    WordEntry *result = dict_search_indonesian(dict, input);

    if (result != NULL) {
      // Simpan sebagai kata terakhir
      dict_set_last_searched(dict, *result);

      // Tampilkan kata dalam box terformat
      printf("\n");
      ui_display_word_box(result, "DETAIL KATA");
    } else {
      printf("\n  " CLR_YELLOW "[Tidak Ditemukan]" CLR_RESET
             " Kata '" CLR_BOLD CLR_RED "%s" CLR_RESET
             "' tidak ada dalam dictionary.\n",
             input);
      printf("  Gunakan prefix search (menu 4) untuk autocomplete.\n");
    }

    printf("\n");
    printf(CLR_CYAN
           "  +----------------------------------------------------------+\n");
    printf(
        "  |     [Enter] Cari kata lagi  |  [<] Kembali ke Menu Utama |\n");
    printf("  "
           "+----------------------------------------------------------+"
           "\n" CLR_RESET);
    printf("\n  Pilihan: " CLR_BOLD CLR_YELLOW);

    if (fgets(choice, sizeof(choice), stdin) == NULL) {
      printf(CLR_RESET);
      return;
    }
    printf(CLR_RESET);
    choice[strcspn(choice, "\n")] = '\0';

    if (strlen(choice) == 0) {
      continue; // Cari kata lagi
    }

    if (choice[0] == '<') {
      // Bersihkan layar dan langsung kembali
      break;
    }

    printf("\n  " CLR_CYAN "[Info]" CLR_RESET " Pilihan tidak valid.\n");
  }
}

// Handler pencarian Inggris
static void handle_search_english(DictionaryManager *dict) {
  char input[MAX_WORD_LEN];
  char choice[20];

  while (1) {
    // Cek apakah ada kata terakhir yang disimpan
    WordEntry *saved_last = dict_get_last_searched(dict);

    printf("\n");
    printf(CLR_CYAN
           "  +----------------------------------------------------------+\n");
    printf("  |              CARI KATA INGGRIS (Hash Table)              |\n");
    printf("  +----------------------------------------------------------+\n");
    if (saved_last != NULL) {
      printf("  |  Kata terakhir: " CLR_BOLD CLR_YELLOW
             "%-36s" CLR_RESET CLR_CYAN "     |\n",
             saved_last->indonesian);
    } else {
      printf("  |         Pencarian berdasarkan terjemahan Inggris         |\n");
    }
    printf("  "
           "+----------------------------------------------------------+"
           "\n" CLR_RESET);

    if (get_string_input("Masukkan kata Inggris", input, MAX_WORD_LEN) ==
        NULL) {
      return;
    }

    if (strlen(input) == 0) {
      if (saved_last != NULL) {
        printf("  " CLR_CYAN "[Info]" CLR_RESET " Kata terakhir: " CLR_GREEN
               "%s" CLR_RESET "\n",
               saved_last->indonesian);
      } else {
        printf("  " CLR_RED "[Error]" CLR_RESET
               " Masukkan kata yang ingin dicari.\n");
      }
      continue;
    }

    printf("\n  Mencari...\n");

    WordEntry *result = dict_search_english(dict, input);

    if (result != NULL) {
      // Simpan sebagai kata terakhir
      dict_set_last_searched(dict, *result);

      // Tampilkan kata dalam box terformat
      printf("\n");
      ui_display_word_box(result, "DETAIL KATA");
    } else {
      printf("\n  " CLR_YELLOW "[Tidak Ditemukan]" CLR_RESET
             " Kata '" CLR_BOLD CLR_RED "%s" CLR_RESET
             "' tidak ada dalam dictionary.\n",
             input);
    }

    printf("\n");
    printf(CLR_CYAN
           "  +----------------------------------------------------------+\n");
    printf(
        "  |     [Enter] Cari kata lagi  |  [<] Kembali ke Menu Utama |\n");
    printf("  "
           "+----------------------------------------------------------+"
           "\n" CLR_RESET);
    printf("\n  Pilihan: " CLR_BOLD CLR_YELLOW);

    if (fgets(choice, sizeof(choice), stdin) == NULL) {
      printf(CLR_RESET);
      return;
    }
    printf(CLR_RESET);
    choice[strcspn(choice, "\n")] = '\0';

    if (strlen(choice) == 0) {
      continue; // Cari kata lagi
    }

    if (choice[0] == '<') {
      // Bersihkan layar dan langsung kembali
      break;
    }

    printf("\n  [Info] Pilihan tidak valid.\n");
  }
}

// Handler pencarian prefix
static void handle_prefix_search(DictionaryManager *dict) {
  char input[MAX_WORD_LEN];
  char nav_input[20];
  int count = 0;

  printf("\n");
  printf(CLR_CYAN
         "  +----------------------------------------------------------+\n");
  printf("  |             PREFIX SEARCH (Autocomplete - Trie)          |\n");
  printf("  "
         "+----------------------------------------------------------+"
         "\n" CLR_RESET);

  if (get_string_input("Masukkan awalan kata", input, MAX_WORD_LEN) == NULL) {
    return;
  }

  if (strlen(input) == 0) {
    printf("  " CLR_RED "[Error]" CLR_RESET " Masukkan awalan kata.\n");
    return;
  }

  // Convert input to lowercase for prefix matching
  for (int i = 0; input[i]; i++) {
    input[i] = tolower(input[i]);
  }

  printf("\n  Mencari kata yang diawali '%s'...\n", input);

  WordEntry *all_results = dict_search_prefix(dict, input, &count);

  if (all_results == NULL || count == 0) {
    printf("\n  " CLR_YELLOW "[Tidak Ditemukan]" CLR_RESET
           " Tidak ada kata yang diawali '%s'.\n",
           input);
    return;
  }

  printf("\n  " CLR_GREEN "[Ditemukan]" CLR_RESET " " CLR_BOLD CLR_YELLOW
         "%d" CLR_RESET " kata ditemukan!\n",
         count);

  // Pagination settings
  int words_per_page = 20;
  int current_page = 1;
  int total_pages = (count + words_per_page - 1) / words_per_page;
  if (total_pages < 1)
    total_pages = 1;

  while (1) {
    int start_idx = (current_page - 1) * words_per_page;
    int end_idx = start_idx + words_per_page;
    if (end_idx > count)
      end_idx = count;

    // Tampilkan header
    printf("\n");
    printf(CLR_CYAN
           "  +----------------------------------------------------------+\n");
    printf("  |            PREFIX SEARCH (Autocomplete - Trie)           |\n");
    printf("  +----------------------------------------------------------+\n");
    printf("  |  Prefix: " CLR_BOLD CLR_YELLOW "%-47s" CLR_RESET CLR_CYAN
           " |\n",
           input);
    printf("  |  Halaman " CLR_BOLD CLR_YELLOW "%d" CLR_RESET CLR_CYAN
           "/" CLR_BOLD CLR_YELLOW "%d" CLR_RESET CLR_CYAN " (" CLR_GREEN
           "%d" CLR_RESET CLR_CYAN
           " kata)                                   |\n",
           current_page, total_pages, count);
    printf("  "
           "+----------------------------------------------------------+"
           "\n" CLR_RESET);

    // Tampilkan tabel
    printf("\n  " CLR_BOLD CLR_WHITE "%-5s | %-25s | %-25s" CLR_RESET "\n",
           "No.", "Indonesian", "English");
    printf("  ");
    for (int i = 0; i < 65; i++)
      printf(CLR_CYAN "-" CLR_RESET);
    printf("\n");

    for (int i = start_idx; i < end_idx; i++) {
      printf("  %-5d | " CLR_GREEN "%-25s" CLR_RESET " | " CLR_YELLOW
             "%-25s" CLR_RESET "\n",
             i + 1, all_results[i].indonesian, all_results[i].english);
    }

    printf("\n");
    printf(CLR_CYAN
           "  +----------------------------------------------------------+\n");
    printf("  |     [1-%d] Pilih Kata   |   [/1-%d] Pindah Halaman        |\n",
           count, total_pages);
    printf("  |     [<] Kembali ke Menu Utama                            |\n");
    printf("  "
           "+----------------------------------------------------------+"
           "\n" CLR_RESET);
    printf("\n  Pilihan: " CLR_BOLD CLR_YELLOW);

    if (fgets(nav_input, sizeof(nav_input), stdin) == NULL) {
      printf(CLR_RESET);
      free(all_results);
      return;
    }
    printf(CLR_RESET);

    nav_input[strcspn(nav_input, "\n")] = '\0';

    if (strlen(nav_input) == 0) {
      continue;
    }

    char first_char = nav_input[0];

    // Kembali ke menu utama
    if (first_char == '<') {
      printf("\n  " CLR_CYAN "[Info]" CLR_RESET " Kembali ke menu utama.\n");
      break;
    }

    // Navigasi halaman dengan /1, /2, /3, dst
    if (first_char == '/') {
      int page_num = atoi(nav_input + 1);
      if (page_num >= 1 && page_num <= total_pages) {
        current_page = page_num;
      } else {
        printf("\n  " CLR_RED "[Error]" CLR_RESET
               " Halaman tidak valid. Pilih 1-%d\n",
               total_pages);
      }
      continue;
    }

    // Pilih kata dengan nomor
    if (first_char >= '1' && first_char <= '9') {
      int word_num = atoi(nav_input);
      if (word_num >= 1 && word_num <= count) {
        WordEntry *word = &all_results[word_num - 1];

        // Tambahkan ke riwayat pencarian
        dict_add_to_history(dict, *word);

        // Hitung lebar box
        int max_len = strlen(word->indonesian);
        if ((int)strlen(word->english) > max_len)
          max_len = strlen(word->english);
        if ((int)strlen(word->example) > max_len)
          max_len = strlen(word->example);
        if ((int)strlen(word->pronunciation) > max_len)
          max_len = strlen(word->pronunciation);
        if ((int)strlen(word->meanings[0].text) > max_len)
          max_len = strlen(word->meanings[0].text);

        int box_width = max_len + 18;
        if (box_width < 45)
          box_width = 45;
        if (box_width > 80)
          box_width = 80;

        // Hitung padding untuk centering judul
        int title_len = 11; // strlen("DETAIL KATA")
        int pad_left = (box_width - title_len) / 2;
        int pad_right = box_width - title_len - pad_left;

        printf("\n");
        printf(CLR_BLUE "  +");
        for (int i = 0; i < box_width; i++)
          printf("-");
        printf("+\n" CLR_RESET);
        printf(CLR_BLUE "  |" CLR_RESET "%*s" CLR_BOLD CLR_YELLOW
                        "DETAIL KATA" CLR_RESET "%*s" CLR_BLUE "|\n" CLR_RESET,
               pad_left, "", pad_right, "");
        printf(CLR_BLUE "  +");
        for (int i = 0; i < box_width; i++)
          printf("-");
        printf("+\n" CLR_RESET);

        // Label width
        int label_w = 13;

        // Indonesian
        printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
                        " : " CLR_GREEN "%s" CLR_RESET "\n",
               label_w - 2, "Indonesian", word->indonesian);
        // English
        printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
                        " : " CLR_GREEN "%s" CLR_RESET "\n",
               label_w - 2, "English", word->english);
        // POS
        printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
                        " : " CLR_MAGENTA "%s" CLR_RESET "\n",
               label_w - 2, "POS", pos_to_string(word->pos));
        // Category
        printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
                        " : " CLR_MAGENTA "%s" CLR_RESET "\n",
               label_w - 2, "Category", category_to_string(word->category));
        // Pronounce
        printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
                        " : " CLR_YELLOW "%s" CLR_RESET "\n",
               label_w - 2, "Pronounce", word->pronunciation);

        // Separator
        printf(CLR_BLUE "  +");
        for (int i = 0; i < box_width; i++)
          printf("-");
        printf("+\n" CLR_RESET);

        // Definitions
        printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
                        ":\n",
               label_w - 2, "Definition(s)");
        for (int i = 0; i < word->meaning_count; i++) {
          printf(CLR_BLUE "  |" CLR_RESET " %*d. " CLR_WHITE "%s" CLR_RESET
                          "\n",
                 label_w - 3, i + 1, word->meanings[i].text);
        }

        // Separator
        printf(CLR_BLUE "  +");
        for (int i = 0; i < box_width; i++)
          printf("-");
        printf("+\n" CLR_RESET);

        // Example
        printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
                        " : " CLR_WHITE "%s" CLR_RESET "\n",
               label_w - 2, "Example", word->example);

        printf(CLR_BLUE "  +");
        for (int i = 0; i < box_width; i++)
          printf("-");
        printf("+\n" CLR_RESET);

        // Kembalikan ke halaman yang sesuai
        current_page = (word_num - 1) / words_per_page + 1;
      } else {
        printf("\n  " CLR_RED "[Error]" CLR_RESET
               " Nomor tidak valid. Pilih 1-%d\n",
               count);
      }
      continue;
    }

    printf("\n  " CLR_CYAN "[Info]" CLR_RESET " Pilihan tidak valid.\n");
  }

  free(all_results);
}

// ============================================================================
// Handler CRUD
// ============================================================================

// Handler tambah kata
static void handle_add_word(DictionaryManager *dict) {
  char indonesian[MAX_WORD_LEN];
  char english[MAX_WORD_LEN];
  char definition[MAX_DEF_LEN];
  char example[MAX_EXAMPLE_LEN];
  char pronunciation[MAX_WORD_LEN];

  printf("\n");
  printf("  +----------------------------------------------------------+\n");
  printf("  |                    TAMBAH KATA BARU                      |\n");
  printf("  +----------------------------------------------------------+\n");

  if (get_string_input("Kata Indonesia", indonesian, MAX_WORD_LEN) == NULL)
    return;
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

  if (get_string_input("Terjemahan Inggris", english, MAX_WORD_LEN) == NULL)
    return;
  if (strlen(english) == 0) {
    printf("  [Error] Terjemahan Inggris tidak boleh kosong.\n");
    return;
  }

  PartOfSpeech pos = get_pos_from_input();

  if (get_string_input("Definisi", definition, MAX_DEF_LEN) == NULL)
    return;
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
static void handle_update_word(DictionaryManager *dict) {
  char indonesian[MAX_WORD_LEN];
  char input[20];

  printf("\n");
  printf("  +----------------------------------------------------------+\n");
  printf("  |                        UPDATE KATA                       |\n");
  printf("  +----------------------------------------------------------+\n");

  if (get_string_input("Kata Indonesia yang akan diupdate", indonesian,
                       MAX_WORD_LEN) == NULL)
    return;
  if (strlen(indonesian) == 0) {
    printf("  [Error] Masukkan kata yang akan diupdate.\n");
    return;
  }

  WordEntry *existing = dict_search_indonesian(dict, indonesian);
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
    printf("  |  1. Indonesian     : %-35s |\n", existing->indonesian);
    printf("  |  2. English        : %-35s |\n", existing->english);
    printf("  |  3. POS            : %-35s |\n", pos_to_string(existing->pos));
    printf("  |  4. Category       : %-35s |\n",
           category_to_string(existing->category));
    printf("  |  5. Pronounce      : %-35s |\n", existing->pronunciation);
    printf("  |  6. Definition     : %-35s |\n", existing->meanings[0].text);
    printf("  |  7. Example        : %-35s |\n", existing->example);
    printf("  +----------------------------------------------------------+\n");
    printf("  |                                                          |\n");
    printf("  |  [1-7] Pilih field yang akan diubah                      |\n");
    printf("  |  [0]   Simpan dan keluar                                 |\n");
    printf("  |                                                          |\n");
    printf("  +----------------------------------------------------------+\n");
    printf("\n  Pilihan: ");

    if (fgets(input, sizeof(input), stdin) == NULL)
      break;
    input[strcspn(input, "\n")] = '\0';

    if (strlen(input) == 0)
      continue;

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
    } else if (first_char == '2') {
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
    } else if (first_char == '3') {
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
          PartOfSpeech pos_array[] = {NOUN,    VERB,         ADJECTIVE,
                                      ADVERB,  PREPOSITION,  CONJUNCTION,
                                      PRONOUN, INTERJECTION, DETERMINER};
          existing->pos = pos_array[choice - 1];
          printf("  [Success] POS berhasil diubah!\n");
        } else {
          printf("  [Error] Pilihan tidak valid.\n");
        }
      }
    } else if (first_char == '4') {
      // Update Category
      printf("\n  Category saat ini: %s\n",
             category_to_string(existing->category));
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
    } else if (first_char == '5') {
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
    } else if (first_char == '6') {
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
    } else if (first_char == '7') {
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
    } else {
      printf(
          "\n  [Error] Pilihan tidak valid. Pilih 1-7 atau 0 untuk keluar.\n");
    }
  }
}

// Handler menghapus kata
static void handle_delete_word(DictionaryManager *dict) {
  char indonesian[MAX_WORD_LEN];
  char confirm;

  printf("\n");
  printf("  +----------------------------------------------------------+\n");
  printf("  |                        HAPUS KATA                        |\n");
  printf("  +----------------------------------------------------------+\n");

  if (get_string_input("Kata Indonesia yang akan dihapus", indonesian,
                       MAX_WORD_LEN) == NULL)
    return;
  if (strlen(indonesian) == 0) {
    printf("  [Error] Masukkan kata yang akan dihapus.\n");
    return;
  }

  WordEntry *existing = dict_search_indonesian(dict, indonesian);
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