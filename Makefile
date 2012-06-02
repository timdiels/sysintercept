# nmake file
# Note: nmake howto: keep things manual, copy paste and it'll work fine
# Note: GNU make + cmd/sh + cl + gnu find = hell. So, don't
# Note: can't split this file because all dependencies have to be known, so use prefix to fake namespaces
# Note: only quote env vars and literals with spaces, assume other variables' contents have whitespace quoted 

srcdir = c:\partyplace\git\sysintercept
g_build_dir = $(srcdir)\bin\debug

cli_build_dir = $(g_build_dir)\cli
cli = $(cli_build_dir)\sysintercept.exe
cli_src_dir = $(srcdir)\SyscallInterceptor
cli_cpps = $(cli_src_dir)\*.cpp

config_xsd = $(srcdir)\xsd\sysintercept_config.xsd

xml_src_dir = $(g_build_dir)\xml

dll_build_dir = $(g_build_dir)\dll
dll = $(dll_build_dir)\sysintercept.dll
dll_src_dir = $(srcdir)\SyscallInterceptorDll
dll_cpps = $(dll_src_dir)\*.cpp

common_build_dir = $(g_build_dir)\common
common = $(common_build_dir)\sysintercept_common.lib
common_src_dir = $(srcdir)\SyscallInterceptorCommon
common_cpps = $(common_src_dir)\*.cpp

ncodehook_build_dir = $(g_build_dir)\ncodehook
ncodehook = $(ncodehook_build_dir)\ncodehook.lib
ncodehook_src_dir = $(srcdir)\ncodehook
ncodehook_cpps = $(ncodehook_src_dir)\*.cpp

ninjectlib_build_dir = $(g_build_dir)\ninjectlib
ninjectlib = $(ninjectlib_build_dir)\ninjectlib.lib
ninjectlib_src_dir = $(srcdir)\ninjectlib
ninjectlib_cpps = $(ninjectlib_src_dir)\*.cpp

distorm_src_dir = $(srcdir)\distorm
distorm_build_dir = $(distorm_src_dir)

xsd = "C:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe"
xsd_headers = "C:\Program Files\CodeSynthesis XSD 3.3\include"
xsd_lib_dir = "C:\Program Files\CodeSynthesis XSD 3.3\lib\vc-10.0"
xsd_lib = $(xsd_lib_dir)\*.lib

boost = "C:\boost"

# Maybe: use /Yc /Yu to speed up compilation
#http://msdn.microsoft.com/en-us/library/d9b6wk21.aspx
#$(stdafx_pch)
#/Yu stdafx.h

# Maybe: allow for better reincremental recompilation

# add -Ox for maximal optimization (default = no optimization)
g_compile = cl /Zi /Gm /c /EHs /nologo \
	/I"C:\Program Files\Microsoft SDKs\Windows\v7.1\Include" \
	/I"C:\Program Files\Microsoft SDKs\Windows\v7.1\Include\gl" \
	/I"C:\Program Files\Microsoft Visual Studio 10.0\VC\Include" \
	/I$(boost) \
	/I$(ninjectlib_src_dir) \
	/I$(ncodehook_src_dir) \
	/I$(distorm_src_dir) \
	
g_libpaths = /libpath:"$(boost)\stage\lib" /libpath:$(xsd_lib_dir)
g_mkexe = link /nologo $(g_libpaths)
g_mklib = lib /nologo $(g_libpaths)
g_mkdll = $(g_mkexe) /DLL

# Note: touching files the windows way:
# type nul >>$(file) & copy $(file) +,,

all : $(cli)

dll : $(dll)
common : $(common)

clean :
	-rmdir /S /Q $(g_build_dir)
	mkdir $(g_build_dir)

$(cli) : $(common) $(ninjectlib) $(cli_cpps)
	-mkdir $(cli_build_dir)
	cd $(cli_build_dir) && \
	$(g_compile) \
		/I$(common_src_dir) \
		/I$(cli_src_dir) \
		$(cli_cpps) && \
	$(g_mkexe) /out:$(cli) *.obj $(common) $(ninjectlib)
	
$(xml_src_dir) : $(config_xsd)
	-mkdir $(xml_src_dir)
	$(xsd) cxx-tree --output-dir $(xml_src_dir) \
		--hxx-suffix .h --cxx-suffix .cpp \
		--char-type wchar_t \
		--namespace-map "https://github.com/limyreth/sysintercept=sysintercept::config::xml" \
		$(config_xsd)

# add --generate-doxygen for free documentation based on the inline annotations in the xsd
# to use doxygen: doxygen -g sysintercept.doxygen, to generate doxygen config,
# then further you can gen doxygen with: doxygen sysintercept.doxygen

$(dll) : $(xml_src_dir) $(common) $(dll_cpps) $(ncodehook)
	-mkdir $(dll_build_dir)
	cd $(dll_build_dir) && \
	$(g_compile) \
		/I$(common_src_dir) \
		/I$(dll_src_dir) \
		/I$(xml_src_dir) \
		/I$(xsd_headers) \
		$(xml_src_dir)\*.cpp \
		$(dll_cpps) && \
	$(g_mkdll) /out:$(dll) *.obj $(common) $(ncodehook) $(xsd_lib)

$(common) : $(common_cpps)
	-mkdir $(common_build_dir)
	cd $(common_build_dir) && \
	$(g_compile) \
		/I$(common_src_dir) \
		$(common_cpps) && \
	$(g_mklib) /out:"$(common)" *.obj
	
$(ncodehook) : $(ncodehook_cpps)
	-mkdir $(ncodehook_build_dir)
	cd $(ncodehook_build_dir) && \
	$(g_compile) \
		/I$(ncodehook_src_dir) \
		$(ncodehook_cpps) && \
	$(g_mklib) /out:$(ncodehook) *.obj $(distorm_build_dir)\distorm.lib
	
$(ninjectlib) : $(ninjectlib_cpps)
	-mkdir $(ninjectlib_build_dir)
	cd $(ninjectlib_build_dir) && \
	$(g_compile) \
		/I$(ninjectlib_src_dir) \
		$(ninjectlib_cpps) && \
	$(g_mklib) /out:"$(ninjectlib)" *.obj


######################
# run 'targets'

runtime_dll = $(cli_build_dir)\sysintercept.dll
runtime_xsd = $(cli_build_dir)\sysintercept_config.xsd

haskell_test_dir = $(srcdir)\tests\haskell_pathrewrite

run_haskell_path_rewrite : $(cli) $(runtime_dll) $(runtime_xsd)
	cd $(haskell_test_dir)\bin && \
	$(cli) $(haskell_test_dir)\config.xml $(haskell_test_dir)\bin\read_a.exe
	
$(runtime_dll) : $(dll)
	copy /Y $(dll) $(runtime_dll)
	
$(runtime_xsd) : $(config_xsd)
	copy /Y $(config_xsd) $(runtime_xsd)



