#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <coap2/coap.h>

coap_context_t  *ctx = NULL;
coap_session_t *session = NULL;
coap_address_t dst;
coap_pdu_t *pdu = NULL;

void cleanup();
int resolve_address();
void response_handler(struct coap_context_t *context, struct coap_address_t *remote, struct coap_pdu_t *sent, struct coap_pdu_t *received, const coap_tid_t id);

int main() {
    coap_startup();
    
    if (resolve_address("localhost", "5683", &dst) < 0) {
        printf("cannot resolve hostname\n");
        cleanup();
        return EXIT_FAILURE;
    }

    ctx = coap_new_context(NULL);

    if(!ctx || !(session = coap_new_client_session(ctx, NULL, &dst, COAP_PROTO_UDP))) {
        printf("cannot create client session\n");
        cleanup();
        return EXIT_FAILURE;
    }

    coap_register_response_handler(ctx, &response_handler);

    pdu = coap_pdu_init(COAP_MESSAGE_CON,
                        COAP_REQUEST_GET,
                        0,
                        coap_session_max_pdu_size(session));

    if(!pdu)  {
        printf("cannot create message");
        cleanup();
        return EXIT_FAILURE;
    }

    coap_send(session, pdu);
    coap_run_once(ctx, 0);

    cleanup();
    return EXIT_SUCCESS;
}

void cleanup() {
  coap_session_release(session);
  coap_free_context(ctx);
  coap_cleanup();
}

int resolve_address(const char *host, const char *service, coap_address_t *dst) {

  struct addrinfo *res, *ainfo;
  struct addrinfo hints;
  int error, len=-1;

  memset(&hints, 0, sizeof(hints));
  memset(dst, 0, sizeof(*dst));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_family = AF_UNSPEC;

  error = getaddrinfo(host, service, &hints, &res);

  if (error != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
    return error;
  }

  for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
    switch (ainfo->ai_family) {
    case AF_INET6:
    case AF_INET:
      len = dst->size = ainfo->ai_addrlen;
      memcpy(&dst->addr.sin6, ainfo->ai_addr, dst->size);
      goto finish;
    default:
      ;
    }
  }

 finish:
  freeaddrinfo(res);
  return len;
}

void response_handler(struct coap_context_t *context, struct coap_address_t *remote, struct coap_pdu_t *sent, struct coap_pdu_t *received, const coap_tid_t id) {
    printf("%u", *received->data);
    printf("\n");
}
