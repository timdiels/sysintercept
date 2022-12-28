If you are looking for a windows hooking library/engine, try [EasyHook](https://easyhook.github.io/) or [detours](https://github.com/microsoft/detours) instead. The original intent of this repo was to rewrite file system paths in system calls of a process without having to use a virtual machine and without modifying the program executable that should see the fake file system. This repo contains a proof of concept, research notes and ideas. When I last worked on this, I was changing the build system directly on the master branch but the code itself looks about right. You'd likely have to go back a number of commits to find a version that works or build it with visual studio again.

The idea/goal: sysintercept allows you to intercept and modify win32 system calls done by a process. sysintercept provides a CLI. Aim is to allow rewriting paths, translating keyboard input, ...

.. contents::


Usage
=====
At the command line::

  sysintercept c:\full\path\to\config.xml relative\or\abs\path\to\something.exe
  
For config.xml syntax see 
`this example <https://github.com/limyreth/sysintercept/blob/master/tests/haskell_pathrewrite/config.xml>`_ to get an idea
and `sysintercept_config.xsd <https://github.com/limyreth/sysintercept/blob/master/xsd/sysintercept_config.xsd>`_
for full details. 

This runs something.exe and applies rules of config.xml to it.

Shortcomings:

- Does not pass on arguments to something.exe
- Verbose logging that cannot be turned off.
- Does not handle relative path
- Catches only a couple of functions for path rewriting but misses many other ones.
- Supports barely any rules currently.


How to compile
----------------
Apparently this requires zero install, you might be able to avoid that by looking for a build command in the zero install feed file, an xml file. You might also get visual studio to compile it as that's how I started out. These are the original instructions:

- Setting up the environment:

  - Download and run `Zero Install setup for windows <http://0install.net/install-windows.html>`_

  - Open a command line and run::

      0alias 0compile http://0install.net/2006/interfaces/0compile.xml

  - Download and unpack the sysintercept source

  - In the root of the source, open a command prompt and run::

      0compile setup
    
- Actual building (incremental build)::

    0compile build


TODO get rid of these steps:

- 0install
- Install windows SDK (for VC++ toolchain)
- boost and friends:

  Regardless of where you install boost, I'll refer to it as C:\boost.
  
  - Install boost: 
    
    Dowload and unpack `this`__ to C:\boost.
  
    __ http://www.boost.org/doc/libs/1_49_0/more/getting_started/windows.html
  
  - Install boost-log:

    Download and unpack boost-log files from sourceforge, to C:\boost.
  
  - Compile all boost libs, open "windows SDK 7.1 command prompt" (via start menu) and execute::
  
      cd C:\boost
      bootstrap
      PATH=%PATH%;C:\boost\bin
      .\b2 --build-type=complete stage
      
  - Download and install codesynthesis msi from here: http://www.codesynthesis.com/products/xsd/download.xhtml


Use cases
=========

ZI (http://0install.net) Use cases
----------------------------------
This was the original use case for the tool but I no longer have a need for this.

- Allow path redirects in feed file. 

  - redirect hardcoded file paths to correct location (location where binaries
    will be placed is impossible to know at compile time)

  - could be used to work with simplified view of environment's filesystem

    - /home: current user's homedir

    - /app/cache

    - /app/config

    - /media/...

    - and no more than those dirs. This way devs don't have to make a call to resolve a relative to an absolute path, the sandbox does it for them.
  
  
Other notes
============
The current implementation intercepts syscalls with user space techniques, by hooking into certain api calls. While programs can't avoid the hook on those api calls, they can still make system calls on their own without those api calls although it would be a fragile way of doing it and no sane program would do this unless specifically trying to thwart api hooking. Point being, this tool will never be a perfect sandbox and it certainly isn't in it's current state; so don't use it on malware.
(See 2d, 2f, 2g of
http://www.stanford.edu/~stinson/paper_notes/win_dev/hooks/defeating_hooks.txt.
Even finding direct interrupt stuff and replacing that would not work because
it's probably an undecidable problem)


Performance
-----------
There is no emulation or virtualization involved. It injects a dll into the target process. The dll wraps all system calls necessary for the given config and only those, the hooking can be done dynamically without recompiling the dll.

Todo: python or lua would be more flexible than an xml config file, but would the overhead of python be acceptable for system calls? We could offer multiple mechanisms, e.g. for some cases python is fast enough. Rust might be a faster alternative. Either way it needs to be implemented first in c++ or rust.


License
-------
Project is covered by the GPLv3 license.

Libraries used in project:

- distorm: Modified BSD license -> GPL compatible
- ncodehook, ninjectlib: no license?
- boost: boost license -> GPL compatible
- CodeSynthesis: GPLv2

  
Design choices
===============

Program design overview
-----------------------

sysintercept.dll: This dll intercepts win32 calls of whatever process it is loaded by.

sysintercept.exe: a cli interface, that starts a program and injects the dll into that program's process.

When sysintercept.exe runs:

- it starts the child process in a suspended state,
- makes the path to config.xml available in shared memory
- modifies the IAT of the child process in memory, so it will load sysintercept.dll when started
- resumes the child process and waits for it to finish

When the child process runs (i.e. when it is resumed):

- it will load the dll, 
- during DllMain, the dll patches all relevant win32 calls (inline patching) so that they are intercepted
- upon first win32 call, the dll will access shared memory, load and parse the xml file so that it knows what to do with intercepted calls.
  Note we couldn't do this in DllMain as many libs aren't loaded yet (e.g. IPC for shared memory), Dll main is very limited.

System call interposition methods
---------------------------------
How to intercept syscalls?

- Translate app binaries and its dependencies to redirect syscalls through the
  compatibility layer (does not require source code)

  Problem: how to tell on behalf of which process a dependency is currently
  executing

  Con: 

  - translating binaries causes first run slow-down
  - translating binaries may end up being very hard

- IAT / caller patching

  http://sandsprite.com/CodeStuff/IAT_Hooking.html

  - the fix for catching libs as well (but not crazy hacky direct use of
    interrupts):
    http://msdn.microsoft.com/en-us/magazine/cc302289.aspx
    /This is because APISPY32 performs its function interception on the
    application executable image, but not on the image of any DLL./
  - Also, there's a problem with NT4, fix with
    http://msdn.microsoft.com/en-us/magazine/cc302289.aspx
    /Matt designed APISPY32 for Windows NT 3.5./

  Easily intercepts a single PE. This means you have to additionally
  intercept its dependencies' PEs as well. So basically you might as well use
  a system-wide technique...

- inline/callee patching
  
  e.g. http://newgre.net/ncodehook (trampolining/hotpatching), detours
  (trampolining/hotpatching), easyhook(?)

  Note:

  - trampolining: first instructions are modified to a jump to hook, the
    hook uses a trampoline function to call the original function (which
    is now modified with a jump)
  - hot patching: functions to patch have free room at start to make
    patching more stable and easy (only when they were compiled that way)
  
  Works on a per-process basis, rather than system-wide. It patches by
  overwriting the first part of the func in shared lib, which apparently only
  affects the current process.

  Pro:

  - relatively fast
  - no root, setup, ... required

  Con:

  - Programs could bypass interception using very hacky techniques if they realize they are being intercepted by this tool but that's fine for this tool's purpose.

- process level emulation: I forgot... But it was effective, though quite slow.
- Various info:
  - windows
    - place dll in same dir
    - http://www.codeproject.com/Articles/2082/API-hooking-revealed
    - http://www.codeproject.com/Articles/30140/API-Hooking-with-MS-Detours
    - http://www.autoitscript.com/forum/topic/87240-windows-api-hooking-injecting-a-dll/
    - http://jpassing.com/2008/01/06/using-import-address-table-hooking-for-testing/
    - http://www.codeproject.com/Articles/4610/Three-Ways-to-Inject-Your-Code-into-Another-Proces
    - http://www.ethicalhacker.net/content/view/207/24/
    - apispy32
    - http://www.appvirtguru.com/
  - linux

    http://wiki.virtualsquare.org/wiki/index.php/System_Call_Interposition:_how_to_implement_virtualization

    - purelibc/LD_PRELOAD (ineffective)
    - ptrace (just slow? or also ineffective?)
    - utrace (requires kernel mod)

    few more like it

    - systemtap (?)
    - uprobes (utrace)
    - ltt-ng (purelibc?)


For now process-level emulation, later I may also check for kmview/utrace
support in the kernel and use process-level as a fallback.
Well, should do another comparison perhaps, will we go for max security
from the start etc?


Related projects
----------------

API hooking:

- http://en.wikipedia.org/wiki/Hooking#Windows
- easyhook, detours, ...

App virtualization:

- windows:
  - free: http://portable-app.com/
  - shareware: http://www.cameyo.com/
- commercial:
  - thinapp
  - endeavor application jukebox
  - http://www.enigmaprotector.com/en/aboutvb.html
- free, linux
  - http://wiki.virtualsquare.org/wiki/index.php/Main_Page#Overview_of_tools_and_libraries

    various interesting implementations: http://wiki.virtualsquare.org/wiki/index.php/System_Call_Interposition:_how_to_implement_virtualization

    (rump, an anykernel, looks interesting too; allows you to run each process
    with a virtual kernel with everything customised to bits)

    **might want to add to this project**

Sandboxes:

- free, linux:
  - LXC http://lxc.sourceforge.net/
  - http://plash.beasts.org/wiki/ (only works if glib isn't statically linked,
    which it normally isn't)
  - http://fedoraproject.org/wiki/Features/VirtSandbox
  - selinux http://blog.bodhizazen.net/linux/selinux-sandbox/
- non-free:
  - windows: sandboxie
  - mac: appstore sandboxing

Process-level emulation:

- https://minemu.org/mediawiki/index.php?title=Main_Page


System calls
------------
A system consists of kernel-space and user-space. CPU has a mechanism for
privileges. Kernel has privilege to access hardware directly, user-space has no
such privilege and must ask the kernel to do so via a syscall. Syscalls can
usually be done by CPU interrupts (x86 also has SYSCALL/SYSENTER (or call
gates)); which to use depends on choices of the kernel. Most OSs provide a
library to do this syscall interrupting.

Any well-behaved application will use that library. Though when wanting to
offer security one should take into account the possibility of a syscall by
manual interrupt without that library (or are the details of the interrupt so
unstable that it'd be very hard to get this working?? and would that justify
ignoring it? Also take into account, it may be statically linked into apps and
libs)

PE executable format
--------------------
http://msdn.microsoft.com/en-us/magazine/cc301805.aspx
http://msdn.microsoft.com/en-us/magazine/cc301808.aspx

Definitions
-----------

Various
'''''''
- System call interposition (linux) = API hooking (windows)
- tracing = hypercall = hook = probing
- process/application level virtualization = sandboxing
- virtualization ~= emulation
- App virtualization terms: http://www.brianmadden.com/blogs/rubenspruijt/archive/2010/09/23/application-virtualization-smackdown-head-to-head-analysis-of-endeavors-citrix-installfree-microsoft-spoon-symantec-and-vmware.aspx
- When a process makes use of a library, the library code is executed in the same process' context

Virtualization vs sandboxing
''''''''''''''''''''''''''''
- application virtualization solutions:
  - a server from which software can be retrieved by clients, 
  - something to record installed files into a single app file which can be
    uploaded to server
  - applications are ran by a virtualization component which modifies and
    passes syscalls (compatibility layer)
  - goal: easier to run app without installing, configging, ...
- sandbox solutions:
  - applications are ran by a virtualization component which modifies and
    passes syscalls
  - or the kernel/libs are modified
  - goal: much greater focus on security/privacy than app virtualization
