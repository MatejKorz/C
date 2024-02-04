#include "constants.h"
#include "currency.h"
#include "decimals.h"
#include "errors.h"
#include "persons.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int semicolon(int letter)
{
    return letter == ';';
}

void load_currency_table(struct currency_table *table, FILE *input)
{
    stack_frame();
    char *line = NULL;

    if (read_error_point()) {
        free(line);
        leave();
    }

    for (; !feof(input); free(line)) {
        line = read_line(input);
        if (empty_string(line))
            continue;
        char *end;

        int full_length = strlen(line);
        char *name = trim_string(line, NULL);
        end = words_end(name);
        *end = '\0';

        OP((line + full_length) != end, MISSING_RATING);
        char *rating = trim_string(end + 1, NULL);
        OP(!strchr(rating, ' '), EXTRA_TERM);
        end = words_end(rating);
        *end = '\0';
        OP(!empty_string(rating), MISSING_RATING);
        OP(correct_num_format(rating) == 0, INVALID_NUMBER);

        add_currency(table, name, load_decimal(rating, RATING_DECIMALS));
    }
    leave();
}

void load_persons(struct persons *persons, FILE *input)
{
    stack_frame();
    char *line = NULL;

    if (read_error_point()) {
        free(line);
        leave();
    }

    for (; !feof(input); free(line)) {
        line = read_line(input);
        if (empty_string(line))
            continue;
        char *end;

        int full_length = strlen(line);
        char *id = trim_string(line, NULL);
        end = words_end(id);
        *end = '\0';

        OP((line + full_length) != end, MISSING_NAME);
        char *name = trim_string(end + 1, &end);
        *end = '\0';

        add_person(persons, id, name);
    }
    leave();
}

void load_payments(struct persons *persons, struct currency_table *table, FILE *input)
{
    stack_frame();
    char *line = NULL;

    if (read_error_point()) {
        free(line);
        leave();
    }

    for (; !feof(input); free(line)) {
        line = read_line(input);
        if (empty_string(line))
            continue;
        char *end;

        int full_length = strlen(line);
        char *from = trim_string(line, NULL);
        end = words_end(from);
        *end = '\0';

        char *to = trim_string(end + 1, NULL);
        end = words_end(to);
        *end = '\0';

        char *amount = trim_string(end + 1, NULL);
        end = words_end(amount);
        *end = '\0';
        if (*amount == '-') {
            OP(correct_num_format(amount + 1) == 0, INVALID_NUMBER);
        } else {
            OP(correct_num_format(amount) == 0, INVALID_NUMBER);
        }

        OP((line + full_length) != end, CURRENCY_NOT_FOUND);
        char *currency = trim_string(end + 1, &end);
        OP(!strchr(currency, ' '), EXTRA_TERM);
        *end = '\0';
        OP(find_currency(table, currency), CURRENCY_NOT_FOUND);

        long long value = convert_currency(table, load_decimal(amount, PAYMENT_DECIMALS), currency);
        long long from_count = char_count(from, ';') + 1;
        long long to_count = char_count(to, ';') + 1;

        char last_char;
        do {
            end = get_token(from, semicolon);
            last_char = *end;

            *end = '\0';

            struct person *p;
            OP(p = find_person(persons, from), PERSON_NOT_FOUND);
            p->amount += value / from_count;

            from = end;
        } while (last_char && ++from);

        do {
            end = get_token(to, semicolon);
            last_char = *end;

            *end = '\0';

            struct person *p;
            OP(p = find_person(persons, to), PERSON_NOT_FOUND);
            p->amount -= value / to_count;

            to = end;
        } while (last_char && ++to);
    }
    leave();
}
