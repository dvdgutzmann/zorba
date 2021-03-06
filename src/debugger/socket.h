/*
 * Copyright 2006-2016 zorba.io
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ZORBA_DEBUGGER_SOCKET_H
#define ZORBA_DEBUGGER_SOCKET_H

#include <string>

#include <zorba/debugger_exception.h>

#ifdef _WIN64
  typedef unsigned __int64 SOCKET;
#else
#ifdef _WIN32
  typedef unsigned int __w64 SOCKET;
#else
# define INVALID_SOCKET -1
typedef int SOCKET;
#endif
#endif

namespace zorba {

class DebuggerDebuggerSocketException;

/**
 *   Base class representing basic communication endpoint
 */
class Socket {
public:
  /**
   *   Close and deallocate this socket
   */
  ~Socket();

  /**
   *
   */
  void close();

  /**
   *   Get the local address
   *   @return local address of socket
   *   @exception DebuggerSocketException thrown if fetch fails
   */
  std::string getLocalAddress();

  /**
   *   Get the local port
   *   @return local port of socket
   *   @exception DebuggerSocketException thrown if fetch fails
   */
  unsigned short getLocalPort();

  /**
   *   Set the local port to the specified port and the local address
   *   to any interface
   *   @param localPort local port
   *   @exception DebuggerSocketException thrown if setting local port fails
   */
  void setLocalPort(unsigned short localPort);

  /**
   *   Set the local port to the specified port and the local address
   *   to the specified address.  If you omit the port, a random port 
   *   will be selected.
   *   @param localAddress local address
   *   @param localPort local port
   *   @exception DebuggerSocketException thrown if setting local port or address fails
   */
  void setLocalAddressAndPort(const std::string &localAddress, 
    unsigned short localPort = 0);

  /**
   *   If WinSock, unload the WinSock DLLs; otherwise do nothing.  We ignore
   *   this in our sample client code but include it in the library for
   *   completeness.  If you are running on Windows and you are concerned
   *   about DLL resource consumption, call this after you are done with all
   *   Socket instances.  If you execute this on Windows while some instance of
   *   Socket exists, you are toast.  For portability of client code, this is 
   *   an empty function on non-Windows platforms so you can always include it.
   *   @param buffer buffer to receive the data
   *   @param bufferLen maximum number of bytes to read into buffer
   *   @return number of bytes read, 0 for EOF, and -1 for error
   *   @exception DebuggerSocketException thrown WinSock clean up fails
   */
  static void cleanUp();

  /**
   *   Resolve the specified service for the specified protocol to the
   *   corresponding port number in host byte order
   *   @param service service to resolve (e.g., "http")
   *   @param protocol protocol of service to resolve.  Default is "tcp".
   */
  static unsigned short resolveService(const std::string &service,
                                       const std::string &protocol = "tcp");

  SOCKET theDescriptor;

private:
  // Prevent the user from trying to use value semantics on this object
  Socket(const Socket &sock);
  void operator=(const Socket &sock);

protected:            // Socket descriptor
  Socket(int aType, int aProtocol);
  Socket(SOCKET aDescriptor);
};

/**
 *   Socket which is able to connect, send, and receive
 */
class CommunicatingSocket : public Socket {
public:
  /**
   *   Establish a socket connection with the given foreign
   *   address and port
   *   @param foreignAddress foreign address (IP address or name)
   *   @param foreignPort foreign port
   *   @exception DebuggerSocketException thrown if unable to establish connection
   */
  void connect(const std::string &foreignAddress, unsigned short foreignPort);

  /**
   *   Write the given buffer to this socket.
   *   Call connect() before calling send().
   *   @param buffer buffer to be written
   *   @param bufferLen number of bytes from buffer to be written
   *   @exception DebuggerSocketException thrown if unable to send data
   */
  void send(const void *buffer, int bufferLen);

  /**
   *   Read into the given buffer up to bufferLen bytes data from this socket.
   *   Call connect() before calling recv().
   *   @param buffer buffer to receive the data
   *   @param bufferLen maximum number of bytes to read into buffer
   *   @return number of bytes read, 0 for EOF, and -1 for error
   *   @exception DebuggerSocketException thrown if unable to receive data
   */
  int recv(void *buffer, int bufferLen);

  /**
   *   Get the foreign address.  Call connect() before calling recv()
   *   @return foreign address
   *   @exception DebuggerSocketException thrown if unable to fetch foreign address
   */
  std::string getForeignAddress();

  /**
   *   Get the foreign port.  Call connect() before calling recv()
   *   @return foreign port
   *   @exception DebuggerSocketException thrown if unable to fetch foreign port
   */
  unsigned short getForeignPort();

protected:
  CommunicatingSocket(int type, int protocol);
  CommunicatingSocket(SOCKET newConnSD);
};

/**
 *   TCP socket for communication with other TCP sockets
 */
class TCPSocket : public CommunicatingSocket {
public:
  /**
   *   Construct a TCP socket with no connection
   *   @exception DebuggerSocketException thrown if unable to create TCP socket
   */
  TCPSocket();

  /**
   *   Construct a TCP socket with a connection to the given foreign address
   *   and port
   *   @param foreignAddress foreign address (IP address or name)
   *   @param foreignPort foreign port
   *   @exception DebuggerSocketException thrown if unable to create TCP socket
   */
  TCPSocket(const std::string &foreignAddress, unsigned short foreignPort);

private:
  // Access for TCPServerSocket::accept() connection creation
  friend class TCPServerSocket;
  TCPSocket(SOCKET newConnSD);
};

/**
 *   TCP socket class for servers
 */
class TCPServerSocket : public Socket {
public:
  /**
   *   Construct a TCP socket for use with a server, accepting connections
   *   on the specified port on any interface
   *   @param localPort local port of server socket, a value of zero will
   *                   give a system-assigned unused port
   *   @param queueLen maximum queue length for outstanding 
   *                   connection requests (default 5)
   *   @exception DebuggerSocketException thrown if unable to create TCP server socket
   */
  TCPServerSocket(unsigned short localPort, int queueLen = 5);

  /**
   *   Construct a TCP socket for use with a server, accepting connections
   *   on the specified port on the interface specified by the given address
   *   @param localAddress local interface (address) of server socket
   *   @param localPort local port of server socket
   *   @param queueLen maximum queue length for outstanding 
   *                   connection requests (default 5)
   *   @exception DebuggerSocketException thrown if unable to create TCP server socket
   */
  TCPServerSocket(const std::string &localAddress, unsigned short localPort,
      int queueLen = 5);

  /**
   *   Blocks until a new connection is established on this socket or error
   *   @return new connection socket
   *   @exception DebuggerSocketException thrown if attempt to accept a new connection fails
   */
  TCPSocket *accept();

private:
  void setListen(int queueLen);
};
}//end of namespace
#endif
