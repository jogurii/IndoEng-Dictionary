# 🇮🇩🇬🇧 IndoEng Dictionary

<p align="center">
  <img src="https://img.shields.io/badge/Language-C-blue.svg" alt="Language: C">
  <img src="https://img.shields.io/badge/Standard-C99-green.svg" alt="Standard: C99">
  <img src="https://img.shields.io/badge/Compiler-GCC-yellow.svg" alt="Compiler: GCC">
  <img src="https://img.shields.io/badge/Words-300%2B-orange.svg" alt="Words: 300+">
</p>

<p align="center">
  Aplikasi kamus dwibahasa Indonesia-Inggris berbasis console dengan 6 struktur data modern
</p>

---

## 📋 Daftar Isi

- [Tentang Project](#-tentang-project)
- [Fitur](#-fitur)
- [Struktur Data](#-struktur-data)
- [Struktur File](#-struktur-file)
- [Instalasi](#-instalasi)
- [Penggunaan](#-penggunaan)
- [Screenshot](#-screenshot)
- [Kontributor](#-kontributor)

---

## 🎯 Tentang Project

**IndoEng Dictionary** adalah aplikasi kamus dwibahasa Indonesia-Inggris berbasis teks (console) yang dibangun menggunakan bahasa pemrograman C. Aplikasi ini menggunakan **6 jenis struktur data** untuk penyimpanan dan pencarian kata yang efisien.

### Spesifikasi Teknis

| Komponen | Detail |
|----------|--------|
| **Bahasa** | C (C99 Standard) |
| **Platform** | Console/Terminal |
| **Compiler** | GCC |
| **Jumlah Kata** | 300+ pasang kata |
| **Struktur Data** | 6 jenis |

---

## ✨ Fitur

### Operasi CRUD
- ➕ **Tambah Kata** - Tambahkan pasangan kata baru
- 🔍 **Cari Kata Indonesia** - Pencarian berdasarkan kata Indonesia
- 🔍 **Cari Kata Inggris** - Pencarian berdasarkan kata Inggris
- ✏️ **Update Kata** - Ubah definisi, contoh, atau informasi kata
- 🗑️ **Hapus Kata** - Hapus entri kata dengan konfirmasi

### Fitur Lanjutan
- 🔤 **Prefix Search** - Autocomplete untuk pencarian kata
- 📜 **Riwayat Pencarian** - Lacak 20 pencarian terakhir
- ↩️ **Undo** - Batalkan operasi hapus/update terakhir
- 🎲 **Word of the Day** - Kata acak setiap hari
- 📊 **Statistik** - Lihat statistik dictionary
- 📖 **Lihat Semua Kata** - Tampilkan semua kata (dengan pagination)

---

## 🏗️ Struktur Data

Project ini mengimplementasikan 6 struktur data utama:

| No | Struktur Data | Penggunaan | Kompleksitas |
|----|---------------|------------|--------------|
| 1 | **Linked List** | Riwayat pencarian, operasi undo | O(n) |
| 2 | **Binary Search Tree (BST)** | Penyimpanan kata Indonesia (terurut abjad) | O(log n) |
| 3 | **Hash Table** | Pencarian cepat kata Inggris | O(1) avg |
| 4 | **Trie (Prefix Tree)** | Autocomplete & prefix search | O(m) |
| 5 | **Stack** | Operasi undo (LIFO) | O(1) |
| 6 | **Queue** | Word of the day, proses bulk import | O(1) |

### Optimasi Performa

| Operasi | Sebelum | Sesudah | Peningkatan |
|---------|---------|---------|-------------|
| Trie Update | O(n) full rebuild | O(k) incremental | ~300x faster |
| Trie Delete | O(n) full rebuild | O(k) incremental | ~300x faster |
| Hash Delete | Memory leak | Safe | Fixed |

---

## 📁 Struktur File

```
indoeng-dictionary/
├── README.md                 # Dokumentasi utama
├── SPEC.md                   # Spesifikasi project
├── Makefile                  # Konfigurasi build
├── DOKUMENTASI_PRESENTASI.md # Dokumentasi presentasi
├── src/
│   ├── main.c                # Entry point & menu interaktif
│   ├── dictionary.c          # Logika utama dictionary manager
│   ├── dictionary.h          # Header dictionary
│   ├── data_structures.c     # Implementasi 6 struktur data
│   ├── data_structures.h     # Deklarasi struktur data
│   ├── ui_utils.c            # Utilitas tampilan UI
│   ├── ui_utils.h            # Header utilitas UI
│   └── word_data.c           # Dataset 300+ kata
├── build/
│   └── indoeng-dictionary.exe # File executable
└── .vscode/
    └── tasks.json            # Konfigurasi VS Code tasks
```

---

## 🔧 Instalasi

### Prasyarat
- GCC Compiler (MinGW untuk Windows)
- Make utility

### Langkah Instalasi

1. **Clone repository ini**
```bash
git clone https://github.com/username/indoeng-dictionary.git
cd indoeng-dictionary
```

2. **Build project**
```bash
make
```

3. **Jalankan program**
```bash
make run
```

### Perintah Makefile

| Perintah | Fungsi |
|----------|--------|
| `make` | Build project |
| `make run` | Build dan jalankan |
| `make clean` | Hapus file build |
| `make test` | Jalankan test |
| `make help` | Tampilkan bantuan |

---

## 📖 Penggunaan

### Menu Utama

```
╔══════════════════════════════════════════════════════════════╗
║         INDONESIAN - ENGLISH DICTIONARY (IndoEng)              ║
╠══════════════════════════════════════════════════════════════╣
║  1. View All Words                     - Paginated A-Z        ║
║  2. Search Word (Indonesian)           - BST Lookup           ║
║  3. Search Word (English)              - Hash Table Lookup    ║
║  4. Prefix Search (Autocomplete)       - Trie Lookup          ║
║  5. Add New Word                       - Create               ║
║  6. Update Word                        - Update               ║
║  7. Delete Word                        - Delete               ║
║  8. Word of the Day                    - Random Pick          ║
║  9. Search History                     - Recent Searches      ║
║ 10. Undo Last Action                   - Stack-based          ║
║ 11. Statistics                         - Dictionary Stats     ║
║  0. Exit                                                      ║
╚══════════════════════════════════════════════════════════════╝
```

### Contoh Penggunaan

1. **Mencari kata Indonesia**
   ```
   Pilih menu: 2
   Masukkan kata Indonesia: rumah
   ```
   Hasil: Menampilkan informasi kata "rumah" menggunakan BST

2. **Mencari kata Inggris**
   ```
   Pilih menu: 3
   Masukkan kata Inggris: house
   ```
   Hasil: Menampilkan informasi kata "house" menggunakan Hash Table

3. **Prefix Search (Autocomplete)**
   ```
   Pilih menu: 4
   Masukkan prefix: rum
   ```
   Hasil: Menampilkan semua kata yang dimulai dengan "rum"

---

## 🖼️ Screenshot

**Menu Utama:**
```
╔══════════════════════════════════════════════════════════════╗
║         INDONESIAN - ENGLISH DICTIONARY (IndoEng)             ║
╠══════════════════════════════════════════════════════════════╣
║  1. View All Words                                           ║
║  2. Search Word (Indonesian) - BST Lookup                    ║
║  3. Search Word (English)    - Hash Table Lookup             ║
║  4. Prefix Search           - Trie Lookup                   ║
║  5. Add New Word                                             ║
║  6. Update Word                                              ║
║  7. Delete Word                                              ║
║  8. Word of the Day                                          ║
║  9. Search History                                           ║
║ 10. Undo Last Action                                         ║
║ 11. Statistics                                               ║
║  0. Exit                                                     ║
╚══════════════════════════════════════════════════════════════╝
```

---

## 📚 Dokumentasi Tambahan

- [SPEC.md](./SPEC.md) - Spesifikasi teknis lengkap
- [DOKUMENTASI_PRESENTASI.md](./DOKUMENTASI_PRESENTASI.md) - Dokumentasi untuk presentasi

---

## 🔄 Riwayat Perubahan

### v1.1 (Refactoring)
- ✅ Optimasi trie update/delete: O(n) → O(k)
- ✅ Perbaikan memory leak pada hash_delete
- ✅ Refaktor UI utilities: hapus ~200 baris kode duplikat
- ✅ Penghapusan kode yang tidak digunakan

### v1.0
- Rilis awal dengan 6 struktur data

---

## 👨‍💻 Kontributor

Project ini dibuat untuk tugas **Struktur Data**.

**Struktur Data yang Digunakan:**
1. Linked List
2. Binary Search Tree (BST)
3. Hash Table
4. Trie
5. Stack
6. Queue

---

## 📄 Lisensi

Project ini menggunakan lisensi MIT.

---

<p align="center">
  Dibuat dengan ❤️ menggunakan C
</p>