# Side Channels

This is a repository for side channels I'm building for various classes.

For simplicity, they will all attack the same process: a custom victim process with a shared library for cryptographic functions.
This victim process will periodically perform encryptions using the shared library so that a spy process has ample attempts to steal the key.
The shared library will include poor (but common) practice for encryption, frequently using secret dependant code paths.

## Victim
More info here once I get the victim working!

## Flush and Reload
More info here once I get the Flush and Reload attack working!

## Prime and Probe
More info here once I get the Prime and Probe attack working!

## Port Smash
More info here once I get the Port Smash attack working!
