# Point Of Sale

## Introduction

This is a proof-of-concept implementation for a Point-Of-Sale terminal, with implementations for both the client and the server
endpoints. The client is in `client.c`, the server is in `server.c` and the database file being used is `Database.txt`. The
database I/O is being performed by `transaction.c`. Exported functionality is recorded in the respective header files.

## Installation

We follow a standard UNIX project distribution format. As such, the installation is just:

```
$ make
```

This builds the code. You will need the **pthread** library, and a standard **libc** for compilation though.

## Usage

We follow the usage format specified in the provided specification. To boot up the server, first choose any port
outside the reserved, i.e, and port about 1024, say 8090. Then run:

```
$ ./server 8090
```

This starts up a blocking server in your terminal, which will accept requests from any clients on a local network. To
close the server, just use **Ctrl-C**. This will gracefully terminate the process.

Next, to open up a client terminal, use:

```
$ ./client <IP of server host> 8090
```

For example, if you use the same host, the IP will be the standard `127.0.0.1`. No DNS expansion is provided, so `localhost`
is invalid. You can start multiple hosts; the server is _capable of processing data from many hosts at the same time_.

Once you fire up a client terminal, you will be greeted with an interactive portal which can be used to buy items, and it will
total up your expenses automatically. The interface of the client is designed to self-explanatory. Use `rlwrap` for history
support.

Shutting down the client can be down by simply using the provided option in the menu.

## Database format

The database follows a simple textual format, as specified in the specification. Each record follows the syntax:

```
<UPC Code> <Product Name> <Price in required currency>
```

The database file simply consists of a collection of such records, one in each line.
