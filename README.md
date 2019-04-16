# Swurl

A wrapper library for CURL on the Nintendo Switch. The objective is to have a simple library where you can pass it a request and recieve back calls on its progress updates, completion and if it incountered an error. Right now the response object is very basic with the parsed headers, raw headers, raw body, and status code. However I plan on in the future allowing for you to pass the response type through the request and have the body be parsed out. (Ex JSON)

## Installation

```bash
make
sudo make install
```

## Usage

I've included an example under the example folder. Remember to add `-lSwurl -lcurl -lz -lmbedtls -lmbedx509 -lmbedcrypto` to `LIBS` in your makefile. Other than that remember to run `SessionManager::initialize();` at the beginning of your app as this initializes sockets and curl, and call `SessionManager::dealloc();` when cleaning up your app as this closes sockets and curl.
