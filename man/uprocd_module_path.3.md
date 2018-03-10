# uprocd_module_path -- Retrieve the path to a file next to the current uprocd module

## SYNOPSIS

```c
#include <uprocd.h>

UPROCD_EXPORT char * uprocd_module_path(const char *path);
```

## DESCRIPTION

This function will retrieve the directory containing the current module's .module file,
then it will append the given path to that directory and return the result. It is
roughly equivalent to this:

```c
// Same as uprocd_module_path("some-file")
char *path = "some-file";
char *directory = uprocd_module_directory();
char full_path[1024];
sprintf(full_path, "%s/%s", directory, path);
// full_path is the result.
```

The return value must be freed using uprocd_module_path_free(3).

## RETURN VALUE

The resulting path. This function never fails.

## SEE ALSO

uprocd.index(7), uprocd.h(3), uprocd_module_directory(3), uprocd_module_path_free(3)
