# nmake file
# Note: nmake howto: keep things manual, copy paste and it'll work fine
# Note: GNU make + cmd/sh + cl + gnu find = hell. So, don't

srcdir = c:\partyplace\git\sysintercept
build = $(srcdir)\bin\debug
	
libpaths = "C:\boost\stage\lib"
MKEXE = link /nologo /libpath:$(libpaths)
MKLIB = lib /nologo /libpath:$(libpaths)
MKDLL = $(MKEXE) /DLL

bcli = $(build)\cli
cli = $(bcli)\sysintercept.exe
i = $(srcdir)\SyscallInterceptor\src
cli_cpps = "$(i)\sysintercept.cpp" "$(i)\interceptconfigshare.cpp"

bdll = $(build)\dll
dll = $(bdll)\sysintercept.dll
d = $(srcdir)\SyscallInterceptorDll\src
dll_cpps = $(d)\ipc.cpp $(d)\substitution.cpp $(d)\interceptconfig.cpp $(d)\dllmain.cpp

bcommon = $(build)\common
common = $(bcommon)\sysintercept_common.lib
c = $(srcdir)\SyscallInterceptorCommon\src
common_cpps = $(c)\logging.cpp

bncodehook = $(build)\ncodehook
ncodehook = $(bncodehook)\ncodehook.lib
nc = $(srcdir)\ncodehook
ncodehook_cpps = $(nc)\ncodehook.cpp $(nc)\ncodehookitem.cpp

bninjectlib = $(build)\ninjectlib
ninjectlib = $(bncodehook)\ninjectlib.lib
ni = $(srcdir)\ninjectlib
ninjectlib_cpps = $(ni)\process.cpp $(ni)\iatmodifier.cpp

build_dirs = "$(bcommon)" "$(bdll)" "$(bcli)" "$(bncodehook)" "$(bninjectlib)"
stdafx = $(c)\stdafx.h

# Maybe: use /Yc /Yu to speed up compilation
#http://msdn.microsoft.com/en-us/library/d9b6wk21.aspx
#$(stdafx_pch)
#/Yu stdafx.h

# Maybe: allow for better reincremental recompilation

# add -Ox for maximal optimization (default = no optimization)
COMPILE = cl /c /EHs /nologo \
	/I"C:\Program Files\Microsoft SDKs\Windows\v7.1\Include" \
	/I"C:\Program Files\Microsoft SDKs\Windows\v7.1\Include\gl" \
	/I"C:\Program Files\Microsoft Visual Studio 10.0\VC\Include" \
	/I"C:\boost" \
	/I"$(srcdir)\ninjectlib" \
	/I"$(srcdir)\ncodehook" \
	/I"$(srcdir)\distorm" \

all : $(cli)

build_dirs :
	-mkdir $(build_dirs)

clean :
	echo del $(build_dirs) for loop TODO /*
  
$(cli) : $(dll) $(common) $(cli_cpps) $(ninjectlib)
	cd "$(bcli)" && \
	$(COMPILE) \
		/I"$(c)" \
		/I"$(i)" \
		$(cli_cpps) && \
	$(MKEXE) /out:"$(cli)" *.obj "$(common)" "$(ninjectlib)"

$(dll) : $(common) $(dll_cpps) $(ncodehook)
	cd "$(bdll)" && \
	$(COMPILE) \
		/I"$(c)" \
		/I"$(d)" \
		$(dll_cpps) && \
	$(MKDLL) /out:"$(dll)" *.obj "$(common)" "$(ncodehook)"

$(common) : $(common_cpps)
	cd "$(bcommon)" && \
	$(COMPILE) \
		/I"$(c)" \
		$(common_cpps) && \
	$(MKLIB) /out:"$(common)" *.obj
	
$(ncodehook) : $(ncodehook_cpps)
	cd "$(bncodehook)" && \
	$(COMPILE) \
		/I"$(nc)" \
		$(ncodehook_cpps) && \
	$(MKLIB) /out:"$(ncodehook)" *.obj "$(srcdir)\distorm\distorm.lib"
	
$(ninjectlib) : $(ninjectlib_cpps)
	cd "$(bninjectlib)" && \
	$(COMPILE) \
		/I"$(ni)" \
		$(ninjectlib_cpps) && \
	$(MKLIB) /out:"$(ninjectlib)" *.obj

$(cli_cpps) $(dll_cpps) $(common_cpps) : $(stdafx)

