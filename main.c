#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#define LOW_DENSITY 0.2
#define MED_DENSITY 0.35
#define HIGH_DENSITY 0.5

enum word_status {
    VERTICAL = 0,
    HORIZONTAL = 1
};

typedef struct {
    char** table;
    int size;
    int filled;
    double density;
} crossword_data;

void cleanup(crossword_data* crossword) {
    if (crossword->table != NULL) {
        for (int i = 0; i < crossword->size; i++) {
            free((crossword->table)[i]);
        }
        free(crossword->table);
        crossword->table = NULL;
    }
}

void print_res(crossword_data* crossword) {
    for (int i = 0; i < crossword->size; i++) {
        for (int k = 0; k < crossword->size * 4 + 1; k++) {
            printf("-");
        }
        printf("\n|");
        for (int j = 0; j < crossword->size; j++) {
            if ((crossword->table)[i][j] < 3 || (crossword->table)[i][j] == ' ')
                printf("   |");
            else 
                printf(" %c |", (crossword->table)[i][j]);
        }
        printf("\n");
    }
    for (int k = 0; k < crossword->size * 4 + 1; k++) {
        printf("-");
    }
    printf("\n");
}

int find_place(crossword_data* crossword, char* word, enum word_status place, int* letters) {
    int len = strlen(word);
    int crossings;
    int flag;
    int ii;
    int jj;
    int kk;
    for (int i = 0; i < len; i++) {
        if (letters[word[i] - 'A']) { // Проверяем, использовалась ли эта буква
            for (int j = 0; j < crossword->size; j++) {
                for (int k = 0; k < crossword->size; k++) {
                    jj = j - i * !(int)place;
                    kk = k - i * (int)place;
                    if (crossword->table[j][k] == word[i] && jj >= 0 && kk >= 0) { // Нашли пересечение и есть место для слова
                        crossings = 0;
                        flag = 1;
                        ii = 0;
                        if (place == HORIZONTAL && ((kk > 0 && crossword->table[jj][kk - 1] == 0) || kk == 0) && ((kk + len < crossword->size && crossword->table[jj][kk + len] == 0) || kk + len == crossword->size)) {
                            for (kk = k - i; kk < crossword->size && ii < len; kk++) {
                                if (!(crossword->table[jj][kk] == word[ii] || (crossword->table[jj][kk] == 0 && (jj == 0 || crossword->table[jj - 1][kk] == 0) && (jj == crossword->size - 1 || crossword->table[jj + 1][kk] == 0)))) {
                                    flag = 0;
                                    break;
                                }
                                if (crossword->table[jj][kk] == word[ii]) {
                                    crossings++;
                                }
                                ii++;
                            }
                            if (flag) {
                                jj = j;
                                kk = k - i;
                                for (ii = 0; ii < len; ii++) {
                                    crossword->table[jj][kk + ii] = word[ii];
                                    letters[word[ii] - 'A'] += 1;
                                }
                                return len - crossings;
                            }
                        }
                        else if (place == VERTICAL && ((jj > 0 && crossword->table[jj - 1][kk] == 0) || jj == 0) && ((jj + len < crossword->size && crossword->table[jj + len][kk] == 0) || jj + len == crossword->size)) {
                            for (jj = j - i; jj < crossword->size && ii < len; jj++) {
                                if (!(crossword->table[jj][kk] == word[ii] || (crossword->table[jj][kk] == 0 && (kk == 0 || crossword->table[jj][kk - 1] == 0) && (kk == crossword->size - 1 || crossword->table[jj][kk + 1] == 0)))) {
                                    flag = 0;
                                    break;
                                }
                                if (crossword->table[jj][kk] == word[ii]) {
                                    crossings++;
                                }
                                ii++;
                            }
                            if (flag) {
                                jj = j - i;
                                kk = k;
                                for (ii = 0; ii < len; ii++) {
                                    crossword->table[jj + ii][kk] = word[ii];
                                    letters[word[ii] - 'A'] += 1;
                                }
                                return len - crossings;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int main() {
    char command[20];
    char inp_word[20];
    int letters[26];
    int inp_size = 0;
    int inp_dens = 0;
    int result = 0;
    clock_t start, stop;
    FILE* words;

    crossword_data crossword;
    crossword.filled = 0;
    crossword.table = NULL;

    memset(letters, 0, sizeof(letters));

    setlocale(LC_ALL, "Rus");
    printf("Генератор кроссвордов\n");
    printf("Введите \"help\", чтобы увидеть доступные команды\n");
    printf("Введите команду:\n");
    scanf("%20s", command);

    while (strcmp(command, "finish") != 0) {
        if (strcmp(command, "help") == 0) {
            printf("Доступные команды:\n1.\"create\": создает новый кроссворд\n2. \"finish\": завершает программу\n");
        } else if (strcmp(command, "create") == 0) {
            cleanup(&crossword);
            printf("Введите размер кроссворда:\n");
            scanf("%d", &inp_size);
            crossword.size = inp_size;
            printf("Выберите плотность кроссворда:\n1.Низкая: 20%%\n2.Средняя: 35%%\n3.Высокая: 50%%\n");
            scanf("%d", &inp_dens);
            switch (inp_dens) {
                case 1:
                    crossword.density = LOW_DENSITY;
                    break;
                case 2:
                    crossword.density = MED_DENSITY;
                    break;
                case 3:
                    crossword.density = HIGH_DENSITY;
                    break;
                default:
                    printf("Неверный ввод. Средняя плотность установлена по умолчанию.\n");
                    crossword.density = MED_DENSITY;
                    break;
            }

            crossword.table = (char**)calloc((size_t)crossword.size, sizeof(char*));
            for (unsigned i = 0; i < crossword.size; i++) {
                (crossword.table)[i] = (char*)calloc((size_t)crossword.size, sizeof(char));
            }

            enum word_status place = VERTICAL;
            words = fopen("wordbase-eng.txt", "r");
            fscanf(words, "%20s", inp_word);
            start = clock();
            
            // Ставим первую букву для размещения первого слова
            crossword.table[crossword.size / 2 - 1][0] = inp_word[0];
            letters[inp_word[0] - 'A'] += 1;
            crossword.filled = 1;
            crossword.filled += find_place(&crossword, inp_word, HORIZONTAL, letters);

            while ((double)crossword.filled / (double)(crossword.size * crossword.size) < crossword.density && !feof(words)) {
                fscanf(words, "%20s", inp_word);
                result = find_place(&crossword, inp_word, place, letters);
                if (result) {
                    crossword.filled += result; // Прибавляем новые заполненные клетки
                    place = (place + 1) % 2; // Следующее слово ставим по другому
                }
            }
            stop = clock();
            print_res(&crossword);
            printf("Time: %lf\n", (double)(stop - start) / (double)CLOCKS_PER_SEC);
        }
        scanf("%20s", command);
    }
    cleanup(&crossword);
    return 0;
}