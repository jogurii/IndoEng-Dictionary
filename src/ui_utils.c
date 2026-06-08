
#include "ui_utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Implementation
// ============================================================================

int ui_calculate_box_width(int max_content_len, int min_width, int max_width) {
    int width = max_content_len + 20;  // Padding for labels and spacing
    if (width < min_width) width = min_width;
    if (width > max_width) width = max_width;
    return width;
}

void ui_calculate_title_padding(int box_width, int title_len, int *pad_left, int *pad_right) {
    *pad_left = (box_width - title_len) / 2;
    *pad_right = box_width - title_len - *pad_left;
}

int ui_get_max_field_length(const WordEntry *word) {
    if (word == NULL) return 0;

    int max_len = strlen(word->indonesian);
    if ((int)strlen(word->english) > max_len)
        max_len = strlen(word->english);
    if ((int)strlen(word->example) > max_len)
        max_len = strlen(word->example);
    if ((int)strlen(word->pronunciation) > max_len)
        max_len = strlen(word->pronunciation);
    if ((int)strlen(word->meanings[0].text) > max_len)
        max_len = strlen(word->meanings[0].text);

    return max_len;
}

void ui_print_separator(int width, const char *color) {
    printf("%s  +", color);
    for (int i = 0; i < width; i++)
        printf("-");
    printf("+\n" CLR_RESET);
}

void ui_display_word_box(const WordEntry *word, const char *title) {
    if (word == NULL || title == NULL) return;

    int max_len = ui_get_max_field_length(word);
    int box_width = ui_calculate_box_width(max_len, 50, 80);
    int title_len = (int)strlen(title);
    int pad_left, pad_right;
    ui_calculate_title_padding(box_width, title_len, &pad_left, &pad_right);
    int label_w = 14;

    // Top border
    printf(CLR_BLUE "  +");
    for (int i = 0; i < box_width; i++) printf("-");
    printf("+\n" CLR_RESET);

    // Title
    printf(CLR_BLUE "  |" CLR_RESET "%*s" CLR_BOLD CLR_YELLOW "%s" CLR_RESET
           "%*s" CLR_BLUE "|\n" CLR_RESET, pad_left, "", title, pad_right, "");

    // Top border
    printf(CLR_BLUE "  +");
    for (int i = 0; i < box_width; i++) printf("-");
    printf("+\n" CLR_RESET);

    // Word fields
    printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
           " : " CLR_GREEN "%s" CLR_RESET "\n",
           label_w - 2, "Indonesian", word->indonesian);
    printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
           " : " CLR_GREEN "%s" CLR_RESET "\n",
           label_w - 2, "English", word->english);
    printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
           " : " CLR_MAGENTA "%s" CLR_RESET "\n",
           label_w - 2, "POS", pos_to_string(word->pos));
    printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
           " : " CLR_MAGENTA "%s" CLR_RESET "\n",
           label_w - 2, "Category", category_to_string(word->category));
    printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
           " : " CLR_YELLOW "%s" CLR_RESET "\n",
           label_w - 2, "Pronounce", word->pronunciation);

    // Separator
    printf(CLR_BLUE "  +");
    for (int i = 0; i < box_width; i++) printf("-");
    printf("+\n" CLR_RESET);

    // Definitions
    printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET ":\n",
           label_w - 2, "Definition(s)");
    for (int i = 0; i < word->meaning_count; i++) {
        printf(CLR_BLUE "  |" CLR_RESET " %*d. " CLR_WHITE "%s" CLR_RESET "\n",
               label_w - 3, i + 1, word->meanings[i].text);
    }

    // Separator
    printf(CLR_BLUE "  +");
    for (int i = 0; i < box_width; i++) printf("-");
    printf("+\n" CLR_RESET);

    // Example
    printf(CLR_BLUE "  |" CLR_RESET " " CLR_BOLD CLR_CYAN "%-*s" CLR_RESET
           " : " CLR_WHITE "%s" CLR_RESET "\n",
           label_w - 2, "Example", word->example);

    // Bottom border
    printf(CLR_BLUE "  +");
    for (int i = 0; i < box_width; i++) printf("-");
    printf("+\n" CLR_RESET);
}

void ui_display_word_compact(const WordEntry *word, int index) {
    if (word == NULL) return;

    printf("  %-4d | " CLR_GREEN "%-25s" CLR_RESET " | " CLR_YELLOW "%-25s" CLR_RESET
           " | " CLR_MAGENTA "%-8s" CLR_RESET " | %s\n",
           index,
           word->indonesian,
           word->english,
           pos_to_string(word->pos),
           category_to_string(word->category));
}