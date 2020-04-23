# unSAFE_MODE

## Intro

This is a new exploit for SAFE_MODE system updater. SAFE_MODE sysupdater is the recovery mode app that launches when L+R+Up+A are held while coldbooting the 3DS. It's normally used to internet update the 3DS from a corrupted state and hopefully repair any damaged system titles. Because it runs *under* SAFE_MODE firm, it's a very interesting (and safe) hax target ;)

## Directions 

Download the zip archive under the Release tab above and follow the instructions inside. You will need some sort of userland exploit to install a hacked wifi save. Details for two suitable exploits are included.<br>
Works on firmwares:<br>
`
old3ds 6.0 - latest<br>
new3ds 8.1 - latest (all versions)
`

## Exploit 

When SAFE_MODE sysupdater launches, it will check all 3 wifi slots for a working access point to perform a sysupdate. If it can't find one, it will allow the user to access wifi connection settings to make changes. When Proxy Settings -> Detailed Setup is selected, the displayed proxy URL string is not adequately checked for length, and a stack smash is possible if the attacker had previously altered the location of the string's NULL terminator in the wifi slot data.<br><br>
In order to prepare the necessary slot modification, userland execution is needed with either cfg:i or cfg:s available. For example, it's possible to attain the cfg:s service with a small modification to the "*hax" otherapp source, or running an mset entrypoint (i.e. bannerbomb3). Note that SAFE_MODE sysupdater is actually a fork of firmware 1.0's mset (System Settings). As a result, mset also had this same bug at one point, but it was fixed in firmware 3.0. The fix obviously was never backported to SAFE_MODE sysupdater due to the fact that SAFE_MODE titles are seldom updated for whatever reason.

## FAQ

Q: Um, ... is this unsafe?<br>
A: It's no more unsafe than any other full exploit chain in terms of user safety. The "unsafe" part is ribbing Nintendo for calling SAFE_MODE as such given, from their perspective, it's full of exploitable bugs (since they never backport fixes from NATIVE_FIRM). The name also refers to the exploit running "un"der SAFE_MODE firm, which is a unique (and nice) aspect of the version of safehax this exploit bootstraps.

Q: One of my shoulder buttons is hosed, what can I do?<br>
A: Some people report that blowing hot humid air into the buttons temporarily allows them to work, but that's just gross and unsanitary (I'd totally do it, but I'm a weirdo).<br>
The best plan B is probably to just use ntrboot or seedminer.

Q: You mentioned safehax a couple of times, does unSAFE_MODE have that?<br>
A: It's bundled in, yes. Usm.bin contains the safehax code (and several other stages). It will automatically install boot9strap to firm0 for permanent cfw.

Q: Is this fixable with a firmware update?<br>
A: I think so. Nintendo has a weird track record ignoring my previous exploits, but they could fix this, and possibly do so without even touching SAFE_MODE titles (they prefer leaving SAFE_MODE untouched, as already mentioned). While the fix I'm thinking of is pretty straightforward, I'd rather not give any hints in the event they attempt to patch this exploit.

## Thanks 
This project is licensed as MIT except the code used and modified from these other projects:<br><br>

General Rop/Code
- yellows8 https://github.com/yellows8/3ds_ropkit (otherapp loader)
- dukesrg https://github.com/dukesrg/rop3ds (rop templates, macros)
- smealum https://github.com/zoogie/ninjhax2.x (ninjhax fork with cfg:s changes - superto and o3ds_newpayloads branches)

Safehax related
- kartik https://github.com/hax0kartik/pre9otherapp (k11/a9 sploit framework)
- aliaspider https://github.com/aliaspider/svchax/ (memchunkhax1)
- patois https://github.com/patois/Brahma (firmlaunchhax)
- tuxsh https://github.com/TuxSH/usr2arm9ldr (rsaverify)
- normmatt https://gist.github.com/Normmatt/b72f7323686af5c9cd7 (rsaverify)
- SciresM https://github.com/SciresM/boot9strap (mini b9s installer template & b9s itself)
- AuroraWright https://github.com/AuroraWright/SafeA9LHInstaller (writefirm)
