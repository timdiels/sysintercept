# nmake file
# Note: nmake howto: keep things manual, copy paste and it'll work fine
# Note: GNU make + cmd/sh + cl + gnu find = hell. So, don't
# Note: can't split this file because all dependencies have to be known, so use prefix to fake namespaces 

srcdir = c:\partyplace\git\sysintercept
g_build_dir = $(srcdir)\bin\debug


cli_build_dir = $(g_build_dir)\cli
cli = $(cli_build_dir)\sysintercept.exe
cli_src_dir = $(srcdir)\SyscallInterceptor\src
cli_cpps = "$(cli_src_dir)\sysintercept.cpp" "$(cli_src_dir)\interceptconfigshare.cpp"
cli_xsd = "$(cli_src_dir)\xml\sysintercept_config.xsd"
cli_xml_src_dir = $(cli_build_dir)\xml

dll_build_dir = $(g_build_dir)\dll
dll = $(dll_build_dir)\sysintercept.dll
dll_src_dir = $(srcdir)\SyscallInterceptorDll\src
dll_cpps = "$(dll_src_dir)\ipc.cpp" "$(dll_src_dir)\substitution.cpp" "$(dll_src_dir)\interceptconfig.cpp" "$(dll_src_dir)\dllmain.cpp"

common_build_dir = $(g_build_dir)\common
common = $(common_build_dir)\sysintercept_common.lib
common_src_dir = $(srcdir)\SyscallInterceptorCommon\src
common_cpps = $(common_src_dir)\logging.cpp

ncodehook_build_dir = $(g_build_dir)\ncodehook
ncodehook = $(ncodehook_build_dir)\ncodehook.lib
ncodehook_src_dir = $(srcdir)\ncodehook
ncodehook_cpps = "$(ncodehook_src_dir)\ncodehook.cpp" "$(ncodehook_src_dir)\ncodehookitem.cpp"

ninjectlib_build_dir = $(g_build_dir)\ninjectlib
ninjectlib = $(ninjectlib_build_dir)\ninjectlib.lib
ninjectlib_src_dir = $(srcdir)\ninjectlib
ninjectlib_cpps = $(ninjectlib_src_dir)\process.cpp $(ninjectlib_src_dir)\iatmodifier.cpp

distorm_src_dir = $(srcdir)\distorm
distorm_build_dir = $(distorm_src_dir)

xsd = "C:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe"
xsd_headers = C:\Program Files\CodeSynthesis XSD 3.3\include
xsd_lib_dir = C:\Program Files\CodeSynthesis XSD 3.3\lib\vc-10.0
xsd_lib = "$(xsd_lib_dir)\*.lib"

g_build_dirs = "$(common_build_dir)" "$(dll_build_dir)" "$(cli_build_dir)" "$(ncodehook_build_dir)" "$(ninjectlib_build_dir)"
stdafx = $(common_src_dir)\stdafx.h

# Maybe: use /Yc /Yu to speed up compilation
#http://msdn.microsoft.com/en-us/library/d9b6wk21.aspx
#$(stdafx_pch)
#/Yu stdafx.h

# Maybe: allow for better reincremental recompilation

# add -Ox for maximal optimization (default = no optimization)
g_compile = cl /c /EHs /nologo \
	/I"C:\Program Files\Microsoft SDKs\Windows\v7.1\Include" \
	/I"C:\Program Files\Microsoft SDKs\Windows\v7.1\Include\gl" \
	/I"C:\Program Files\Microsoft Visual Studio 10.0\VC\Include" \
	/I"C:\boost" \
	/I"$(ninjectlib_src_dir)" \
	/I"$(ncodehook_src_dir)" \
	/I"$(distorm_src_dir)" \
	
g_libpaths = /libpath:"C:\boost\stage\lib" /libpath:"$(xsd_lib_dir)"
g_mkexe = link /nologo $(g_libpaths)
g_mklib = lib /nologo $(g_libpaths)
g_mkdll = $(g_mkexe) /DLL

# Note: touching files the windows way:
# type nul >>$(file) & copy $(file) +,,

all : $(cli)

clean :
	-rmdir /S /Q "$(g_build_dir)"
	mkdir "$(g_build_dir)"

# Note: xml_code needs to be generated before cpps, hope this order forces that
$(cli) : $(cli_xml_src_dir) $(dll) $(common) $(ninjectlib) $(cli_cpps)
	-mkdir "$(cli_build_dir)"
	cd "$(cli_build_dir)" && \
	$(g_compile) \
		/I"$(common_src_dir)" \
		/I"$(cli_src_dir)" \
		/I"$(cli_xml_src_dir)" \
		/I"$(xsd_headers)" \
		"$(cli_xml_src_dir)\*.cpp" \
		$(cli_cpps) && \
	$(g_mkexe) /out:"$(cli)" *.obj "$(common)" "$(ninjectlib)" $(xsd_lib)
	
$(cli_xml_src_dir) : $(cli_xsd)
	-mkdir "$(cli_xml_src_dir)"
	$(xsd) cxx-tree --char-type wchar_t --output-dir "$(cli_xml_src_dir)" \
		--namespace-map "https://github.com/limyreth/sysintercept=sysintercept::config" \
		--hxx-suffix .h --cxx-suffix .cpp $(cli_xsd)

# add --generate-doxygen for free documentation based on the inline annotations in the xsd
# to use doxygen: doxygen -g sysintercept.doxygen, to generate doxygen config,
# then further you can gen doxygen with: doxygen sysintercept.doxygen

$(dll) : $(common) $(dll_cpps) $(ncodehook)
	-mkdir "$(dll_build_dir)"
	cd "$(dll_build_dir)" && \
	$(g_compile) \
		/I"$(common_src_dir)" \
		/I"$(dll_src_dir)" \
		$(dll_cpps) && \
	$(g_mkdll) /out:"$(dll)" *.obj "$(common)" "$(ncodehook)"

$(common) : $(common_cpps)
	-mkdir "$(common_build_dir)"
	cd "$(common_build_dir)" && \
	$(g_compile) \
		/I"$(common_src_dir)" \
		$(common_cpps) && \
	$(g_mklib) /out:"$(common)" *.obj
	
$(ncodehook) : $(ncodehook_cpps)
	-mkdir "$(ncodehook_build_dir)"
	cd "$(ncodehook_build_dir)" && \
	$(g_compile) \
		/I"$(ncodehook_src_dir)" \
		$(ncodehook_cpps) && \
	$(g_mklib) /out:"$(ncodehook)" *.obj "$(distorm_build_dir)\distorm.lib"
	
$(ninjectlib) : $(ninjectlib_cpps)
	-mkdir "$(ninjectlib_build_dir)"
	cd "$(ninjectlib_build_dir)" && \
	$(g_compile) \
		/I"$(ninjectlib_src_dir)" \
		$(ninjectlib_cpps) && \
	$(g_mklib) /out:"$(ninjectlib)" *.obj

$(cli_cpps) $(dll_cpps) $(common_cpps) : $(stdafx)

