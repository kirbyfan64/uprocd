# uprocd_on_exit -- Set a handler to be called on uprocd_run failure

## SYNOPSIS

```c
#include <uprocd.h>

typedef void (*uprocd_exit_handler)(void *userdata);
UPROCD_EXPORT void uprocd_on_exit(uprocd_exit_handler func, void *userdata);
```

## DESCRIPTION

This function will set a handler to be called if uprocd_run(3) fails. This handler
will only ever be called from the original parent process.

The given userdata will be passed to the handler when it is called.

## SEE ALSO

uprocd.index(7), uprocd.h(3), uprocd_run(3)
