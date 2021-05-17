// 7date.c
//
// A utility to print out dates in the 7date calendar.
// The 7date is based on the day-of-year written in base 7, starting with 0 for
// January 1st. If this calendar is new to you, please see:
//
// http://tylerneylon.com/a/7date_spec/
// http://tylerneylon.com/a/7date/
//
// Simply running:
//
//   7date
//
// will print out today's 7date based on your system time and time zone.
//
// These options are available:
//
//   -d        Use digital notation; better for sorting / filtering.
//   --week    Print with a precision of weeks; for example, 12-.2016.
//   --7month  Print with a precision of 7months; for example, 1--.2016.
//
// Finally, you can also specify a file's path as an argument, and the
// modification time for that file will be displayed as a 7date instead of the
// system time.
//
// 7date examples:
//
//  Jan 1, 1925  ==  0.1925
//
//  Feb 1, 2025  ==  43.2025
//

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define BUF_SIZE 64


// Support macOS and Linux.
// (I have not even tried to compile this on Windows,
//  so I'm guessing it would need some modifcations there.)
#if defined(__linux__)
    #define MODIFIED_TIME st_mtime
#else
    #define MODIFIED_TIME st_mtimespec.tv_sec
#endif


// Internal types and globals.

typedef enum {
  scope_day,
  scope_week,
  scope_7month
} Scope;

static Scope scope = scope_day;

int do_use_digital_form = 0;

char *filepath = NULL;


// Internal functions.

static void print_usage() {
  printf("Usage:\n\n");
  printf("  7date [-d] [--week|--7month] [filepath]\n\n");
  printf("Available options:\n");
  printf("  -d        Use digital notation; better for sorting / filtering.\n");
  printf("  --week    Print with week precision; for example, 12-.2016.\n");
  printf("  --7month  Print with 7month precision; for example, 1--.2016.\n");
}

// Returns a base 7 string representation of the given number.
// The returned value should be copied unless used before the
// next call to base_7_str.
static char *base_7_str(int n) {
  if (n < 0) return "";
  if (n == 0) return "0";
  static char buf[BUF_SIZE];
  char *c = buf + (BUF_SIZE - 1);
  *c-- = '\0';
  while (n > 0 && c >= buf) {
    *c-- = '0' + n % 7;
    n /= 7;
  }
  if (c < buf) return "";
  return ++c;
}

static char *to_scope(char *date_str, Scope scope) {
  static char out_str[5];
  char *prefix = "00";
  int prefix_len = scope - strlen(date_str) + 1;
  if (prefix_len < 0) prefix_len = 0;
  strcpy(out_str, prefix + 2 - prefix_len);
  strcat(out_str, date_str);
  // Change the last `scope` character to '-'.
  for (int i = 0; i < scope; ++i) out_str[strlen(out_str) - 1 - i] = '-';
  return out_str;
}

// Zero-pads the given string up to four characters.
static char *zero_pad(char *str) {
  static char out_str[5];
  strcpy(out_str, "0000");
  strcpy(out_str + 4 - strlen(str), str);
  return out_str;
}

int main(int argc, char **argv) {

  // Parse the command-line arguments.
  while (argc > 1) {
    if (strcmp(argv[1], "--week") == 0) {
      scope = scope_week;
    } else if (strcmp(argv[1], "--7month") == 0) {
      scope = scope_7month;
    } else if (strcmp(argv[1], "-d") == 0) {
      do_use_digital_form = 1;
    } else {
      if (filepath) {
        // Unrecognized argument.
        printf("!Unrecognized argument: '%s'\n", argv[1]);
        print_usage();
        return 2;  // 2 --> indicate an argument error
      }
      filepath = argv[1];
    }
    argc--;
    argv++;
  }

  // This will hold the time to display.
  // The value will either be from a file or the system clock depending on
  // whether or not the user specified a filepath argument.
  time_t time_sec;

  if (filepath) {
    struct stat st;
    int err = stat(filepath, &st);
    if (err) {
      printf("!Error reading the file info for '%s'\n", filepath);
      print_usage();
      return 1;  // 1 --> application error
    }
    time_sec = st.MODIFIED_TIME;
  } else {
    time(&time_sec);
  }

  struct tm *t = localtime(&time_sec);
  char *day_part = to_scope(base_7_str(t->tm_yday), scope);

  if (do_use_digital_form) {
    // Print the digital notation: <year>-<4char 7day>
    printf("%d-%s\n", t->tm_year + 1900, zero_pad(day_part));
  } else {
    // Print the standard notation: <7day>.<year>
    printf("%s.%d\n", day_part, t->tm_year + 1900);
  }
  return 0;
}
