# Dokumentasi IndoEng Dictionary

## Panduan Presentasi Project Struktur Data

---

## 1. Gambaran Project

### Apa itu IndoEng Dictionary?

**IndoEng Dictionary** adalah aplikasi kamus dwibahasa Indonesia-Inggris berbasis teks (console) yang dibangun menggunakan bahasa pemrograman C. Aplikasi ini memiliki fitur lengkap untuk mengelola 150+ pasangan kata dengan berbagai operasi pencarian dan manipulasi data.

### Spesifikasi Teknis

| Komponen | Detail |
|----------|--------|
| **Bahasa** | C (C99 Standard) |
| **Platform** | Console/Terminal |
| **Compiler** | GCC |
| **Jumlah Kata** | ~150+ pasang kata Indonesia-Inggris |
| **Data Structures** | 6 jenis struktur data |

---

## 2. Struktur File Project

```
indoeng-dictionary/
├── SPEC.md                  # Spesifikasi project
├── Makefile                 # Konfigurasi build
├── src/
│   ├── main.c               # Entry point & menu interaktif
│   ├── dictionary.c         # Logika utama dictionary manager
│   ├── dictionary.h         # Header dictionary
│   ├── data_structures.c    # Implementasi 6 struktur data
│   ├── data_structures.h    # Deklarasi struktur data
│   └── word_data.c          # Dataset 150+ kata
└── bin/
    └── indoeng-dictionary.exe  # File executable
```

### Penjelasan Setiap File

---

### 2.1 `main.c` - Entry Point & Menu Interaktif

**Tugas utama:**
- Menampilkan menu utama aplikasi
- Menghandle input pengguna
- Mengatur alur program (switch case)

**Fitur utama:**
```
1.  View All Words       - Lihat semua kata (dengan pagination)
2.  Search Indonesian    - Cari kata Indonesia (BST)
3.  Search English       - Cari kata Inggris (Hash Table)
4.  Prefix Search        - Autocomplete (Trie)
5.  Add New Word         - Tambah kata baru
6.  Update Word          - Update kata (pilih field yang diubah)
7.  Delete Word          - Hapus kata
8.  Word of the Day      - Kata acak hari ini
9.  Search History       - Riwayat pencarian
10. Undo Last Action     - Batalkan aksi terakhir (add/update/delete)
11. Statistics           - Statistik kamus
0.  Exit                 - Keluar program
```

**Fitur Keyboard/UX:**
- Di setiap fitur (Add, Update, Delete, Undo, Statistics, Word of the Day), setelah operasi selesai user harus **tekan Enter** untuk kembali ke menu utama
- Pada fitur Update, user bisa memilih field mana yang ingin diubah satu per satu tanpa harus mengisi semua
- Jika input hanya tekan Enter (kosong), tidak akan mengubah nilai field yang ada
- Input simbol `-` untuk mengosongkan field

**Fungsi penting:**
- `display_main_menu()` - Menampilkan menu
- `handle_add_word()` - Handler menambah kata (dengan validasi kata sudah ada)
- `handle_update_word()` - Handler update kata (pilih field satu per satu)
- `handle_delete_word()` - Handler menghapus kata
- `get_pos_from_input()` - Input part of speech
- `get_category_from_input()` - Input kategori kata
- `print_last_searched_if_exists()` - Tampilkan kata terakhir di atas menu

---

### 2.2 `dictionary.c` - Dictionary Manager

**Tugas utama:**
- Mengelola operasi CRUD pada dictionary
- Mengkoordinasikan semua struktur data
- Menyediakan fungsi helper

**Struktur utama `DictionaryManager`:**
```c
typedef struct {
    BSTNode* bst_root;           // BST untuk lookup Indonesia
    HashTable* hash_english;      // Hash table untuk lookup Inggris
    TrieNode* trie_root;          // Trie untuk autocomplete
    LinkedList* search_history;   // History pencarian
    Stack* undo_stack;            // Stack untuk undo operations
    Queue* word_queue;            // Queue untuk word of the day

    int total_words;
    int word_of_day_index;
    WordEntry last_searched;      // Kata terakhir yang dicari
    int has_last_searched;        // Flag apakah ada kata terakhir
} DictionaryManager;
```

**Fungsi penting:**
- `dict_create()` - Membuat instance dictionary baru
- `dict_destroy()` - Membebaskan memori
- `dict_add_word()` - Menambah kata baru
- `dict_update_word()` - Update kata
- `dict_delete_word()` - Hapus kata
- `dict_delete_word_no_undo()` - Hapus kata tanpa menambah ke undo stack
- `dict_search_indonesian()` - Cari dengan BST
- `dict_search_english()` - Cari dengan Hash Table
- `dict_search_prefix()` - Cari prefix dengan Trie
- `dict_view_all()` - Tampilkan semua kata dengan pagination
- `dict_undo()` - Batalkan aksi terakhir
- `dict_word_of_day()` - Tampilkan kata acak
- `dict_display_history()` - Tampilkan riwayat pencarian
- `dict_statistics()` - Tampilkan statistik kamus
- `dict_save_to_file()` - Simpan dictionary ke file

**Helper functions:**
- `collect_all_words()` - Kumpulkan semua kata dari BST
- `rebuild_trie()` - Rebuild Trie setelah update
- `count_bst_stats()` - Hitung statistik
- `save_bst_node()` - Simpan node BST ke file

**Undo mechanism (Marker-based):**
- Menggunakan prefix `@ADD:`, `@UPDATE:`, `@DELETE:` pada field `indonesian` di WordEntry
- Saat undo, prefix di-parse untuk menentukan aksi yang harus dilakukan

---

### 2.3 `data_structures.h` - Deklarasi Struktur Data

**File ini mendeklarasikan semua struktur data dan enums.**

**Enums:**

```c
// Part of Speech (Jenis Kata)
typedef enum {
    NOUN,        // Kata benda
    VERB,        // Kata kerja
    ADJECTIVE,   // Kata sifat
    ADVERB,      // Kata keterangan
    PRONOUN,     // Kata ganti
    PREPOSITION, // Kata depan
    CONJUNCTION, // Kata hubung
    INTERJECTION,// Kata seru
    DETERMINER   // Kata penentu
} PartOfSpeech;

// Word Category (Kategori Kata)
typedef enum {
    EVERYDAY,    // Sehari-hari
    FORMAL,      // Formal
    SLANG,       // Slang/gaul
    TECHNICAL    // Teknis/teknologi
} WordCategory;
```

**Konstanta:**
```c
#define MAX_WORD_LEN 100       // Panjang maksimal kata
#define MAX_DEF_LEN 500        // Panjang definisi
#define MAX_EXAMPLE_LEN 300    // Panjang contoh kalimat
#define MAX_CATEGORY_LEN 50   // Panjang kategori
#define MAX_MEANINGS 5         // Maksimal jumlah makna
#define HASH_TABLE_SIZE 1000   // Ukuran hash table
#define ALPHABET_SIZE 26       // Ukuran alphabet Trie
#define MAX_STACK_SIZE 100    // Ukuran stack undo
#define MAX_QUEUE_SIZE 1000   // Ukuran queue kata
```

**Struct Meaning:**
```c
typedef struct {
    char text[MAX_DEF_LEN];
} Meaning;
```

**Struct WordEntry:**
```c
typedef struct {
    char indonesian[MAX_WORD_LEN];   // Kata Indonesia
    char english[MAX_WORD_LEN];       // Terjemahan Inggris
    PartOfSpeech pos;                 // Jenis kata
    Meaning meanings[MAX_MEANINGS];    // Array makna/definisi
    int meaning_count;                 // Jumlah makna yang tersimpan
    char example[MAX_EXAMPLE_LEN];    // Contoh kalimat
    char pronunciation[MAX_WORD_LEN];  // Cara pengucapan
    WordCategory category;            // Kategori
    time_t timestamp;                  // Waktu ditambahkan
} WordEntry;
```

---

### 2.4 `data_structures.c` - Implementasi 6 Struktur Data

**Ini adalah file TERPENTING untuk presentasi karena berisi implementasi 6 struktur data.**

**Helper Functions:**
```c
const char* pos_to_string(PartOfSpeech pos);    // NOUN→"Noun", VERB→"Verb", dll.
const char* category_to_string(WordCategory cat); // EVERYDAY→"Everyday", dll.
void print_word_entry(WordEntry* word);         // Tampilkan detail kata
WordEntry create_word_entry(...);                // Buat WordEntry baru
```

---

## 3. Enam Struktur Data (POIN PRESENTASI UTAMA)

### 3.1 Binary Search Tree (BST)

**Tujuan:** Penyimpanan kata secara alfabetis & pencarian O(log n)

**Struktur Node:**
```c
typedef struct BSTNode {
    WordEntry word;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;
```

**Karakteristik:**
- **Left subtree:** Kata yang lebih kecil (A-Z)
- **Right subtree:** Kata yang lebih besar
- **Pencarian:** Seperti mencari di kamus (buka halaman tengah, bandingkan)
- **Complexitas:** O(log n) rata-rata, O(n) worst case

**Operasi utama:**
| Fungsi | Deskripsi |
|--------|-----------|
| `bst_create_node()` | Buat node baru |
| `bst_insert()` | Sisipkan kata baru |
| `bst_search()` | Cari kata berdasarkan Indonesia |
| `bst_delete()` | Hapus kata |
| `bst_find_min()` | Cari node dengan nilai terkecil (successor) |
| `bst_update_key()` | Update key (indonesian) di BST |
| `bst_inorder()` | Traversing in-order (A-Z) |
| `bst_destroy()` | Bebaskan memori |
| `bst_count()` | Hitung jumlah node |

**Implementasi pencarian:**
```c
BSTNode* bst_search(BSTNode* root, const char* indonesian) {
    if (root == NULL) return NULL;

    int cmp = strcasecmp(indonesian, root->word.indonesian);

    if (cmp == 0) {
        return root;  // Ketemu!
    } else if (cmp < 0) {
        return bst_search(root->left, indonesian);   // Lebih kecil
    } else {
        return bst_search(root->right, indonesian); // Lebih besar
    }
}
```

**Implementasi penyisipan:**
```c
BSTNode* bst_insert(BSTNode* root, WordEntry word) {
    if (root == NULL) {
        return bst_create_node(word);
    }

    int cmp = strcasecmp(word.indonesian, root->word.indonesian);

    if (cmp < 0) {
        root->left = bst_insert(root->left, word);
    } else if (cmp > 0) {
        root->right = bst_insert(root->right, word);
    } else {
        root->word = word;  // Update jika sudah ada
    }
    return root;
}
```

**Implementasi penghapusan:**
```c
BSTNode* bst_delete(BSTNode* root, const char* indonesian) {
    // Kasus 1: Node tidak ditemukan → return NULL
    // Kasus 2: Node adalah leaf (tanpa anak) → free, return NULL
    // Kasus 3: Node punya satu anak → replace dengan anak, free
    // Kasus 4: Node punya dua anak → cari successor (min dari right subtree)
}
```

---

### 3.2 Hash Table

**Tujuan:** Pencarian O(1) berdasarkan kata Inggris

**Struktur:**
```c
#define HASH_TABLE_SIZE 1000

typedef struct HashNode {
    WordEntry word;
    struct HashNode* next;  // Chaining untuk collision
} HashNode;

typedef struct {
    HashNode* buckets[HASH_TABLE_SIZE];
    int size;
} HashTable;
```

**Karakteristik:**
- Menggunakan **chaining** untuk menangani collision
- **Hash function:** Menggunakan metode polynomial rolling hash
- **Key:** Kata Inggris (case-insensitive)
- **Complexitas:** O(1) average case

**Hash Function:**
```c
unsigned int hash_function(const char* str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + (unsigned char)(tolower(*str));
        str++;
    }
    return hash % HASH_TABLE_SIZE;
}
```

**Operasi utama:**
| Fungsi | Deskripsi |
|--------|-----------|
| `hash_create()` | Buat hash table baru |
| `hash_insert()` | Sisipkan kata |
| `hash_search()` | Cari kata Inggris |
| `hash_delete()` | Hapus kata (return WordEntry*) |
| `hash_remove()` | Hapus kata (tanpa return) |
| `hash_destroy()` | Bebaskan memori |
| `hash_size()` | Mendapatkan ukuran |

**Implementasi pencarian:**
```c
WordEntry* hash_search(HashTable* ht, const char* english) {
    unsigned int index = hash_function(english);

    HashNode* current = ht->buckets[index];
    while (current != NULL) {
        if (strcasecmp(current->word.english, english) == 0) {
            return &(current->word);  // Ketemu!
        }
        current = current->next;
    }
    return NULL;  // Tidak ditemukan
}
```

---

### 3.3 Trie (Prefix Tree)

**Tujuan:** Autocomplete & pencarian prefix (awalan kata)

**Struktur:**
```c
#define ALPHABET_SIZE 26

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    int is_end_of_word;
    WordEntry word;      // Simpan word entry di node akhir
    int word_count;      // Counter untuk prefix
} TrieNode;
```

**Karakteristik:**
- Setiap node memiliki 26 children (a-z)
- **Path:** Merepresentasikan prefix kata
- **Node akhir:** Menandai akhir sebuah kata
- **Complexitas:** O(m) where m = panjang prefix

**Visualisasi Trie:**
```
Contoh kata: "makan", "maju", "kecil"

Root
├── m
│   ├── a
│   │   ├── k
│   │   │   └── a
│   │   │       └── n (END: makan)
│   │   └── j
│   │       └── u (END: maju)
│   └── i
│       └── n
│           └── u
│               └── m
│                   └── (END: minum)
└── k
    └── e
        └── c
            └── i
                └── l (END: kecil)
```

**Operasi utama:**
| Fungsi | Deskripsi |
|--------|-----------|
| `trie_create_node()` | Buat node baru |
| `trie_insert()` | Sisipkan kata |
| `trie_search_prefix()` | Cari prefix node |
| `trie_get_all_with_prefix()` | Ambil semua kata dengan prefix |
| `trie_collect_all()` | Helper - rekursif collect semua kata |
| `trie_destroy()` | Bebaskan memori |

**Implementasi penyisipan:**
```c
void trie_insert(TrieNode* root, const char* word, WordEntry entry) {
    TrieNode* current = root;

    while (*word) {
        int index = tolower(*word) - 'a';

        if (current->children[index] == NULL) {
            current->children[index] = trie_create_node();
        }

        current = current->children[index];
        current->word_count++;
        word++;
    }

    current->is_end_of_word = 1;
    current->word = entry;  // Simpan word entry
}
```

**Implementasi pencarian prefix:**
```c
TrieNode* trie_search_prefix(TrieNode* root, const char* prefix) {
    TrieNode* current = root;

    while (*prefix) {
        int index = tolower(*prefix) - 'a';

        if (current->children[index] == NULL) {
            return NULL;  // Prefix tidak ditemukan
        }
        current = current->children[index];
        prefix++;
    }
    return current;  // Node prefix ditemukan
}
```

**Implementasi get all with prefix:**
```c
void trie_get_all_with_prefix(TrieNode* root, const char* prefix, 
                              WordEntry** results, int* count) {
    TrieNode* prefix_node = trie_search_prefix(root, prefix);
    
    if (prefix_node == NULL) {
        *results = NULL;
        *count = 0;
        return;
    }

    int capacity = 10;
    *results = (WordEntry*)malloc(capacity * sizeof(WordEntry));
    *count = 0;

    trie_collect_all(prefix_node, results, count, &capacity);
}
```

---

### 3.4 Linked List

**Tujuan:** Menyimpan riwayat pencarian (history)

**Struktur:**
```c
typedef struct ListNode {
    WordEntry word;
    struct ListNode* next;
    struct ListNode* prev;
} ListNode;

typedef struct {
    ListNode* head;
    ListNode* tail;
    int size;
    int max_size;  // Batas maksimal (20 entries)
} LinkedList;
```

**Karakteristik:**
- **Doubly Linked List** (ada prev & next)
- **FIFO:** Item terbaru di depan, terlama di belakang
- **Auto-trimming:** Jika size > max_size, hapus tail
- **Duplicate handling:** Jika kata sudah ada di list, pindahkan ke head (update posisi)

**Operasi utama:**
| Fungsi | Deskripsi |
|--------|-----------|
| `list_create()` | Buat list baru (dengan max_size) |
| `list_insert_front()` | Sisipkan di depan |
| `list_insert_back()` | Sisipkan di belakang (FIFO) |
| `list_search()` | Cari dalam list |
| `list_remove()` | Hapus dari list |
| `list_display()` | Tampilkan semua |
| `list_destroy()` | Bebaskan memori |

**Implementasi sisip depan (dengan duplicate handling):**
```c
void list_insert_front(LinkedList* list, WordEntry word) {
    // 1. Cek duplicate - jika ada, pindahkan ke head
    // 2. Jika list penuh, hapus tail terlebih dahulu
    // 3. Buat node baru, set sebagai head

    ListNode* new_node = (ListNode*)malloc(sizeof(ListNode));
    new_node->word = word;
    new_node->next = list->head;
    new_node->prev = NULL;

    if (list->head != NULL) {
        list->head->prev = new_node;
    }
    list->head = new_node;

    if (list->tail == NULL) {
        list->tail = new_node;
    }
    list->size++;
}
```

---

### 3.5 Stack

**Tujuan:** Operasi undo (membatalkan aksi)

**Struktur:**
```c
#define MAX_STACK_SIZE 100

typedef struct {
    WordEntry entries[MAX_STACK_SIZE];
    int top;           // -1 jika kosong
    int max_size;
} Stack;
```

**Karakteristik:**
- **LIFO (Last In First Out):** Item terakhir masuk, pertama keluar
- **Fixed size array:** Maksimal 100 undo actions
- **Operasi:** Push (tambah), Pop (ambil), Peek (lihat)

**Operasi utama:**
| Fungsi | Deskripsi |
|--------|-----------|
| `stack_create()` | Buat stack baru |
| `stack_push()` | Tambah item ke top |
| `stack_pop()` | Ambil item dari top |
| `stack_peek()` | Lihat item di top |
| `stack_is_empty()` | Cek apakah kosong |
| `stack_is_full()` | Cek apakah penuh |

**Implementasi Push:**
```c
int stack_push(Stack* s, WordEntry entry) {
    if (s == NULL || stack_is_full(s)) {
        return 0;  // Gagal
    }
    s->entries[++s->top] = entry;  // Increment top, lalu push
    return 1;  // Berhasil
}
```

**Implementasi Pop:**
```c
int stack_pop(Stack* s, WordEntry* entry) {
    if (s == NULL || stack_is_empty(s)) {
        return 0;  // Gagal
    }
    *entry = s->entries[s->top--];  // Ambil, decrement top
    return 1;  // Berhasil
}
```

**Penggunaan dalam Undo (Enhanced dengan Marker):**
- Saat **add kata** → push dengan marker `@ADD:nama_kata` di field indonesian
- Saat **update kata** → push dengan marker `@UPDATE:nama_kata` + data kata lama
- Saat **delete kata** → push dengan marker `@DELETE:nama_kata` + data kata
- Saat **undo** → pop dari stack, cek marker prefix:
  - `@ADD:` → delete kata (undo add)
  - `@UPDATE:` → restore kata lama (undo update)
  - `@DELETE:` → restore kata (undo delete)

---

### 3.6 Queue

**Tujuan:** Fitur "Word of the Day" & shuffle kata

**Struktur:**
```c
#define MAX_QUEUE_SIZE 1000

typedef struct {
    WordEntry entries[MAX_QUEUE_SIZE];
    int front;      // Index depan
    int rear;       // Index belakang (-1 jika kosong)
    int size;       // Jumlah item saat ini
    int max_size;
} Queue;
```

**Karakteristik:**
- **Circular Queue:** Menggunakan modulo untuk wrap-around
- **FIFO (First In First Out):** Item pertama masuk, pertama keluar
- **Shuffle:** Menggunakan Fisher-Yates algorithm untuk random

**Operasi utama:**
| Fungsi | Deskripsi |
|--------|-----------|
| `queue_create()` | Buat queue baru |
| `queue_enqueue()` | Tambah item ke belakang |
| `queue_dequeue()` | Ambil item dari depan |
| `queue_peek()` | Lihat item di depan |
| `queue_get_random()` | Ambil kata acak |
| `queue_shuffle()` | Acak urutan queue |
| `queue_is_empty()` | Cek apakah kosong |
| `queue_is_full()` | Cek apakah penuh |

**Implementasi Enqueue (Circular):**
```c
int queue_enqueue(Queue* q, WordEntry entry) {
    if (q == NULL || queue_is_full(q)) {
        return 0;  // Gagal
    }
    q->rear = (q->rear + 1) % q->max_size;  // Wrap around
    q->entries[q->rear] = entry;
    q->size++;
    return 1;  // Berhasil
}
```

**Implementasi Shuffle (Fisher-Yates):**
```c
void queue_shuffle(Queue* q) {
    if (q == NULL || q->size <= 1) return;

    int n = q->size;
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);  // Random index

        int idx_i = (q->front + i) % q->max_size;
        int idx_j = (q->front + j) % q->max_size;

        // Tukar posisi
        WordEntry temp = q->entries[idx_i];
        q->entries[idx_i] = q->entries[idx_j];
        q->entries[idx_j] = temp;
    }
}
```

---

## 4. Alur Kerja Aplikasi

### 4.1 Inisialisasi

```
main()
    ↓
dict_create() → Buat DictionaryManager
    ├── bst_root = NULL
    ├── hash_english = hash_create()
    ├── trie_root = trie_create_node()
    ├── search_history = list_create(20)
    ├── undo_stack = stack_create(50)
    └── word_queue = queue_create(1000)
    ↓
dict_load_initial_data() → Muat 150+ kata
    ├── Buat WordEntry dari dataset
    ├── dict_add_word() untuk setiap kata
    └── Reset undo_stack (kosongkan)
```

### 4.2 Menambah Kata

```
dict_add_word(dict, word)
    │
    ├── bst_search() → Cek apakah sudah ada
    │       └── Jika ada → return 0 (gagal)
    │
    ├── bst_insert() → Sisipkan ke BST
    ├── hash_insert() → Sisipkan ke Hash Table
    ├── trie_insert() → Sisipkan ke Trie
    ├── queue_enqueue() → Tambah ke queue
    ├── total_words++
    │
    └── push undo marker (@ADD:nama_kata)
```

### 4.3 Update Kata

```
dict_update_word(dict, indonesian, new_word)
    │
    ├── bst_search() → Cari kata lama
    │
    ├── Simpan kata lama untuk undo
    │
    ├── bst_delete() → Hapus kata lama
    ├── bst_insert() → Sisipkan kata baru
    ├── hash_delete() + hash_insert() → Update Hash
    ├── Rebuild Trie (destroy + rebuild)
    │
    ├── push undo marker (@UPDATE:nama_kata + data lama)
    │
    └── return 1 (berhasil)
```

### 4.4 Hapus Kata

```
dict_delete_word(dict, indonesian)
    │
    ├── bst_search() → Cari kata
    │
    ├── Simpan kata untuk undo
    │
    ├── bst_delete() → Hapus dari BST
    ├── hash_delete() → Hapus dari Hash
    ├── Rebuild Trie
    ├── Total words--
    │
    ├── push undo marker (@DELETE:nama_kata + data kata)
    │
    └── return 1 (berhasil)
```

### 4.5 Pencarian Indonesia (BST)

```
dict_search_indonesian(dict, "makan")
    │
    ├── bst_search(bst_root, "makan")
    │       └── Recursive: kiri jika lebih kecil, kanan jika lebih besar
    │
    ├── Jika ketemu:
    │       ├── list_insert_front() → Tambah ke history
    │       └── dict_set_last_searched() → Simpan kata terakhir
    │
    └── return WordEntry* atau NULL
```

### 4.6 Pencarian Inggris (Hash Table)

```
dict_search_english(dict, "eat")
    │
    ├── hash_function("eat") → hitung index
    │
    ├── hash_search() → traverse linked list di bucket
    │       └── Bandingkan case-insensitive
    │
    ├── Jika ketemu:
    │       ├── list_insert_front() → Tambah ke history
    │       └── dict_set_last_searched() → Simpan kata terakhir
    │
    └── return WordEntry* atau NULL
```

### 4.7 Autocomplete (Trie)

```
dict_search_prefix(dict, "ma")
    │
    ├── trie_search_prefix(trie_root, "ma")
    │       └── Traverse: m → a
    │       └── Return node "ma"
    │
    ├── trie_get_all_with_prefix()
    │       └── Panggil trie_collect_all() secara rekursif
    │       └── DFS dari node "ma" → collect semua is_end_of_word
    │
    └── return array WordEntry[] + count
```

### 4.8 Undo (Enhanced dengan Marker)

```
dict_undo(dict)
    │
    ├── stack_pop() → Ambil undo marker dari stack
    │
    ├── Parse marker prefix (@ADD: / @UPDATE: / @DELETE:)
    │
    ├── Jika @ADD: → dict_delete_word_no_undo() → Hapus kata (undo add)
    │
    ├── Jika @UPDATE: → dict_add_word() → Restore kata lama
    │
    ├── Jika @DELETE: → dict_add_word() → Restore kata (undo delete)
    │
    ├── Tampilkan pesan sukses dengan nama kata
    │
    └── Tunggu user tekan Enter untuk kembali ke menu
```

---

## 5. Kompleksitas Algoritma

| Struktur Data | Insert | Search | Delete | Use Case |
|---------------|--------|--------|--------|----------|
| **BST** | O(log n) | O(log n) | O(log n) | Lookup Indonesia |
| **Hash Table** | O(1)* | O(1)* | O(1)* | Lookup Inggris |
| **Trie** | O(m) | O(m) | O(m) | Autocomplete |
| **Linked List** | O(1) | O(n) | O(n) | History |
| **Stack** | O(1) | - | O(1) | Undo |
| **Queue** | O(1) | O(1) | O(1) | Word of Day |

*m = panjang kata, * = average case

---

## 6. Kategori Kata dalam Dataset

### 6.1 Everyday (Sehari-hari)
Kata-kata umum yang digunakan dalam percakapan sehari-hari.
- Contoh: `makan`, `minum`, `buku`, `rumah`

### 6.2 Formal (Resmi)
Kata-kata yang digunakan dalam konteks formal atau akademis.
- Contoh: `implementasi`, `konfigurasi`, `evaluasi`

### 6.3 Slang (Gaul)
Kata-kata kekinian yang populer di kalangan anak muda.
- Contoh: `baper`, `gabut`, `ngopi`, `cuan`

### 6.4 Technical (Teknis)
Kata-kata dalam domain teknologi dan komputer.
- Contoh: `algoritma`, `variabel`, `debugging`

---

## 7. Fitur Unggulan untuk Presentasi

### 7.1 Multi-Structure Integration
Demonstrasikan bagaimana 6 struktur data bekerja BERSAMA:
```
User input "makan"
    ↓
BST: Apakah ada di dictionary?
    ↓
Hash: Simpan terjemahan "eat"
    ↓
Trie: Index untuk autocomplete "ma..."
    ↓
History: Catat pencarian
    ↓
Stack: Siapkan untuk undo
    ↓
Queue: Word of the Day
```

### 7.2 Data Synchronization
Saat menambah/menghapus/mengupdate kata, SEMUA struktur data harus tersinkron:
```c
int dict_add_word(DictionaryManager* dict, WordEntry word) {
    // 1. Cek apakah sudah ada di BST
    if (bst_search(dict->bst_root, word.indonesian) != NULL) {
        return 0;  // Gagal - sudah ada
    }

    // 2. Insert ke BST
    dict->bst_root = bst_insert(dict->bst_root, word);

    // 3. Insert ke Hash Table
    hash_insert(dict->hash_english, word);

    // 4. Insert ke Trie
    trie_insert(dict->trie_root, word.indonesian, word);

    // 5. Enqueue ke Queue
    queue_enqueue(dict->word_queue, word);

    // 6. Update counter
    dict->total_words++;

    // 7. Push undo marker
    WordEntry undo_marker = word;
    snprintf(marker_prefix, "@ADD:%s", word.indonesian);
    strncpy(undo_marker.indonesian, marker_prefix, MAX_WORD_LEN - 1);
    stack_push(dict->undo_stack, undo_marker);

    return 1;
}
```

### 7.3 Undo Mechanism (Enhanced dengan Marker)
Stack-based undo dengan marker untuk identifikasi aksi:
- **@ADD:** → Undo add = delete kata
- **@UPDATE:** → Undo update = restore kata lama
- **@DELETE:** → Undo delete = restore kata yang dihapus

Pesan sukses menunjukkan aksi apa yang di-undo.

### 7.4 Fitur Update Fleksibel
Pada fitur Update Word:
1. Pilih kata yang akan diupdate
2. Pilih field mana yang ingin diubah (1-7)
3. Tekan Enter untuk tidak mengubah, input simbol `-` untuk mengosongkan
4. Setelah selesai, tekan **0** untuk simpan dan keluar

### 7.5 Last Searched Word Display
Kata terakhir yang dicari ditampilkan di atas menu utama setiap kali menu utama ditampilkan.

### 7.6 View All Words dengan Filter
- Pagination: 20 kata per halaman
- Filter A-Z: Tekan huruf A-Z untuk filter berdasarkan awalan
- Pilih kata: Tekan nomor (1-N) untuk lihat detail
- Navigasi halaman: /1, /2, dst

---

## 8. Diagram Alur Menu

```
                    +------------------+
                    |   MENU UTAMA      |
                    | (dengan last word)|
                    +------------------+
                           |
    +----------+----------+----------+----------+----------+
    |          |          |          |          |          |
    v          v          v          v          v          v
[1.View] [2.Search ID] [3.Search EN] [4.Prefix] [5.Add] [6.Update]
    |          |          |          |          |          |
    v          v          v          v          v          v
 Pagination   BST       Hash       Trie       Input    Field Select
   A-Z      lookup    lookup    autocomplete  Form      (1-7, 0=save)
                          |
    +----------+----------+----------+
    |          |          |          |
    v          v          v          v
[7.Delete] [8.WoD] [9.History] [10.Undo]
    |          |          |          |
    v          v          v          v
Confirm    Enter      List View   Stack Pop
```

---

## 9. Tips untuk Presentasi

### Sesi Demonya:
1. **Tunjukkan menu utama** - 11 opsi fitur + last searched word
2. **Demo View All Words** - Pagination, filter A-Z, pilih kata (tekan 1-N)
3. **Demo pencarian Indonesia** - Gunakan BST, tunjukkan proses
4. **Demo pencarian Inggris** - Gunakan Hash, tunjukkan O(1)
5. **Demo autocomplete** - Ketik "ma", tunjukkan semua kata "ma..."
6. **Demo Add Word** - Validasi kata sudah ada, tekan Enter
7. **Demo Update Word** - Pilih field satu per satu, tekan Enter, simbol `-`
8. **Demo Delete Word** - Konfirmasi, sukses tekan Enter
9. **Demo Word of the Day** - Kata acak, tekan Enter
10. **Demo undo** - Add/Update/Delete, lalu undo, kata kembali/dihapus
11. **Demo statistics** - Tunjukkan distribusi POS & Category

### Slide Penjelasan:
1. **Slide 1:** Judul & overview project
2. **Slide 2:** Struktur file project
3. **Slide 3-8:** Satu slide per struktur data
4. **Slide 9:** Diagram integrasi + Undo mechanism (Enhanced)
5. **Slide 10:** Demo & Q&A

### Poin Penting untuk Disampaikan:
- **Kenapa 6 struktur data?** Masing-masing punya tujuan spesifik
- **Trade-offs:** BST (ordered) vs Hash (fast lookup)
- **Enhanced Undo:** Marker-based approach untuk identifikasi aksi
- **UX:** Tekan Enter setelah setiap operasi untuk konfirmasi
- **Update Fleksibel:** Pilih field satu per satu, tekan Enter untuk skip
- **Real-world application:** Dictionary sederhana tapi lengkap
- **Memory management:** Allokasi & dealokasi memori

---

## 10. Contoh Output Program

### Menu Utama:
```
  ============================================================
  =                     KATA TERAKHIR                        =
  ============================================================
  | Indonesian : rumah                                        |
  | English    : house                                       |
  | POS        : Noun                                        |
  +=========================================================+

  +----------------------------------------------------------+
  |              INDONESIAN - ENGLISH DICTIONARY             |
  |                       (IndoEng)                          |
  +----------------------------------------------------------+
  |  1. View All Words                - Paginated A-Z        |
  |  2. Search Word (Indonesian)      - BST Lookup           |
  |  3. Search Word (English)         - Hash Table Lookup    |
  |  4. Prefix Search (Autocomplete)  - Trie Lookup          |
  |  5. Add New Word                  - Create               |
  |  6. Update Word                   - Update               |
  |  7. Delete Word                   - Delete               |
  |  8. Word of the Day               - Random Pick          |
  |  9. Search History                - Recent Searches      |
  | 10. Undo Last Action              - Stack-based          |
  | 11. Statistics                    - Dictionary Stats     |
  |  0. Exit                                                 |
  +----------------------------------------------------------+

  Masukkan pilihan:
```

### Contoh Undo Berhasil:
```
  [Sukses] Undo berhasil! Menambahkan kata 'rumah' telah dikembalikan.

  [Tekan Enter untuk kembali ke menu utama]
  Pilihan:
```

### Contoh Update Word:
```
  +----------------------------------------------------------+
  |                    UPDATE: rumah                         |
  +----------------------------------------------------------+
  |  1. Indonesian   : rumah                                 |
  |  2. English      : house                                 |
  |  3. POS          : Noun                                  |
  |  4. Category     : Everyday                              |
  |  5. Pronounce    : [roo-mah]                              |
  |  6. Definition   : bangunan tempat tinggal                |
  |  7. Example      : Saya pergi ke rumah.                   |
  +----------------------------------------------------------+
  |                                                          |
  |  [1-7] Pilih field yang akan diubah                       |
  |  [0]   Simpan dan keluar                                  |
  |                                                          |
  +----------------------------------------------------------+

  Pilihan:
```

### Contoh View All Words:
```
  +----------------------------------------------------------+
  |  FILTER: [*] [A] [B] [C] ...                              |
  +----------------------------------------------------------+

      Filter: *      |      Halaman 1/8      |      Total: 150 kata

  +----+-------------------------+-------------------------+--------+-----------+
  | No | Indonesian              | English                 | POS    | Category  |
  +----+-------------------------+-------------------------+--------+-----------+
  | 1  | algoritma               | algorithm               | Noun   | Technical |
  | 2  | arsitektur              | architecture            | Noun   | Technical |
  | ...| ...                     | ...                     | ...    | ...       |
  +----+-------------------------+-------------------------+--------+-----------+

  +----------------------------------------------------------+
  |     [1-150] Pilih Kata    |  [A-Z] Awalan  |  [/1-8] Halaman      |
  |     [0] Tampilkan Semua   |  [<] Menu Utama                          |
  +----------------------------------------------------------+

  Pilihan:
```

---

## 11. Perbedaan Dokumentasi vs Kode

Dokumentasi ini telah diperbarui untuk mencerminkan kode aktual. Beberapa perubahan penting:

| Item | Dokumentasi Lama | Kode Aktual |
|------|------------------|-------------|
| UndoAction struct | Digunakan | **Dihapus** - menggunakan marker-based approach |
| hash_remove() | Tidak ada | **Ditambahkan** - untuk update English |
| bst_update_key() | Tidak ada | **Ditambahkan** - untuk update Indonesian |
| trie_collect_all() | Tidak ada | **Ditambahkan** - helper untuk Trie |
| trie_get_all_with_prefix() | Tidak ada | **Ditambahkan** - fungsi utama Trie |
| Jumlah kata | ~300+ | ~150+ |
| BST delete cases | 4 kasus | 3 kasus (doktrinasi digabung) |

---
