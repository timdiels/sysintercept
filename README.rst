Note: project is in early development stage and not ready for use yet, at all.

sysintercept allows you to intercept and modify win32 system calls done by a process. sysintercept provides a CLI. Aim is to allow rewriting paths, translating keyboard input, ... various things for improved compatibility.


Current state
=============

Succesfully gets notepad to show custom message on process exit.


How to compile
==============

Install eclipse. 
Load projects.
Install windows SDK (for VC++ toolchain)
Clean and build SyscallInterceptor

Note: does not compile with MinGW, you really need the VC++ toolchain


Licensing
=========

Project is covered by the GPLv3 license.

Notes on licenses of dependencies:
- distorm: Modified BSD license -> GPL compatible
- ncodehook, ninjectlib: no license?
- boost: boost license -> GPL compatible