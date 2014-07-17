#!/bin/sh
codesynthesis-xsd cxx-tree --char-type wchar_t --hxx-suffix .h --cxx-suffix .cpp --namespace-map 'https://github.com/limyreth/sysintercept=sysintercept::config::xml' sysintercept_config.xsd
mv *.cpp ../SyscallInterceptorCommon
mv *.h ../SyscallInterceptorCommon
