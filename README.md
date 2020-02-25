# NtLauncher

There is two versions :
- HackyWay which is patching NostaleClientX.exe to allow to start with gf parameter without any named pipe. Use it only if you have a login dll. It launchs by default Ayugra.dll 
- PipeWay, which is just using the "normal protocol" (see more here : https://github.com/morsisko/NosTale-Gfless). If you have an injector.exe recieving DllPath and Pid as parameter, you can also use it, just leave it in the same repository, and here we go. By default, its name is also Ayugra.dll
