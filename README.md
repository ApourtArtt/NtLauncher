# NtLauncher

There are two versions :
- HackyWay which is patching NostaleClientX.exe to allow starting with gf parameter without any named pipe. Use it only if you have a login dll. It launchs by default Ayugra.dll.
- PipeWay, which is just using the "normal protocol" (see more here : https://github.com/morsisko/NosTale-Gfless). If you have an injector.exe receiving DllPath and Pid as parameter, you can also use it, just leave it in the same repository, name it injector.exe. By default, dll name is also Ayugra.dll.

__If you want to compile it by yourself : use PipeWay, it is the simpler way. Use HackyWay only if you have a reason to.__

## I have been asked "how do I use it"

First, download a [release](https://github.com/ApourtArtt/NtLauncher/releases) (or compile it by yourself) and save it wherever you want.

Saying we have this on GfClient : ![Get Account Name](https://zupimages.net/up/20/36/54ba.png)

Your available usernames are clearly written : account1 (top right), account2 and account3 (bot right widget).
Please, don't use the username prefix XX_. If your username is displayed as FR_account2, your __real__ username is account2. You have to use account2.
Their password are the one you used to come to this page.

This launcher looks something like that : ![Get Account Name](https://zupimages.net/up/20/36/fw2k.png)

1 : Lang used for connection. \
2 : Pseudonym displayed on the account list. It can be whatever you want. \
3 : Username needed for connection. \
4 : Password needed for connection. \
5 : (Don't use it if you don't know what it is) it only change the GFUID requested by GF's api. Leave it empty if you don't know what to type. Since Frabruary 2021, it is __necessary__ to use a valid and registered GFUID. \
6 : Code cache is used to bypass anti-bruteforce system. If you don't have one but still want to use it, leave it empty and it will be filled at the first connection. \
7 : Account file, used to regroup some accounts. \

8 : Path to NostaleClientX.exe (used for Update/Repair and launch game) \
9 : PlatformGameId needed by Gf's api. Don't modify it if you don't have a reason to. \
10 : Time between each account connection (cannot connect accounts simultaneously because NosTale is "blocking it"). \
11 : This launcher and gf's one are using the same process : they cannot get along with, so here is an option to shut down Gfclient before the Named Pipe is created. \
12 : Use or not the cache to avoid or not anti-bruteforce system. \
13 : Check on launcher start if there is an update of Ayugra. \
14 : Repair button, it will redownload the whole game. Avoid using it if you have a pretty bad connection. \
15 : Update the game : that is what was missing to have a completely independant launcher right now. \

16 : Account list will be displayed here when you have one or more.
