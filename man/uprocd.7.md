# uprocd -- Guide on managing uprocd modules with systemd

## DESCRIPTION

**uprocd** is a tool that starts processes in the background, then "freezes" them. When
you run a program using uprocctl(1), uprocd will fork the process and connect it to
the current terminal. This is essentially a form of "process caching" or "process
preloading"; processes will start more quickly, as the expensive initialization steps
have already taken place in the background.

## MODULES

Module files are stored in any of the following locations:

- /usr/share/uprocd/modules - Built-in modules.
- /usr/local/share/uprocd/modules - Installed modules.
- $XDG_CONFIG_HOME/uprocd/modules - User modules.

(For more information on the module format, see uprocd.module(5).)

Each uprocd module is a seperate daemon, and all are started via the systemd unit
uprocd@. Therefore, primary management of uprocd modules uses systemctl(1). In
order to access a module, use the uprocd@ template unit, with the instance name set to
the module name. For instance, the python module will be under the systemd name
uprocd@python. uprocd logs can be viewed through either **systemctl status** or
journalctl(1).

Querying information about, or running, modules should be done via uprocctl(1)

## MODULE MANAGEMENT EXAMPLES

In order to load a module, use **systemctl --user start**:

```
$ systemctl --user start uprocd@module-name
```

For example, to start the *python* module, run:

```
$ systemctl --user start uprocd@python
```

To check module's status and logs, use **systemctl --user status**:

```
$ systemctl --user status uprocd@module-name
```

Other commands:

```
$ systemctl --user stop uprocd@module-name
$ systemctl --user restart uprocd@module-name
```

To view the full, non-truncated logs, use journalctl(1):

```
$ journalctl -b --user-unit uprocd@module-name
```

## WORKINGS

It is recommended you read this section if you plan on creating uprocd modules!

When you start a uprocd service via systemd, a **uprocd** daemon is run for the module.
Every module has its own daemon. The module will immediately perform any initialization
steps, followed by relinquishing control back to the daemon. When the user requests for
a program to be run via uprocctl(1), the daemon will fork, and the forked process
will give a "context" back to the module.

The context object contains the environment of and arguments passed to uprocctl(1).
When the native module "enters" the context, its environment will be updated to match
that of the uprocctl(1) caller, and it will be attached to the caller's terminal.
After this, the native module will run the desired program.

## SEE ALSO

uprocd.index(7), uprocctl(1), uprocd.module(5), systemctl(1), journalctl(1)
