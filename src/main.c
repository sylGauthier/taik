#include <stdio.h>
#include <stdlib.h>

#include "taik.h"

unsigned int* load_highscores(const char* path) {
    FILE* f = NULL;
    unsigned int* ret = NULL;

    if (!(f = fopen(path, "r"))) {
        if (!(ret = malloc(sizeof(*ret)))) return NULL;
        ret[0] = 0;
    } else {
        unsigned int i = 1;
        unsigned int s;
        void* tmp;
        while (fscanf(f, "%u\n", &s) != EOF) {
            if (!(tmp = realloc(ret, (i + 1) * sizeof(*ret)))) {
                fprintf(stderr, "Error: can't realloc highscore array\n");
                free(ret);
                ret = NULL;
            }
            ret = tmp;
            ret[i - 1] = s;
            i++;
        }
        ret[i - 1] = 0;
    }
    if (f) fclose(f);
    return ret;
}

int write_highscore(const char* path, unsigned int* scores, unsigned int size) {
    FILE* f = NULL;
    unsigned int i;

    if (!(f = fopen(path, "w"))) return 0;
    for (i = 0; i < size; i++) {
        fprintf(f, "%u\n", scores[i]);
    }
    fclose(f);
    return 1;
}

int main(int argc, char** argv) {
    struct Taik taik;
    unsigned int level = 0, score = 0;
    unsigned int* highscores = NULL;
    char ok;

    if (!(highscores = load_highscores("save"))) return 0;
    for (; highscores[level] >= (level + 1) * 50; level++);
    if (!taik_init(&taik, level)) return 0;
    if (!taik_parse_options(&taik, argc, argv)) return 0;

    ok = taik_run(&taik, &score);
    if (score > highscores[level]) {
        highscores[level] = score;
        printf("HIGHSCORE: %u\n", score);
        if (score >= (level + 1) * 50) {
            printf("LEVEL %u UNLOCKED\n", level + 1);
        }
    }
    write_highscore("save", highscores, level + 1);

    taik_free(&taik);
    return !ok;
}
