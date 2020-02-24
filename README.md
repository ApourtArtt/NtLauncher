# NtLauncher

This is my work on doing a NosTale launcher, since Gameforge's one is really bad.
Please, note that there is a really easier possibility than using my project : https://github.com/morsisko/NosTale-Gfless.

I could not figured out why I was still getting "gf init fail" message, so here is my version.

## How it works

 - You will be asked to enter your accounts, it will be automaticaly saved (and loaded) in accounts.txt.
 - You will also be asked to enter NostaleClientX.exe path.
 
Then, when you'll click on "Connect", after selecting an account,
 - NostaleClientX.exe will be patched in order to bypass "gf init fail" message (adding an unconditionnal jump), new file will be called NostaleClientXPatched.exe.
 - Last step, this launcher will start NostaleClientXPatched.exe and inject a dll (called Ayugra.dll, in my case) which will ask for authentification token, username, NostaleClientX.exe md5 hash and gfuid to successfully log in. Those informations are needed because we are bypassing gf_wrapper.dll functions.
 


Please, note that all my named pipe messages are removed from this git repository, and Ayugra.dll is private.
