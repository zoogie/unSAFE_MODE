About
==============
This is a generalistic, _position-independant_ payload that, being provided the userland-to-kernel9 exploit chain, sets up the environment as cleanly as possible. All Brahma payloads should be compatible with this, **including applications that are using the arm11**.

This payload is entirely independant from the entrypoint it is executed from. All you have to do is drop your exploit chain in `exploit.c`.
It already contains the srv:pm into VerifyShaRsa256 exploit-chain.

It been thoroughly tested on 2.1 using the browser. Help is needed to test/fix support from the remaining system version, up to 5.0 (non-inclusive).
This started as being a 2xrsa fork, but I ended up rewriting everything entirely.

Credits
==============
- 2xrsa credits:
    - Normmatt for the rsa exploit
    - dukesrg for rop3ds and hosting
    - Gelex for debugging and testing
    - stuckpixel, others for testing
    - anon contrib for initial version of stager
