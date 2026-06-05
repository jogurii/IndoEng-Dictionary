/**
 * @file ui_utils.h
 * @brief UI utilities for consistent console display formatting
 */
#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "data_structures.h"

// ============================================================================
// UI UTILITIES
// ============================================================================

/**
 * @brief Calculate optimal box width based on content
 * @param max_content_len Maximum string length in content
 * @param min_width Minimum box width (default: 50)
 * @param max_width Maximum box width (default: 80)
 * @return Optimal box width
 */
int ui_calculate_box_width(int max_content_len, int min_width, int max_width);

/**
 * @brief Calculate center padding for box title
 * @param box_width Total box width
 * @param title_len Title string length
 * @param pad_left Output: left padding
 * @param pad_right Output: right padding
 */
void ui_calculate_title_padding(int box_width, int title_len, int *pad_left, int *pad_right);

/**
 * @brief Display a word entry in a formatted box
 * @param word Pointer to WordEntry to display
 * @param title Box title (e.g., "DETAIL KATA")
 */
void ui_display_word_box(const WordEntry *word, const char *title);

/**
 * @brief Display a word entry in a compact horizontal format
 * @param word Pointer to WordEntry to display
 * @param index Display index number
 */
void ui_display_word_compact(const WordEntry *word, int index);

/**
 * @brief Display a bordered box with content lines
 * @param box_width Total box width
 * @param lines Array of formatted lines (label:value pairs)
 * @param line_count Number of lines
 */
void ui_display_box_lines(int box_width, const char **lines, int line_count);

/**
 * @brief Get maximum string length from a word entry for box sizing
 * @param word Pointer to WordEntry
 * @return Maximum string length among all fields
 */
int ui_get_max_field_length(const WordEntry *word);

/**
 * @brief Print horizontal separator line
 * @param width Line width (including corners)
 * @param color Color code to use (e.g., CLR_CYAN)
 */
void ui_print_separator(int width, const char *color);

#endif // UI_UTILS_H