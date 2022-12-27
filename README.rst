In its current state it's more of a proof of concept and some ideas. I also haven't worked on it in years and probably won't any time soon.

sysintercept allows you to intercept and modify win32 system calls done by a process. sysintercept provides a CLI. Aim is to allow rewriting paths, translating keyboard input, ... various things for improved compatibility.

.. contents::

Project state
=============

See usage to get an idea. Also note we're currently at no-version, 
i.e. not quite ready for production, or testing, ...


Usage
=====

At the command line::

  sysintercept c:\full\path\to\config.xml relative\or\abs\path\to\something.exe
  
For config.xml syntax see 
`this example <https://github.com/limyreth/sysintercept/blob/master/tests/haskell_pathrewrite/config.xml>`_ to get an idea
and `sysintercept_config.xsd <https://github.com/limyreth/sysintercept/blob/master/xsd/sysintercept_config.xsd>`_
for full details. 

This runs something.exe and applies rules of config.xml to it. 

What it also does:

- does not pass on arguments to something.exe

- prints logging messages like mad and you can't turn them off (mahahahaaa)

- really likes absolute paths, hates relative paths

- catches a few functions for path rewriting but haaaaardly all of them

.. TODO Here's how to get the binary. Here's how to do a few common use cases.


How to compile/'install'
------------------------

This section is fairly TODO atm (as 0compile doesn't work out of the box atm, apparently).


For users:

- Dowload and run `Zero Install setup for windows <http://0install.net/install-windows.html>`_

- 0launch the_uri...


For developers:

- Setting up the environment:

  - Dowload and run `Zero Install setup for windows <http://0install.net/install-windows.html>`_

  - Open a command line and run::

      0alias 0compile http://0install.net/2006/interfaces/0compile.xml

  - Download and unpack the sysintercept source

  - In the root of the source, open a command prompt and run::

      0compile setup
    
- Actual building (incremental build)::

    0compile build


TODO get rid of these steps:

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

- Allow path redirects in feed file. 

  - redirect hardcoded file paths to correct location (location where binaries
    will be placed is impossible to know at compile time)

  - could be used to work with simplified view of environment's filesystem

    - /home: current user's homedir

    - /app/cache

    - /app/config

    - /media/...

    - and no more than those dirs. This way devs don't have to make a call to resolve a relative to an absolute path, the sandbox does it for them.


Other use cases
---------------

TODO there are other uses as well, just need to brainstorm and note them down


Outcome: What are we working towards?
=====================================

.. (will later be titled What is sysintercept?... or such)

- Something that enables intercepting system calls of a child process on linux, windows and mac.

- Does not require manual set up (e.g. kernel modifications)

- Does not require root/admin rights, plain users can run it. (especially handy if you have to run it on a public computer)

- Intercepting does not affect anything else but the process whose syscalls are being intercepted.

- Is performant, does not cause serious slow-down.

- Easily supports:

  - Filepath translation (e.g. when app tries to open A, give it B)

  - maybe keyboard input translation (e.g. you are on a PC and have no rights to change keyboard layout, current layout is azerty, but you want dvorak. So you use this!)
  
  - ... (to be suggested/added by others (mail limyreth@gmail.com))
  
  
More project info
=================

Stability and effectiveness: How well does it work
--------------------------------------------------

The technique used is very effective to intercept system calls of any normal program. Programs can still get past interception, but only if using hacky techniques which aren't used unless you are really trying to not have your calls intercepted. 

The project is new, runs fine on my machine but might be full of bugs and will need test cases to convince people in production environments (TODO add bug report link).

Security
--------

Though benign programs will properly have their calls intercepted, it's possible to circumvent interception through hacky ways. The current implementation intercepts syscalls with user space techniques. If security is your goal, you'll want to intercept in kernel space.
(See 2d, 2f, 2g of
http://www.stanford.edu/~stinson/paper_notes/win_dev/hooks/defeating_hooks.txt.
Even finding direct interrupt stuff and replacing that would not work because
it's probably an undecidable problem The article isn't enthusiastic about
kernel hooks, though I suppose that can be made safe and solid)


Performance
-----------

TODO

Some might be concerned about performance, so should explain that this isn't emulation, it's just a dll injected into the target process that only adds a few ifs for every system call that *needs* to be intercepted for it to do its job. We won't hook things we don't need, ...

No profiling or tuning was done. (TODO once conceptually stable)

In a much much later project state, sysintercept could detect support for system call interposition and choose the best available mechanism. (e.g. prefer kernel module to userland patching) 


License
-------

Project is covered by the GPLv3 license.

Libraries used in project:

- distorm: Modified BSD license -> GPL compatible
- ncodehook, ninjectlib: no license?
- boost: boost license -> GPL compatible
- CodeSynthesis: GPLv2


Contributor documentation
=========================

Info for those wanting to contribute to development of sysintercept.

Currently this is info on various concepts related to design decisions and implementation of sysintercept.

These sections can be fairly messy or outdated, you might want to mail limyreth@gmail.com instead.

TODO change contact point to a mailing list

TODO list
---------

- Current: Side-tracked: make it easy to use ZI and 0compile on windows (it's apparently somewhat broken atm)
- make a ZI feed with source implementation for it, so that you can 0alias uri/sysintercept, and then sysintercept arg arg arg, without having to do any building of your own (then present that feed to ZI). dev_interface.xml, followed by a public feed file.
- allow changing verboseness of dll and cli. that of dll... well... we like to log while reading that xml, might want to keep verboseness separate from the xml! Was there no easier way to use boost ipc for multiple var passing?
- add logging for all file related functions we might need for file path rewriting
- ----- file path rewrite functionality is done now ------
- pass cli args
- test it on windows 7, public pcs, upload and check it passes anti virus software
- what about win64 support, testing it works everywhere in any program? ... stability?
- ----- is now usable if you manage to compile it, but no doc or other nifty bits ------
- add -v verbosity level for omitting some logging, default should mean nothing is logged
- once we add a -h --help and -V --version message on CLI, optionally include short notice of license (find a pretty print library for standard help message printing)
- could tweak boost.log by building it with BOOST_LOG_USE_WCHAR_T, ...
- documentation: xml config file: xsd and doxygen and/or example file with comments that demonstrates/uses everything in the xsd
- suggest to haskell for prefix fix, ...   

A todo
------

Solve this:
http://www.haskell.org/pipermail/cabal-devel/2011-November/007926.html

Compile haskell program and then allow it to be relocated.

Note: if we solve this, should post on those places and notify them of
available sandbox.

Next:

- compile a haskell program in windows

- make it crash by relocating

- now fix with sandbox

- make sure virus scanners allow sandbox.exe to run (upload to some interesting
  site) TODO
  
  
Design documentation
====================

Any documentation useful for making 'design' decisions of the program.

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

  - translating binaries causes (ZI) first run slow-down

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

  Easily intercepts of single PE. This means you have to additionally
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

  - Malicious programs could bypass interception using very hacky techniques.
    Benign programs are pretty much sure to be intercepted.
    directly using interrupts.

- process level emulation: I forgot... But it was quite effective, though quite slow.

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

  - mac: yet to look up


For now process-level emulation, later you may also check for kmview/utrace
support in the kernel and use process-level as a fallback.
Well, should do another comparison perhaps, will we go for max security
from the start etc?


Related projects
----------------

API hooking:

- http://en.wikipedia.org/wiki/Hooking#Windows

- http://easyhook.codeplex.com/

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

  - goal: similar to ZI; easier to run app without installing, configging, ...

- sandbox solutions:

  - applications are ran by a virtualization component which modifies and
    passes syscalls

  - or the kernel/libs are modified

  - goal: much greater focus on security/privacy than app virtualization


Virtualization vs emulation
'''''''''''''''''''''''''''

The difference between virtualization and emulation is vague, usually emulation
refers to imitating at a much lower level.

