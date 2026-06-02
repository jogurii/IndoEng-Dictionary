# Indonesia-English Dictionary - Project Specification

## 1. Project Overview

- **Project Name**: IndoEng Dictionary (Indonesian-English Dictionary)
- **Project Type**: Console Application (C Language)
- **Core Functionality**: A fully-featured bilingual dictionary supporting CRUD operations on 300 Indonesian-English word pairs, utilizing 6 different data structures for efficient storage, searching, and organization.
- **Target Users**: Language learners, translators, students studying Indonesian or English

## 2. Technical Requirements

### Data Structures (Minimum 6 Required)

1. **Linked List** - For word history and undo operations
2. **Binary Search Tree (BST)** - For alphabetically sorted word storage and lookup
3. **Hash Table** - For fast O(1) word lookup by English key
4. **Trie (Prefix Tree)** - For autocomplete and prefix-based search
5. **Stack** - For undo operations and navigation history
6. **Queue** - For processing bulk operations and word of the day feature

### CRUD Operations

- **Create**: Add new word pairs (Indonesian-English)
- **Read**: Search and display words (multiple search methods)
- **Update**: Modify existing word definitions, examples, or pronunciations
- **Delete**: Remove word entries with confirmation

## 3. Data Structure Details

### 3.1 Main Storage
- **Primary**: Binary Search Tree (BST) - sorted alphabetically by Indonesian word
- **Secondary**: Hash Table - indexed by English word for fast English-based lookup
- **Index**: Trie - for prefix/autocomplete searches

### 3.2 Supporting Structures
- **Linked List**: Recent searches/history tracking
- **Stack**: Undo operations for deletes and edits
- **Queue**: Word of the day queue, bulk import processing

## 4. Word Entry Structure

Each dictionary entry contains:
- Indonesian word (primary key)
- English translation (secondary key)
- Part of speech (noun, verb, adjective, etc.)
- Definition (multiple meanings supported)
- Example sentence(s)
- Pronunciation guide
- Category (everyday, formal, slang, technical)
- Word frequency score
- Entry timestamp

## 5. User Interface

### Main Menu
```
╔══════════════════════════════════════════════════════════════╗
║         INDONESIAN - ENGLISH DICTIONARY (IndoEng)            ║
╠══════════════════════════════════════════════════════════════╣
║  1. Search Word (Indonesian)                                ║
║  2. Search Word (English)                                    ║
║  3. Prefix Search (Autocomplete)                            ║
║  4. Add New Word                                            ║
║  5. Update Word                                             ║
║  6. Delete Word                                             ║
║  7. View All Words (Alphabetical)                           ║
║  8. Word of the Day                                         ║
║  9. Search History                                          ║
║ 10. Undo Last Action                                        ║
║ 11. Statistics                                              ║
║  0. Exit                                                    ║
╚══════════════════════════════════════════════════════════════╝
```

## 6. Real-World Case Flow

1. **Dictionary Management Flow**: Librarian/editor adds, updates, deletes words
2. **Learning Flow**: Student searches words, uses autocomplete, views history
3. **Quality Control Flow**: Undo capability for accidental deletions
4. **Discovery Flow**: Word of the day encourages exploration
5. **Bulk Import Flow**: Queue-based processing for batch imports

## 7. Search Capabilities

- Search by Indonesian word (BST lookup - O(log n))
- Search by English word (Hash table - O(1) average)
- Prefix search (Trie traversal - efficient)
- Case-insensitive search
- Display matching words count

## 8. Data Persistence

- Initial load from embedded 300 word dataset
- Option to save changes to file
- Data remains in memory for session duration

## 9. Acceptance Criteria

- [ ] Menu system displays all 6 CRUD options
- [ ] All 6 data structures are clearly identifiable in code
- [ ] Dictionary contains minimum 300 word pairs
- [ ] BST maintains alphabetical order
- [ ] Hash table enables fast English word lookup
- [ ] Trie provides accurate autocomplete
- [ ] Undo restores previous word states
- [ ] History tracks last 20 searches
- [ ] Word of the Day cycles through available words
- [ ] Code compiles without warnings
- [ ] All features work as specified

## 10. File Structure

```
indoeng-dictionary/
├── SPEC.md
├── src/
│   ├── main.c              # Entry point and menu
│   ├── dictionary.c        # Dictionary operations
│   ├── dictionary.h        # Header files
│   ├── data_structures.c   # All 6 data structure implementations
│   ├── data_structures.h   # Data structure headers
│   └── word_data.c         # 300 embedded words
└── Makefile
```
