# uprocd_module_directory -- Retrieve the path to the current uprocd module

## SYNOPSIS

```c
#include <uprocd.h>

UPROCD_EXPORT const char * uprocd_module_directory();
```

## DESCRIPTION

This function returns the directory holding the current module's .module file. If you
wish to retrieve the path to a file within this directory, you can use
uprocd_module_path(3) as a shortcut.

## RETURN VALUE

The resulting path. This function never fails.

## SEE ALSO

uprocd.index(7), uprocd.h(3), uprocd.module(5), uprocd_module_path(3)
