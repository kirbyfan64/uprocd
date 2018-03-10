# uprocd_run -- Enter the main uprocd daemon and fork the process

## SYNOPSIS

```c
#include <uprocd.h>

UPROCD_EXPORT uprocd_context * uprocd_run();
```

## DESCRIPTION

When this function is called from a native module, it will wait for a request, then
return from a new, forked process. It will return a context, which will contain the
necessary information from uprocctl(1) to mimic the original session. Once the module
is ready, they should call uprocd_context_enter(3) to update the current environment
to match that of the context.

Here is a more detailed description of what happens:

1. A native module is loaded, performing steps further detailed in
   uprocd_module_entry(3).

2. The native module, after initialization, calls **uprocd_run**.

3. **uprocd_run** will begin receiving D-Bus messages from uprocctl(3). When a message
   to spawn a new program is received, **uprocd_run(3)** will retrieve uprocctl(3)'s
   environment, place it in a context, and fork the process. After the fork is complete,
   the new context is returned.

4. At this point, the module is now running inside of the forked process. It is now
   the module's responsibility to initialize needed values, then enter the context via
   uprocd_context_enter(3).

## RETURN VALUE

A context object. This function will never properly return from the original process;
it will only ever return from the forked processes. If there is code that should be run
if an error occurs in the original process, use uprocd_on_exit(3) to attach an exit
handler that will be run if an error occurs.

## SEE ALSO

uprocd.index(7), uprocd.h(3), uprocctl(1), uprocd_context_enter(3),
uprocd_module_entry(3), uprocd_on_exit(3)
