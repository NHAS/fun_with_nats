# Fun with NATs
So I heard you like some cheeky NATs, and not being able to talk to clients on the other side of those stateful firewalls.  

Be a shame if, someone did something stupid and broke that. 

Turns out if you just shoot off a bunch of UDP packets with known SRC and DST port numbers stateful firewalls get confused. 
Meaning you only have to know someones IP address, and have them using this on their network with your IP address. 

Bing bang boom you can now talk through your NAT. Magic.
