#include <windows.h>
#include <iostream>

#include "emul_dll.h"
#include "emulator.h"


#if defined(EMULDLL_EMBEDDING)

int main()
{
    std::cout << "Hello " << std::endl;
    return 0;
}

#elif defined(EMULDLL_EXPORTS)

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
        // Loaded to a process - perform base init
    case DLL_PROCESS_ATTACH:
        break;
        // Joined to a thread - ignore
    case DLL_THREAD_ATTACH:
        break;
        // Release from a thread - ignore
    case DLL_THREAD_DETACH:
        break;
        // Unloaded from a process - perform deinit
    case DLL_PROCESS_DETACH:
        break;
    };
    return TRUE;
}

#else

#error "Unknown compilation"

#endif
