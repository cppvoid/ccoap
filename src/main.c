#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <coap2/coap.h>

coap_context_t  *ctx = NULL;
coap_session_t *session = NULL;
coap_address_t dst;
coap_pdu_t *pdu = NULL;

int main() {
    coap_startup();

    dst.addr.sin.sin_family = AF_INET;
    dst.addr.sin.sin_port = htons(5683);
    dst.addr.sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    ctx = coap_new_context(NULL);

    if(!ctx || !(session = coap_new_client_session(ctx, NULL, &dst, COAP_PROTO_UDP))) {
        printf("cannot create client session\n");
        cleanup();
        return EXIT_FAILURE;
    }

    cleanup();
}

void cleanup() {
  coap_session_release(session);
  coap_free_context(ctx);
  coap_cleanup();
}