# uprocd_context_get_args -- Retrieve arguments from a uprocd context

## SYNOPSIS

```c
#include <uprocd.h>

UPROCD_EXPORT void uprocd_context_get_args(uprocd_context *ctx, int *pargc,
                                           char ***pargv);
```

## DESCRIPTION

This function will retrieve the command-line arguments that were passed to
uprocctl(1) and then stored in the context. The argument count will be stored in
pargc, and the argument array will be stored in pargv.

## EXAMPLE

```c
uprocd_context *ctx = uprocd_run();

int argc;
char **argv;

uprocd_context_get_args(ctx, &argc, &argv);
```

## SEE ALSO

uprocd.index(7), uprocd.h(3), uprocctl(1)
