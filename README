-----------------------------------------------------------------------------
Beats of Rage 1.0028
Dreamcast port by Neill Corlett
-----------------------------------------------------------------------------

Visit the Beats of Rage web site at http://senile.mine.nu/beatsofrage/

-----------------------------------------------------------------------------

Why did I port this again?
--------------------------

I think a lot more people will be able to appreciate this port than my PS2
port.  The Dreamcast is a far cheaper, far more homebrew-friendly console
than the PS2, and it boots CD-Rs out of the box.  I would've done a Dreamcast
port first, if not for the memory requirements.  The Dreamcast might not be
as powerful as the PS2, but it's a lot more fun to develop for.

And besides, why not?  A Streets of Rage remake belongs on a Sega console.

-----------------------------------------------------------------------------

How tough was this port?
------------------------

Most of the portability challenges were already taken care of in the PS2
port, so that made it a lot easier.  And again, the Dreamcast SH4 has the
same byte order as the PS2 EE.  See, life would be so much simpler if
everyone just conformed to little-endian.

Since the Dreamcast BIOS doesn't automatically perform read-ahead on GDROM
calls, I had to almost completely rewrite filecache.c.  I am particularly
proud of this new implementation.  Even though the load times ended up a bit
slower than the PS2 version, this time *my* code is doing all the work!

There was, of course, the other small matter of how you fit a 24MB game into
16MB of memory.  This turned out to be simple:  The vast majority of memory
is used for sprites, and Beats of Rage was keeping two copies of each sprite
in memory (one normal, one flipped).  All I had to do was add some code to
flip them on the fly instead, and presto - 50% memory savings.

-----------------------------------------------------------------------------

Does this version support 4 players?
------------------------------------

No, but you can remap controller 3 and 4's buttons onto player 1 or 2.
*golf clap*

-----------------------------------------------------------------------------

How come you can use the analog pad in this one, but not the PS2 one?
---------------------------------------------------------------------

Let's just say I had a feature request.

-----------------------------------------------------------------------------

Gosh, will that stub really work on any UCLPACKed executable?
-------------------------------------------------------------

Yes, but only --2e.

-----------------------------------------------------------------------------

How the hell did you get streaming sound without writing an ARM driver?
-----------------------------------------------------------------------

I know AICA kung fu.

-----------------------------------------------------------------------------

Why was ProtoCat not mentioned in the PS2 port's README.TXT?
------------------------------------------------------------

Because he didn't eat his Sony Flakes(TM).

-----------------------------------------------------------------------------

Where to find me
----------------

email: neill@neillcorlett.com
web:   http://www.neillcorlett.com/

-----------------------------------------------------------------------------
