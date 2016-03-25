# sha256-random-attack
Small piece of code attacking sha256 hashs using brute-force &amp; luck to try to create collisions

This is purely theorical & shows how difficult it is to "crack" this code.

You'll need openssl-dev to compile this.

Example :

\# echo -n "0000" | sha256sum -

\# ./sha256-random-attack -s 4 -d 9af15b336e6a9619928537df30b2e6a2376569fcf9d7e773eccede65606529a0 > password.txt

Good luck !
