/*
 * ============================================================
 *  POOHKAO'S GUITAR LAB  -  Blues & Rock, Phuket, Thailand
 *  Beginner to Cracked Developer
 * ============================================================
 *  Concepts used:
 *    variables & types | arrays | 2D arrays | conditions
 *    strings | for loops | while loops | functions | static
 *
 *  Little algorithms inside:
 *    - modular arithmetic (music theory on a 12-note circle)
 *    - bubble sort (songs by difficulty)
 *    - closest-match search (pick your next song)
 *    - max / sum / average (practice log)
 *
 *  Compile:  gcc poohkao_guitar_lab.c -o guitar_lab
 *  Run:      ./guitar_lab
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM_NOTES   12
#define NUM_SONGS   10
#define NUM_DAYS    7
#define NUM_STRINGS 6
#define LINE_LEN    100
#define TITLE_LEN   40
#define ARTIST_LEN  32

/* ---------- DATA ---------- */

/* The 12 notes on a circle. Index math wraps around with % 12 */
const char notes[NUM_NOTES][3] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

/* Standard tuning shown high e -> low E, as note indexes */
const char string_names[NUM_STRINGS + 1] = "eBGDAE";
const int  tuning[NUM_STRINGS] = {4, 11, 7, 2, 9, 4};

/* Minor pentatonic = the guitarist's best friend (semitones from root) */
const int pentatonic[5] = {0, 3, 5, 7, 10};

/* 12-bar blues: 3 rows x 4 bars, each number = semitones above the key */
const int blues_steps[3][4] = {
    {0, 0, 0, 0},     /* I   I   I   I  */
    {5, 5, 0, 0},     /* IV  IV  I   I  */
    {7, 5, 0, 7}      /* V   IV  I   V  */
};

/* Open chord shapes that are easy to play: C D E G A */
const int easy_shapes[5] = {0, 2, 4, 7, 9};

/* Song library (parallel arrays, same index = same song) */
const char song_title[NUM_SONGS][TITLE_LEN] = {
    "Sweet Home Chicago",
    "Pride and Joy",
    "Stormy Monday",
    "The Thrill Is Gone",
    "Smoke on the Water",
    "Crossroads",
    "Sweet Child O' Mine",
    "Hotel California",
    "Voodoo Child (Slight Return)",
    "Layla"
};
const char song_artist[NUM_SONGS][ARTIST_LEN] = {
    "Robert Johnson",
    "Stevie Ray Vaughan",
    "T-Bone Walker",
    "B.B. King",
    "Deep Purple",
    "Cream",
    "Guns N' Roses",
    "Eagles",
    "Jimi Hendrix",
    "Derek and the Dominos"
};
/* My own rough difficulty guess, 1 (easy) to 10 (hard) */
const int song_level[NUM_SONGS] = {3, 5, 6, 5, 2, 7, 7, 8, 9, 9};
/* 0 = blues, 1 = rock */
const int song_genre[NUM_SONGS] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1};
const char genre_names[2][8] = {"Blues", "Rock"};

/* Weekly practice minutes (static = private to this file) */
static int practice_minutes[NUM_DAYS];
const char days[NUM_DAYS][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

/* ---------- HELPERS ---------- */

static void print_line(char c, int length) {
    for (int i = 0; i < length; i++) {
        putchar(c);
    }
    putchar('\n');
}

static void read_text(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        exit(0);
    }
    buf[strcspn(buf, "\n")] = '\0';
}

/* Keep asking until the number is between min and max */
static int read_int(const char *prompt, int min, int max) {
    char line[LINE_LEN];
    int value = 0;
    int valid = 0;

    while (!valid) {
        read_text(prompt, line, LINE_LEN);
        if (sscanf(line, "%d", &value) == 1 && value >= min && value <= max) {
            valid = 1;
        } else {
            printf("  Please type a number from %d to %d.\n", min, max);
        }
    }
    return value;
}

void to_upper_copy(const char *src, char *dest) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = (char)toupper((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

/* Turn "a" or "F#" into a note index 0-11, or -1 if it isn't a note */
int find_note(const char *text) {
    char up[8];
    if (strlen(text) > 2) {
        return -1;
    }
    to_upper_copy(text, up);
    for (int i = 0; i < NUM_NOTES; i++) {
        if (strcmp(notes[i], up) == 0) {
            return i;
        }
    }
    return -1;
}

static int ask_key(void) {
    char line[LINE_LEN];
    int idx = -1;

    while (idx == -1) {
        read_text("  Key (C C# D D# E F F# G G# A A# B): ", line, LINE_LEN);
        idx = find_note(line);
        if (idx == -1) {
            printf("  Not a note. Use sharps like F# (not flats).\n");
        }
    }
    return idx;
}

static void print_difficulty_bar(int level) {
    for (int d = 0; d < 10; d++) {
        putchar(d < level ? '#' : '.');
    }
}

int total_minutes(void) {
    int total = 0;
    for (int i = 0; i < NUM_DAYS; i++) {
        total += practice_minutes[i];
    }
    return total;
}

const char *guitar_rank(int minutes) {
    if (minutes < 30) {
        return "Bedroom Strummer";
    } else if (minutes < 120) {
        return "Open-Mic Hopeful";
    } else if (minutes < 300) {
        return "Patong Bar Regular";
    }
    return "PHUKET BLUES LEGEND";
}

/* ---------- MENU ACTIONS ---------- */

/* 1) 12-bar blues in any key (modular arithmetic) */
static void blues_12_bar(void) {
    int root = ask_key();
    char chord[8];

    printf("\n  12-BAR BLUES in %s\n", notes[root]);
    print_line('-', 40);
    for (int row = 0; row < 3; row++) {
        printf("  |");
        for (int col = 0; col < 4; col++) {
            snprintf(chord, sizeof(chord), "%s7",
                     notes[(root + blues_steps[row][col]) % NUM_NOTES]);
            printf(" %-4s|", chord);
        }
        printf("\n");
    }
    printf("  I = %s7   IV = %s7   V = %s7\n",
           notes[root],
           notes[(root + 5) % NUM_NOTES],
           notes[(root + 7) % NUM_NOTES]);
    printf("  Try a shuffle feel and count 1-2-3-4 each bar.\n");
}

/* 2) Fretboard map of the minor pentatonic scale */
static void pentatonic_map(void) {
    int root = ask_key();
    int in_scale[NUM_NOTES] = {0};

    for (int i = 0; i < 5; i++) {
        in_scale[(root + pentatonic[i]) % NUM_NOTES] = 1;
    }

    printf("\n  %s MINOR PENTATONIC:", notes[root]);
    for (int i = 0; i < 5; i++) {
        printf(" %s", notes[(root + pentatonic[i]) % NUM_NOTES]);
    }
    printf("\n  (R) = root   o = scale note\n\n");

    printf("   ");
    for (int fret = 0; fret <= 12; fret++) {
        printf("%-3d", fret);
    }
    printf("\n");

    for (int s = 0; s < NUM_STRINGS; s++) {
        printf("%c |", string_names[s]);
        for (int fret = 0; fret <= 12; fret++) {
            int n = (tuning[s] + fret) % NUM_NOTES;
            if (n == root) {
                printf("(R)");
            } else if (in_scale[n]) {
                printf(" o ");
            } else {
                printf(" - ");
            }
        }
        printf("\n");
    }
}

/* 3) Capo helper: which capo fret gives easy open shapes? */
static void capo_helper(void) {
    int key = ask_key();

    printf("\n  Song in %s. Easy ways to play it:\n", notes[key]);
    print_line('-', 40);
    for (int capo = 0; capo <= 7; capo++) {
        int shape = (key - capo + NUM_NOTES) % NUM_NOTES;
        for (int j = 0; j < 5; j++) {
            if (easy_shapes[j] == shape) {
                printf("  Capo %d -> play %s shapes\n", capo, notes[shape]);
            }
        }
    }
}

/* 4) Song library sorted by difficulty (bubble sort) */
static void song_library(void) {
    int order[NUM_SONGS];
    for (int i = 0; i < NUM_SONGS; i++) {
        order[i] = i;
    }

    /* Bubble sort: swap neighbors until the easiest songs float to the top */
    for (int i = 0; i < NUM_SONGS - 1; i++) {
        for (int j = 0; j < NUM_SONGS - 1 - i; j++) {
            if (song_level[order[j]] > song_level[order[j + 1]]) {
                int tmp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = tmp;
            }
        }
    }

    printf("\n  SONG LIBRARY (easy to hard)\n");
    print_line('-', 60);
    for (int k = 0; k < NUM_SONGS; k++) {
        int s = order[k];
        printf("  %-28s %-22s ", song_title[s], song_artist[s]);
        print_difficulty_bar(song_level[s]);
        printf(" %s\n", genre_names[song_genre[s]]);
    }
}

/* 5) Closest-match search: find the song nearest to your skill level */
static void pick_song(void) {
    int skill = read_int("  Your skill level (1-10)? ", 1, 10);
    int genre = read_int("  1) Blues  2) Rock  3) Surprise me: ", 1, 3);
    int best = -1;
    int best_gap = 99;

    for (int i = 0; i < NUM_SONGS; i++) {
        int matches = (genre == 3) || (song_genre[i] == genre - 1);
        int gap = abs(song_level[i] - skill);
        if (matches && gap < best_gap) {
            best_gap = gap;
            best = i;
        }
    }

    printf("\n  Your next song: \"%s\" by %s\n",
           song_title[best], song_artist[best]);
    if (song_level[best] > skill) {
        printf("  A stretch! Start at 60%% speed.\n");
    } else if (song_level[best] < skill) {
        printf("  Easy win. Add your own bends and vibrato.\n");
    } else {
        printf("  Perfect match for your level.\n");
    }
}

/* 6) Tempo math with doubles */
static void tempo_helper(void) {
    int bpm = read_int("  Tempo in BPM (40-220)? ", 40, 220);
    double ms_per_beat = 60000.0 / bpm;
    double bar_seconds = 4.0 * 60.0 / bpm;
    double chorus_seconds = 12.0 * bar_seconds;

    printf("\n  One beat   : %.0f ms\n", ms_per_beat);
    printf("  One bar    : %.1f sec\n", bar_seconds);
    printf("  12-bar run : %.1f sec\n", chorus_seconds);
    printf("  Feel       : ");
    if (bpm < 70) {
        printf("slow, sweaty blues\n");
    } else if (bpm < 110) {
        printf("mid-tempo groove\n");
    } else if (bpm < 160) {
        printf("rock energy\n");
    } else {
        printf("shred zone!\n");
    }

    printf("  Count      : ");
    for (int beat = 1; beat <= 4; beat++) {
        printf(beat == 1 ? "[%d] " : " %d  ", beat);
    }
    printf("\n");
}

/* Weekly chart: one # per 10 minutes, plus total / best day / average */
static void show_week(void) {
    int best_day = 0;

    printf("\n  THIS WEEK\n");
    print_line('-', 40);
    for (int i = 0; i < NUM_DAYS; i++) {
        printf("  %s %3d min ", days[i], practice_minutes[i]);
        for (int b = 0; b < practice_minutes[i] / 10; b++) {
            putchar('#');
        }
        printf("\n");
        if (practice_minutes[i] > practice_minutes[best_day]) {
            best_day = i;
        }
    }
    printf("  Total: %d min | Average: %.1f min/day\n",
           total_minutes(), (float)total_minutes() / NUM_DAYS);
    if (practice_minutes[best_day] > 0) {
        printf("  Best day: %s\n", days[best_day]);
    }
}

/* 7) Practice log: static counter remembers sessions between calls */
static void practice_log(void) {
    static int sessions = 0;

    int day  = read_int("  Which day (1=Mon ... 7=Sun)? ", 1, 7);
    int mins = read_int("  Minutes practiced (1-300)? ", 1, 300);

    practice_minutes[day - 1] += mins;
    sessions++;

    printf("  Logged! That's session #%d this run.\n", sessions);
    show_week();
}

/* ---------- MAIN ---------- */

int main(void) {
    const char name[] = "Poohkao";
    int  choice = -1;
    char grade;

    print_line('=', 46);
    printf("   %s'S GUITAR LAB\n", "POOHKAO");
    printf("   Blues & Rock | Phuket, Thailand\n");
    print_line('=', 46);
    printf("\nSawasdee, %s! Grab your guitar.\n", name);

    while (choice != 0) {
        printf("\n");
        print_line('=', 46);
        printf("  1) 12-bar blues       2) Pentatonic map\n");
        printf("  3) Capo helper        4) Song library\n");
        printf("  5) Pick my next song  6) Tempo math\n");
        printf("  7) Practice log       0) Finish\n");
        print_line('=', 46);

        choice = read_int("Choose: ", 0, 7);

        if (choice == 1) {
            blues_12_bar();
        } else if (choice == 2) {
            pentatonic_map();
        } else if (choice == 3) {
            capo_helper();
        } else if (choice == 4) {
            song_library();
        } else if (choice == 5) {
            pick_song();
        } else if (choice == 6) {
            tempo_helper();
        } else if (choice == 7) {
            practice_log();
        }
    }

    /* Final summary */
    int total = total_minutes();
    if (total >= 300) {
        grade = 'A';
    } else if (total >= 120) {
        grade = 'B';
    } else if (total >= 30) {
        grade = 'C';
    } else {
        grade = 'D';
    }

    printf("\n");
    print_line('=', 46);
    printf("  SESSION SUMMARY FOR %s\n", name);
    print_line('=', 46);
    printf("  Practice time: %d min (%.1f hours)\n", total, total / 60.0f);
    printf("  Grade: %c\n", grade);
    printf("  Rank : %s\n", guitar_rank(total));
    printf("\n  Keep jamming at sunset, %s!\n\n", name);

    return 0;
}