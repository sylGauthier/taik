#include <stdio.h>

#include "taik.h"

int main(int argc, char** argv) {
    struct Taik taik;
    char ok;

    if (!taik_init(&taik)) return 0;
    if (!taik_parse_options(&taik, argc, argv)) return 0;
    ok = taik_run(&taik);
    taik_free(&taik);

    return !ok;
}
