# NtLauncher

There is two versions :
- HackyWay which is patching NostaleClientX.exe to allow to start with gf parameter without any named pipe. Use it only if you have a login dll. It launchs by default Ayugra.dll 
- PipeWay, which is just using the "normal protocol" (see more here : https://github.com/morsisko/NosTale-Gfless). If you have an injector.exe recieving DllPath and Pid as parameter, you can also use it, just leave it in the same repository, and here we go. By default, its name is also Ayugra.dll

__If you want to compile it by yourself : use PipeWay, it is the simpler way. Use HackyWay only if you want to have headache.__

## I have been asked "how do I use it"

First, download a [release](https://github.com/ApourtArtt/NtLauncher/releases) (or compile it by yourself) and save it wherever you want.

Saying we have this on GfClient : ![Get Account Name](https://zupimages.net/up/20/09/cc9h.png)

Your available usernames are clearly written : MySuperAccount (top right), MySuperAccount1 and MySuperAccount2 (middle widget).

Their password are the one you used to come to this page.
If you want to save MySuperAccount2 in this launcher, here is what it should looks like :
![Setting account informations](https://zupimages.net/up/20/09/7nnm.png)
__DO NOT FORGET TO SET YOUR LANGUAGE BEFORE SAVING YOUR ACCOUNT__

Last step : click on browse and select NostaleClientX.exe in your Nostale repository.

About GFUID : if you do not know what it is, keep it empty, this launcher will chose for you.
