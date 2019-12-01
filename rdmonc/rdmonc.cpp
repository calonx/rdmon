#include "pch.h"

//#include <windows.h>

#include <string.h>
#include <stdio.h>

#include <cereal\cereal.hpp>
#include <cereal\archives\json.hpp>
#include <sstream>

#include "..\rdmon\rdmon_shared.h"

#include <czmq.h>

constexpr wchar_t kReconnectEventName[]  = L"ig_rdmon-Reconnect";
constexpr wchar_t kDisconnectEventName[] = L"ig_rdmon-Disconnect";

int main(void)
{
	HANDLE reconnect_event  = CreateEvent(nullptr, false, false, kReconnectEventName);
	HANDLE disconnect_event = CreateEvent(nullptr, false, false, kDisconnectEventName);

	printf("Connecting to hello world server...\n");
	void *context = zmq_ctx_new();
	void *requester = zmq_socket(context, ZMQ_REQ);
	zmq_connect(requester, "tcp://yogcalon:5555");

	std::ostringstream os{};
	cereal::JSONOutputArchive archive(os);

	Datagram myData;
	archive(myData);

	std::string data = os.str();

	int request_nbr;
	for (request_nbr = 0; request_nbr != 10; request_nbr++)
	{
		HANDLE handles[] = { reconnect_event, disconnect_event };
		DWORD ret = WaitForMultipleObjects(2, handles, false, INFINITE);
		if (ret == WAIT_FAILED)
		{
			DebugBreak();
			exit(1);
		}
		int i = (int)(ret - WAIT_OBJECT_0);
		const char* msg;
		if (handles[i] == reconnect_event)
		{
			msg = "RD Connected";
		}
		else
		{
			msg = "RD Disconnected";
		}
		printf("Sending data: '%s'\n", msg);
		zstr_send(requester, msg);

		char* response = zstr_recv(requester);
		printf("Received data: %s\n", response);
		zstr_free(&response);
	}
	getchar();
	zmq_close(requester);
	zmq_ctx_destroy(context);

	CloseHandle(reconnect_event);
	CloseHandle(disconnect_event);
	return 0;
}