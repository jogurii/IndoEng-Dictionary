
#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "data_structures.h"

// ============================================================================
// UI Utilities
// ============================================================================

int ui_calculate_box_width(int max_content_len, int min_width, int max_width);

void ui_calculate_title_padding(int box_width, int title_len, int *pad_left, int *pad_right);

void ui_display_word_box(const WordEntry *word, const char *title);

void ui_display_word_compact(const WordEntry *word, int index);

void ui_display_box_lines(int box_width, const char **lines, int line_count);

int ui_get_max_field_length(const WordEntry *word);

void ui_print_separator(int width, const char *color);

#endif // UI_UTILS_H