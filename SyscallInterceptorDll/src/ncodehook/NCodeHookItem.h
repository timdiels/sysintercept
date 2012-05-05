#pragma once

#include <iostream>

struct NCodeHookItem
{
	NCodeHookItem() : OriginalFunc(0), HookFunc(0), PatchSize(0), Trampoline(0) {};
	NCodeHookItem(uintptr_t of, uintptr_t hf, uintptr_t tp, uintptr_t ps)
		: OriginalFunc(of), HookFunc(hf), PatchSize(ps), Trampoline(tp)
	{
	};
	uintptr_t OriginalFunc;
	uintptr_t HookFunc;
	uintptr_t PatchSize;
	uintptr_t Trampoline;
};
