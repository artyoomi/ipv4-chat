# ipv4-chat
Hello there!
To run ipv4-chat program you need:
1) Run "sudo bash build.sh". After that executable will be created
2) Run program with "./build/ipv4-chat" and give it IP and Port to work
3) Finally! It's time to talk to your crazy...
(Debian compatibility)

Description:
It is necessary to implement an IPv4 chat program that exchanges text broadcast
(IPv4 UDP broadcast) messages over a local network segment, with copies running on other
machines physically connected to one logical network segment.
The input program, in the form of mandatory command-line options, must expect an IPv4 address and
port number on which it will expect to receive messages from its other copies running on
other machines of the same network segment. And immediately after launch, it will prompt the user to enter their
nickname.
The program should consist of two threads (POSIX Threads):
- The first thread "listens" to socket (recvfrom) and, after receiving a UDP datagram, displays IPv4
  the sender's address, his nickname, and the received message.
- The second stream, waiting for the user to enter a text message, with a limit of 1000 bytes, after
  the end of the input, along with the entered text message and nickname, generates and sends
  UDP datagram (sendto) to broadcast IPv4 address 255.255.255.255 and port
  (specified in the command line options).
Together with the program, a script should be developed, the launch of which will provide
the necessary conditions for the execution of the program.
