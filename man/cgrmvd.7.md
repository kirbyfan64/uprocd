# cgrmvd -- The CGRoup MoVer Daemon

## DESCRIPTION

**cgrmvd** is a daemon that lets you define processes who can be moved to the cgroups of
other processes. In short, you can have a process foo be moved to the exact same
cgroups as process bar, provided a policy file is present (cgrmvd.policy(5)). The
process being moved (foo) is referred to as the *copier*, and the process whose cgroups
are being copied (bar) is referred to as the *origin*.

The advantage is that unprivileged processes are able to use cgrmvd via D-Bus, and the
only root access required would be to install the proper policy file.

## D-BUS

cgrmvd comes with a simple D-Bus API for moving cgroups.

**com.refi64.uprocd.Cgrmvd.MoveCgroup('x' copier, 'x' origin)**

> Move a process to another's cgroups. **copier** specifies the PID of the process that
> should be moved, and **origin** specifies the PID of the process whose cgroups **copier**
> should be moved to.

> The pathnames for the processes should be specified in a .policy file, otherwise
> the move will be rejected. (This is to ensure random processes don't try to move
> cgroups around.)

## POLICIES

Policy files are stored in /usr/share/cgrmvd/policies. For more information, see
cgrmvd.policy(5).

## SECURITY

Process pathnames are checked using /proc/<pid>/exe. In theory, this could be a
security risk; since processes can change this symlink via prctl(2) with
PR_SET_MM_EXE_FILE, it could mask itself as another process in order to pass the
policies.

However, in order to use prctl(2) with PR_SET_MM, the CAP_SYS_RESOURCE capability is
required, and one would need root access in order to give a file this capability.
Therefore, if one is able to give any file the CAP_SYS_RESOURCE capability, they would
already have to be root, and therefore have the ability to move processes between
cgroups anyway.

## SEE ALSO

uprocd.index(7), cgrmvd.policy(5), prctl(2)
