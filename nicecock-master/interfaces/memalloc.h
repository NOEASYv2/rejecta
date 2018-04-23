#pragma once

/*
	https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/tier0/memalloc.h
	Structures aren't checked, if using, refer to your favorite disassembler.
	Needed for certain things when messing with the source engine (example: rebuilding anim stuff and allocating mem with
	win32 functions will cause an unexpected crash.)
*/

struct _CrtMemState;

#define MEMALLOC_VERSION 1

typedef size_t(*MemAllocFailHandler_t)(size_t);

//-----------------------------------------------------------------------------
// NOTE! This should never be called directly from leaf code
// Just use new,delete,malloc,free etc. They will call into this eventually
//-----------------------------------------------------------------------------
class IMemAlloc
{
public:
	void* Alloc(int nSize)
	{
		using Fn = void*(__thiscall*)(void*, int);
		return VT::vfunc< Fn >(this, 1)(this, nSize);
	}

	void* Realloc(void* pMem, int nSize)
	{
		using Fn = void*(__thiscall*)(void*, void*, int);
		return VT::vfunc< Fn >(this, 3)(this, pMem, nSize);
	}

	void Free(void* pMem)
	{
		using Fn = void(__thiscall*)(void*, void*);
		return VT::vfunc< Fn >(this, 5)(this, pMem);
	}
};