##       Problem Statement
--------------------------------------------

We were asked to create a Network simulator. <br>
to design and implement <br>
Network discovery protocols <br>
Routing protocol (optional) <br>
and Protocols to monitor health of the network <br>
for a wireless network

##      Motivation
------------------------------------------------

To brush up my knowledge in networking :-)

##      Project health
---------------------------------------------------

Aiming for a strongly consistent network aware LinkState protocol for networkupdate <br>

##      How to use
-----------------------------------------------------
Ensure sqlite is installed <br>
Run<br>
make -f makefile all <br>
chmod 777 . <br>
./simulator.out<br>
<br>

##      Project details
-----------------------------------------------------
Network simulation is done leveraging linux processes and message Queue ( and signals) IPC <br>
sqlite3 is used for db maintenance <br>
