# Spider
C++
Keylogger client realized for the 3rd year Spider project at Epitech.
It sends the current used process, the mouse and keyboard information.

This project needs the libraries OpenSSL version 0.9.8 and boost version 1.66 to work.
Don't forget to link it in Visual Studio.

The IP and port of the server must be configured in Client.h or conf.txt.
At the connection to the server the client sends 100 and an ID.
To begin the communication the server must send 101 and the same ID.

Demonstration:
![demonstration](https://raw.githubusercontent.com/aveldocquin/Spider/master/docs/images/demonstration.gif)