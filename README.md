Bat Chat
========

A secure chat service for friends.

Bat Chat offers text and voice communication with peers.

Bat Chat does not keep any persistent records of your activity on
our system. What records we keep during the lifetime of your session
are purged once the client disconnects from the server. After
disconnect it is as if you were never online.

We also encrypt your communication with those in your chat channel.
The requests you send the server are encrypted and the replies
received from the server are encrypted.

No registration is required nor requested. To use our service you
just need to download a client, run it and choose any display name
and channel your friends are in at the login screen. After that you
may chat with confidence!

This software was first developed at Citrus Hack 2014, University of
California at Riverside. This project continued its development
after the hackathon and is continually developed.

The source code is broken up into two parts--the server and client.

The server is a concurrent program utilizing as multithreaded
approach. Each new client connection spawns a new worker thread
dedicated to fulfilling the client's needs. The server is written
in a mix of languages. Most is written in C++ with ties to a MySQL
backend. This MySQL backend is accessed through a PHP handler on
an Apache server. The database backend is used to keep a stable
record of current client connections and chat channels. There is
a "clean-up" daemon running on the server-side to purge any old
user records if there has been inactivity for a long time
(ensuring we do not keep data for too long after use).

The client is designed to be a single-threaded process and first
line of defense against eavesdroppers. The client's duty is to
ensure that your chat history is not kept beyond the lifetime of
your session and to encrypt/decrypt messages to/from server.

The client implements a number of features to make your
experience efficient and memorable.
