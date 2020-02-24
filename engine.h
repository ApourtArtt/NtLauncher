#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <windows.h>
#include <signal.h>
#include "pipemanager.h"
#include "codegenerator.h"
#include "FilePatching.h";

class Engine : public QObject
{
    Q_OBJECT
public:
    Engine(QString NostaleFile, QObject *parent = nullptr);
    bool connectToAccount(QString username, QString password, QString gfuid);
    void changeNostaleFile(QString NostaleFile);

private:
    void startNostale();
    //void startInjector();
    bool injectDll(DWORD processId, std::string dllPath);
    PipeManager *pipe, *pipeService;
    CodeGenerator *codeGen;
    NetworkRequester *netReq;
    FilePatching patcher;
    QString nosFilename;
    bool started;
    QString un, pw, gfid, code;
};

#endif // ENGINE_H
