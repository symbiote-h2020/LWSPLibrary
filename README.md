# LWSPLibrary

This Repo contains a C++ library that exposes api for use the lwsp in symbiote.
This is a Netbeans project, you can open it and compile using this IDE.
The needed api are exported as C function, thus they can be called also in python as external library - this is because if exported as C++ method this can't be done.

## How it works

LWSP is a lightweight protocol that establish a secure communication tunnel between the symbiote agent and the Innkeeper. After that, data can flow crypted between the two endpoints.
LWSP handle two couple of message:
- Hello message (and its response from innk)
- AuthN (and its response from innk)

Between this two messages the LWSP calculates the crypto stuff and after that it can crypt/decrypt data.
Please remember that the library only creates and interprets the string, it doesn't send data.
To properly make a symbiote-agent remember that you should use the interface for registration of sdev, then the interface to join its resources and periodically sends a keepalive message.

# API

The relevant api that you should use are (they are listed in the order you should call):
## begin(char* SSPId)
Initialize the library.

- in: char* SSPId - the SSP identifier, it can be the ssid that you are connected with.

- return value:  void

## char* sendSDEVHelloToGW(char* mac)
Prepare the first message to begin the LWSP.

- in: char* mac - the mac address of your SDEV

- return value: the Json string that you should put in the body of the POST to send to innkeeper.

## uint8_t elaborateInnkResp( char* resp)
Elaborate the response of the innkeeper. You have to call this method for both the message received from the innkeeper in the sendHello and authN.

-in: char* resp - the Json response get back from innkeeper.

- return value: a enum rapresenting the status of the request. It can be:
	- COMMUNICATION_OK (0xEE)
	- COMMUNICATION_ERROR (0x10)
	- JSON_PARSE_ERR (0xED)

## void calculateDK1(uint8_t num_iterations)
Do things to calculate the DK1

- in: uint8_t num_iterations - the number of iteration to retrive the key. At the actual implementation, you must use num_iterations = 4

- return value:  void

## void calculateDK2(uint8_t num_iterations)
Do things to calculate the DK2

- in: uint8_t num_iterations - the number of iteration to retrive the key. At the actual implementation, you must use num_iterations = 4

- return value:  void

## char* sendAuthN()
Send the authN message to innkeeper. 
Remember to call elaborateInnkResp with the response get back from innk.

- in: void

- return value: char* - the Json string that you should put in the body of the POST to send to innkeeper.

## const char* preparePacket(char* semantic)
Crypt the message to be send to innkeeper.
 - in: char * semantic - the data to be crypted and sent using the LWSP

 - return value: char* - the Json string that you should put in the body of the POST to send to innkeeper.

## char* decryptPacketFromInnk(char* data)
Decrypt the innkeeper response.
- in: char* data - the Json message coming from the innkeeper. 

- return value: const char* - the decrypted content of the message sent from innkeeper

# Use the library in python

You have to use python2.7 and import the library using *ctypes*.
There is an example script that show you how to do that.