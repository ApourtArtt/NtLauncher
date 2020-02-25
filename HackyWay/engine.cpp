#include "engine.h"

Engine::Engine(QString NostaleFile, QObject* parent)
    : QObject(parent), patcher(NostaleFile)
{
    pipe = new PipeManager(this);
    nosFilename = NostaleFile;
    qDebug() << "created";
}

bool Engine::connectToAccount(QString username, QString password, QString gfuid)
{
    if(!patcher.patchFile())
        return false;
    gfid = gfuid;
    un = username;
    pw = password;
    codeGen = new CodeGenerator(this);
    connect(codeGen, &CodeGenerator::codeRecieved, this, [=](QString recievedCode)
    {
        pipe->connectAccount(recievedCode, un, gfid);
        startNostale();
    });
    code = codeGen->connectToAccount(un, pw, gfid);
    return true;
}

void Engine::changeNostaleFile(QString NostaleFile)
{
    nosFilename = NostaleFile;
    patcher.changeDir(NostaleFile);
}
/*
void Engine::startInjector()
{
    QProcess *injector = new QProcess(this);
    injector->startDetached("dllexp.exe");
}*/

void Engine::startNostale()
{
    patcher.patchFile();
    QProcess *Nos = new QProcess(this);
    connect(Nos, &QProcess::started, this, [=]()
    {
        injectDll((DWORD)Nos->processId(), "Ayugra.dll");
    });
    qDebug() << Nos->startDetached(patcher.getPatchedFilename(), {"gf"});
    Nos->waitForStarted();
    injectDll((DWORD)Nos->processId(), "Ayugra.dll");
}

bool Engine::injectDll(DWORD processId, std::string dllPath)
{
    HANDLE hThread, hProcess;
    void*  pLibRemote = NULL;
    HMODULE hKernel32 = GetModuleHandleA("Kernel32");
    char DllFullPathName[_MAX_PATH];
    GetFullPathNameA(dllPath.c_str(), _MAX_PATH, DllFullPathName, NULL);
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    char szLibPath[_MAX_PATH];
    strcpy_s(szLibPath, DllFullPathName);
    pLibRemote = VirtualAllocEx(hProcess, NULL, sizeof(szLibPath), MEM_COMMIT, PAGE_READWRITE);
    if (pLibRemote == NULL)
        return false;
    WriteProcessMemory(hProcess, pLibRemote, (void*)szLibPath, sizeof(szLibPath), NULL);
    hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA"), pLibRemote, 0, NULL);
    if (hThread == NULL)
        return false;
    return true;
}
