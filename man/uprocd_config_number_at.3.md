# uprocd_config_number_at -- Return the number at the given index of a list

## SYNOPSIS

```c
#include <uprocd.h>

UPROCD_EXPORT double uprocd_config_number_at(const char *list, int index);
```

## DESCRIPTION

This function will retrieve the number at the given index of the given list. To retrieve
a number property, use uprocd_config_number(3).

## RETURN VALUE

The number's value, or 0 if the list is not present or index is invalid. It is undefined
to call this function on a non-number list value.

To distinguist between 0 values and non-present lists or invalid indexes, use
uprocd_config_present(3) and uprocd_config_list_size(3).

## EXAMPLE

Module config:

```ini
[NativeModule]

[Properties]
Present=list number
NotPresent=list number

[Defaults]
Present=10 20 30
```

Source code:

```c
double num;

num = uprocd_config_number_at("Present", 0);
printf("Present[0]: %d\n", num); // Present[0]: 10

num = uprocd_config_number_at("Present", 20);
printf("Present[20]: %d\n", num); // Present[20]: 0

num = uprocd_config_number_at("NotPresent", 0);
printf("NotPresent[0]: %d\n", num); // NotPresent[0]: 0
```

## SEE ALSO

uprocd.index(7), uprocd.h(3), uprocd_config_present(3), uprocd_config_list_size(3),
uprocd_config_number(3)
