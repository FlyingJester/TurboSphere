#pragma once
#include "status.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *ExplainError_Socket(enum WSockErr);

/* Intended lifetime:
 Create_Socket, Connect_Socket, Read_Socket/Write_Socket...
...Read_Socket/Write_Socket, Disconnect_Socket, Destroy_Socket.
*/

struct WSocket *Create_Socket(void);
void Destroy_Socket(struct WSocket *aSocket);

/* timeout is in milliseconds. A negative timeout means to wait as long as the
 stack wants to.
*/
enum WSockErr Connect_Socket(struct WSocket *aSocket, const char *aTo,
                             unsigned long aPortNum, long timeout);
enum WSockErr Disconnect_Socket(struct WSocket *aSocket);

/*
 * Checks for an incoming connection on this port and connects aSocket.
*/
enum WSockErr Listen_Socket(struct WSocket *aSocket, unsigned long aPortNum);
struct WSocket *Accept_Socket(struct WSocket *aSocket);

/* char streams are NUL terminated. */
/* Passing in NULL to aTo will result in a new buffer being allocated.
 You can keep passing in this buffer and it will be resized to fit any
 new data.
*/
enum WSockErr Read_Socket(struct WSocket *aSocket, char **aTo);
enum WSockErr Write_Socket(struct WSocket *aSocket, const char *aToWrite);
enum WSockErr Write_Socket_Len(struct WSocket *aSocket, const char *aToWrite, unsigned long len);

enum WSockErr State_Socket(struct WSocket *aSocket);

/* Gets the number of pending bytes. This can increase at any time, so
 you should only trust this to see if there are any pending bytes at all,
 or to get a minimum number of incuming bytes.
*/
unsigned long Length_Socket(struct WSocket *aSocket);

#ifdef __cplusplus
}
#endif
