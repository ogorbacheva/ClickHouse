#include <new>

#include <common/config_common.h>
#include <common/memory.h>
#include <Common/MemoryTracker.h>

/// Replace default new/delete with memory tracking versions.
/// @sa https://en.cppreference.com/w/cpp/memory/new/operator_new
///     https://en.cppreference.com/w/cpp/memory/new/operator_delete
#if NOT_UNBUNDLED

namespace Memory
{

ALWAYS_INLINE void trackMemory(std::size_t size)
{
#if USE_JEMALLOC
    /// The nallocx() function allocates no memory, but it performs the same size computation as the mallocx() function
    /// @note je_mallocx() != je_malloc(). It's expected they don't differ much in allocation logic.
    if (likely(size != 0))
        CurrentMemoryTracker::alloc(nallocx(size, 0));
#else
    CurrentMemoryTracker::alloc(size);
#endif
}

ALWAYS_INLINE bool trackMemoryNoExept(std::size_t size) noexcept
{
    try
    {
        trackMemory(size);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

ALWAYS_INLINE void untrackMemory(void * ptr [[maybe_unused]], std::size_t size [[maybe_unused]] = 0) noexcept
{
    try
    {
#if USE_JEMALLOC
        /// @note It's also possible to use je_malloc_usable_size() here.
        if (likely(ptr != nullptr))
            CurrentMemoryTracker::free(sallocx(ptr, 0));
#else
        if (size)
            CurrentMemoryTracker::free(size);
#endif
    }
    catch (...)
    {}
}

}

/// new

void * operator new(std::size_t size)
{
    Memory::trackMemory(size);
    return Memory::newImpl(size);
}

void * operator new[](std::size_t size)
{
    Memory::trackMemory(size);
    return Memory::newImpl(size);
}

void * operator new(std::size_t size, const std::nothrow_t &) noexcept
{
    if (likely(Memory::trackMemoryNoExept(size)))
        return Memory::newNoExept(size);
    return nullptr;
}

void * operator new[](std::size_t size, const std::nothrow_t &) noexcept
{
    if (likely(Memory::trackMemoryNoExept(size)))
        return Memory::newNoExept(size);
    return nullptr;
}

/// delete

/// C++17 std 21.6.2.1 (11)
/// If a function without a size parameter is defined, the program should also define the corresponding function with a size parameter.
/// If a function with a size parameter is defined, the program shall also define the corresponding version without the size parameter.

/// cppreference:
/// It's unspecified whether size-aware or size-unaware version is called when deleting objects of
/// incomplete type and arrays of non-class and trivially-destructible class types.

void operator delete(void * ptr) noexcept
{
    Memory::untrackMemory(ptr);
    Memory::deleteImpl(ptr);
}

void operator delete[](void * ptr) noexcept
{
    Memory::untrackMemory(ptr);
    Memory::deleteImpl(ptr);
}

void operator delete(void * ptr, std::size_t size) noexcept
{
    Memory::untrackMemory(ptr, size);
    Memory::deleteSized(ptr, size);
}

void operator delete[](void * ptr, std::size_t size) noexcept
{
    Memory::untrackMemory(ptr, size);
    Memory::deleteSized(ptr, size);
}

#else

/// new

void * operator new(std::size_t size) { return Memory::newImpl(size); }
void * operator new[](std::size_t size) { return Memory::newImpl(size); }

void * operator new(std::size_t size, const std::nothrow_t &) noexcept { return Memory::newNoExept(size); }
void * operator new[](std::size_t size, const std::nothrow_t &) noexcept { return Memory::newNoExept(size); }

/// delete

void operator delete(void * ptr) noexcept { Memory::deleteImpl(ptr); }
void operator delete[](void * ptr) noexcept { Memory::deleteImpl(ptr); }

void operator delete(void * ptr, const std::nothrow_t &) noexcept { Memory::deleteImpl(ptr); }
void operator delete[](void * ptr, const std::nothrow_t &) noexcept { Memory::deleteImpl(ptr); }

void operator delete(void * ptr, std::size_t size) noexcept { Memory::deleteSized(ptr, size); }
void operator delete[](void * ptr, std::size_t size) noexcept { Memory::deleteSized(ptr, size); }

#endif
