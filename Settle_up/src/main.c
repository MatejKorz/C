#include "constants.h"
#include "currency.h"
#include "errors.h"
#include "load.h"
#include "persons.h"
#include "structures.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

struct person *find_extreme(struct persons *persons, int sign)
{
    if (!persons->size)
        return NULL;
    struct person *extreme = &persons->persons[0];
    long long extreme_amount = extreme->amount * sign;
    for (int i = 1; i < persons->size; ++i) {
        if (persons->persons[i].amount * sign > extreme_amount) {
            extreme = &persons->persons[i];
            extreme_amount = extreme->amount * sign;
        }
    }
    return extreme;
}

void settle_debt(struct persons *persons, struct currency_table *table)
{
    while (1) {
        struct person *debtor = find_extreme(persons, -1);
        struct person *creditor = find_extreme(persons, 1);

        if (debtor->amount == 0) {
            break;
        }

        long long amount = -debtor->amount;
        if (amount > creditor->amount)
            amount = creditor->amount;

        debtor->amount += amount;
        creditor->amount -= amount;

        char *amount_base = malloc(sizeof(char) * 20);
        memset(amount_base, 0, sizeof(char) * 20);
        sprintf(amount_base, "%lld", amount / power_of_ten(PAYMENT_DECIMALS));

        char *amount_decimal = malloc(sizeof(char) * 20);
        memset(amount_decimal, 0, sizeof(char) * 20);
        sprintf(amount_decimal, "%lld", amount % power_of_ten(PAYMENT_DECIMALS));

        if (!strcmp(amount_base, "0") && !strcmp(amount_decimal, "0")) {
            free(amount_base);
            free(amount_decimal);
            break;
        }

        if (!strcmp(amount_decimal, "0")) {
            printf("%s (%s) -> %s (%s): %s %s\n", debtor->name, debtor->id, creditor->name, creditor->id, amount_base, table->main_currency);
        } else {
            printf("%s (%s) -> %s (%s): %s.%.2s %s\n", debtor->name, debtor->id, creditor->name, creditor->id, amount_base, amount_decimal, table->main_currency);
        }
        free(amount_base);
        free(amount_decimal);
    }
}

int main(int argc, char **argv)
{
    stack_top_frame();
    enum error_codes error_code;
    struct currency_table currency_table;
    struct persons persons;
    FILE *person_file = NULL;
    FILE *currency_file = NULL;
    FILE *payment_file = NULL;
    memset(&currency_table, 0, sizeof(currency_table));
    memset(&persons, 0, sizeof(persons));

    if ((error_code = read_error_point())) {
        object_destroy(&currency_table);
        object_destroy(&persons);
        if (person_file != NULL) {
            fclose(person_file);
        }
        if (currency_file != NULL) {
            fclose(currency_file);
        }
        if (payment_file != NULL) {
            fclose(payment_file);
        }
        if (error_code != SUCCESS)
            print_error_message(error_code);

        return return_code(error_code);
    }
    OP(argc == 4, INVALID_ARGUMENTS);

    init_currency_table(&currency_table);
    init_persons(&persons);

    OP(person_file = fopen(argv[1], "r"), FILE_MISSING);
    load_persons(&persons, person_file);
    OP(persons.size >= 2, ONE_PERSON);

    OP(currency_file = fopen(argv[2], "r"), FILE_MISSING);
    load_currency_table(&currency_table, currency_file);
    OP(currency_table.size >= 1, NO_CURRENCY);
    OP(currency_table.main_currency != NULL, NO_MAIN_CURRENCY);

    OP(payment_file = fopen(argv[3], "r"), FILE_MISSING);
    load_payments(&persons, &currency_table, payment_file);

    settle_debt(&persons, &currency_table);

    exit_success();
    return EXIT_SUCCESS;
}
