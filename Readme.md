##       Problem Statement
--------------------------------------------

We were asked to create a Network simulator. <br>
to design and implement <br>
Network discovery protocols <br>
Routing protocol (optional) <br>
Protocols to monitor health of the network <br>
for a wireless network

##      Motivation
------------------------------------------------

To brush up my knowledge in networking :-)

##      Project health
---------------------------------------------------

Currently,
It has a functional 'Network Discovery' protocol.

##      How to use
-----------------------------------------------------
Run<br>
make -f makefile all <br>
chmod 777 . <br>
./simulator.out<br>
<br>
or use codeblocks to open the project :-)

PS: make sure msgget etc., succseeds

##      Project details
-----------------------------------------------------
Network simulation is done leveraging linux processes and message Queue (/signals) IPC
