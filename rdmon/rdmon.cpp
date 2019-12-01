#include "pch.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cereal/cereal.hpp>

#include <czmq.h>

int main(void)
{
	//  Socket to talk to clients
	void *context = zmq_ctx_new();
	void *responder = zmq_socket(context, ZMQ_REP);
	int rc = zmq_bind(responder, "tcp://*:5555");
	assert(rc == 0);

	while (1) {
		char* msg = zstr_recv(responder);
		printf("Received '%s'\n", msg);
		zstr_free(&msg);
		zstr_send(responder, "Roger that");
	}
	return 0;
}