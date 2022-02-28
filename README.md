# Project 1 

This project is about creating a TCP server which will communicate via HTTP protocol. The server will return various infromation about itself.

Author: Petr Hýbl (xhyblp01)
[xhyblp01@vutbr.cz](mailto:xhyblp01@vutbr.cz)

## Instalation

Run the Makefile command to compile the project

```
$ make
```
This will create an executable file called *hinfosvc* with this file you can start your server.


## Usage

To run a server you need to execute following command:
```
$ ./hinfosvc 11111
```
where *11111* is a port number for your local server.

List of possible commands:
 * load - returns cpu usage
 * cpu-name - returns a name of cpu
 * hostname - returns a name of computure where the server is running



## Examples

```
$ curl http://localhost:11111/hostname
$ curl http://localhost:11111/load
$ curl http://localhost:11111/cpu-name
```