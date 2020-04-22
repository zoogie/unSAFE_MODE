# unSAFE_MODE

## Intro

This is a new exploit for SAFE_MODE system updater. This is the recovery mode app that appears when L+R+Up+A are held while coldbooting the 3DS.<br>
It's normally used to internet update the 3DS from a corrupted state and hopefully repair any damaged titles.<br>
Because it runs *under* SAFE_MODE firm, it's a very interesting (and safe) hax target ;)<br>

## Directions 

Download the zip archive under the Release tab above and follow the instructions inside. You will need some sort of userland exploit to install a hacked wifi save. Details for two of these exploits are included.<br>
Works on firmwares:<br>
old3ds 6.0 - latest<br>
new3ds 8.1 - latest (all versions)

## Exploit 

When SAFE_MODE sysupdater launches, it will check all 3 wifi slots for a working access point to perform a sysupdate. If it can't find one, it will allow the user to access wifi connection settings to make changes.
When Proxy Settings -> Detailed Setup is selected, the displayed proxy URL string is not adequately checked for length, and a stack smash is possible if the attacker had previously altered the location of the string's NULL terminator in the wifi slot data.<br><br>
In order to do the necessary slot modification, userland execution is needed with either cfg:i or cfg:s available. It's possible to attain this service with a small modification to the "*hax" source, or running an mset entrypoint (i.e. bannerbomb3). Note that SAFE_MODE sysupdater is actually a fork of firmware 1.0's mset (System Settings). As a result, mset also had this same bug, but it was fixed in firmware 3.0. The fix obviously was never backported to SAFE_MODE sysupdater, as SAFE_MODE titles are seldom updated for whatever reason.

## FAQ

Q: Um, ... is this unsafe?<br>
A: It's no more unsafe than any other full exploit chain in terms of user safety. The "unsafe" part is ribbing Nintendo for calling SAFE_MODE as such given, from their perspective, it's full of exploitable bugs (since they never backport fixes from NATIVE_FIRM). From the user's standpoint, on the other hand, uSM is safe enough for safehax!<br>

Q: One of my shoulder buttons is hosed, what can I do?<br>
A: There's always ntrboot or seedminer  ¯\\_(ツ)_/¯<br>

Q: You mentioned safehax a couple of times, does uSM have that?<br>
A: It's bundled in, yes. usm.bin contains the safehax code (and several other stages). It will automatically install boot9strap for permanent cfw.

Q: Is this fixable with a firmware update?<br>
A: I think so. Nintendo has a weird track record ignoring my previous exploits, but they could fix this, and possibly do so without even touching SAFE_MODE titles (they prefer leaving SAFE_MODE untouched, it seems). While the fix I'm thinking of is pretty straightforward, I'd rather not give any hints in the unlikely event they act on this exploit.

## Thanks 
This project is licensed as MIT except the code used and modified from these other projects:<br><br>

General Rop/Code
- yellows8 https://github.com/yellows8/3ds_ropkit (otherapp loader)
- dukesrg https://github.com/dukesrg/rop3ds (rop templates, macros)
- smealum https://github.com/zoogie/ninjhax2.x (ninjhax fork with cfg:s changes)

Safehax related
- kartik https://github.com/hax0kartik/pre9otherapp (k11/a9 sploit framework)
- aliaspider https://github.com/aliaspider/svchax/ (memchunkhax1)
- patois https://github.com/patois/Brahma (firmlaunchhax)
- tuxsh https://github.com/TuxSH/usr2arm9ldr (rsaverify)
- normmatt https://gist.github.com/Normmatt/b72f7323686af5c9cd7 (rsaverify)
- SciresM https://github.com/SciresM/boot9strap (mini b9s installer template & b9s itself)
- AuroraWright https://github.com/AuroraWright/SafeA9LHInstaller (writefirm)
