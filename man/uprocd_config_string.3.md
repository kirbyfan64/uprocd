# uprocd_config_string -- Return the string at the given property

## SYNOPSIS

```c
#include <uprocd.h>

UPROCD_EXPORT const char * uprocd_config_string(const char *key);
```

## DESCRIPTION

This function will retrieve the string at the given property. To retrieve a string from
a list, use uprocd_config_string_at(3).

## RETURN VALUE

The string value, or NULL if it is not present. It is undefined to call this function
on a non-string value.

## EXAMPLE

Module config:

```ini
[NativeModule]

[Properties]
Present=string
NotPresent=string

[Defaults]
Present=test
```

Source code:

```c
const char *s;

s = uprocd_config_string("Present");
printf("Present: %s\n", s); // Present: test

s = uprocd_config_string("NotPresent");
printf("NotPresent: %s\n", s); // NotPresent: (NULL) (if your libc doesn't crash)
```

## SEE ALSO

uprocd.index(7), uprocd.h(3), uprocd_config_string_at(3)
