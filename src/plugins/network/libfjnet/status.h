#pragma once

struct WSocket;
enum WSockErr {eConnected = 0, eSuccess = 0, eFailure, eNotConnected, eRefused, eTimeout, eAlreadyConnected, ePermission, eBadAddress};
enum WSockType {eRead = 1, eWrite = 2, eError = 4};
