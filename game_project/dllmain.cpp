#include "includes.h"

#include "base/global_variables.h"
#include "base/tools/memory.h"
#include "base/tools/netvar_parser.h"
#include "base/other/game_functions.h"
#include "base/hooks/hooks.h"

#include "base/tools/render.h"

#include "functions/listeners/listener_entity.h"
#include "functions/listeners/listener_event.h"

#include "functions/config_system.h"

#include "functions/aimbot/ragebot.h"
#include "functions/aimbot/engine_prediction.h"

#include "additionals/threading/shared_mutex.h"
#include "additionals/threading/threading.h"

#include <ShlObj.h>

static Semaphore dispatchSem;
static Semaphore waitSem;
static SharedMutex smtx;

template<typename T, T& Fn>
static void AllThreadsStub(void*)
{
	dispatchSem.Post();
	smtx.rlock();
	smtx.runlock();
	Fn();
}

template<typename T, T& Fn>
static void DispatchToAllThreads(void* data)
{
	smtx.wlock();

	for (size_t i = 0; i < Threading::numThreads; i++)
		Threading::QueueJobRef(AllThreadsStub<T, Fn>, data);

	for (size_t i = 0; i < Threading::numThreads; i++)
		dispatchSem.Wait();

	smtx.wunlock();

	Threading::FinishQueue(false);
}

FILE* stream = NULL;

void create_console()
{
#ifdef _DEBUG
	AllocConsole();
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);
#endif
}

void destroy_console()
{
#ifdef _DEBUG
	HWND console = GetConsoleWindow();
	FreeConsole();
	PostMessage(console, WM_CLOSE, 0, 0);
	fclose(stream);
#endif
}

uintptr_t WINAPI init(HMODULE hModule)
{
	g::window = FindWindow("Valve001", NULL);

	create_console();

	log_console("Hack was injected! \n");
	log_console(" \n");

	log_console("Get modules... \n");
	m::init();
	log_console(" \n");

	while (!GetModuleHandleA("serverbrowser.dll"))
		Sleep(200);

	log_console("Initalizing threads \n");
	Threading::InitThreads();

	AllocateThreadID = PE::GetExport(m::tier0, HASH("AllocateThreadID")).as<ThreadIDFn>();
	FreeThreadID = PE::GetExport(m::tier0, HASH("FreeThreadID")).as<ThreadIDFn>();

	DispatchToAllThreads<ThreadIDFn, AllocateThreadID>(nullptr);

	log_console("  \n");

	config::create_config_folder();

	memory::pattern::parse();
	i::parse();

	//log_console("Get hitchance seed \n");
	//aim_utils::prepare_seed();
	//log_console("  \n");

	render::init();

	netvars::parse();
	game_fn::parse();

	log_console("Initalizing new listeners... \n");
	listener_entity->init_listener();
	listener_event->init_listener();
	log_console(" \n");

	hooks::hook();

#ifdef _DEBUG
	i::engine->execute_cmd_unrestricted("cl_fullupdate");
#endif

	while (!g::uninject)
		Sleep(1);

	log_console("Unloading cheat... \n");

	Sleep(500);
	listener_entity->remove_listener();
	listener_event->remove_listener();

	hooks::unhook();

	DispatchToAllThreads<ThreadIDFn, FreeThreadID>(nullptr);
	Threading::EndThreads();

	destroy_console();
	FreeLibraryAndExitThread(hModule, 1);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, uintptr_t ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)init, hModule, 0, NULL);
		break;
	}
	return TRUE;
}