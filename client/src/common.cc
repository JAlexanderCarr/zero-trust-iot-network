/* minimal CoAP functions
 *
 * Copyright (C) 2018-2021 Olaf Bergmann <bergmann@tzi.org>
 */

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "common.h"

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

static char *cert_file = NULL;
static char *key_file = NULL;
static char *ca_file = NULL;
static int verify_peer_cert = 1;

static void update_pki_key(coap_dtls_key_t *dtls_key,
                           const char *key_name,
                           const char *cert_name,
                           const char *ca_name)
{
    memset(dtls_key, 0, sizeof(*dtls_key));
    dtls_key->key_type = COAP_PKI_KEY_PEM;
    dtls_key->key.pem.public_cert = cert_name;
    dtls_key->key.pem.private_key = key_name ? key_name : cert_name;
    dtls_key->key.pem.ca_file = ca_name;

}

static int verify_cn_callback(const char *cn,
                              const uint8_t *asn1_public_cert COAP_UNUSED,
                              size_t asn1_length COAP_UNUSED,
                              coap_session_t *session COAP_UNUSED,
                              unsigned depth,
                              int validated COAP_UNUSED,
                              void *arg
)
{
    union
    {
        coap_dtls_role_t r;
        void *v;
    } role = {.v = arg};

    coap_log(LOG_INFO, "CN '%s' presented by %s (%s)\n",
             cn, role.r == COAP_DTLS_ROLE_SERVER ? "client" : "server",
             depth ? "CA" : "Certificate");
    return 1;
}

coap_dtls_pki_t *setup_pki(coap_dtls_role_t role)
{
    static coap_dtls_pki_t dtls_pki;
    
    memset(&dtls_pki, 0, sizeof(dtls_pki));
    dtls_pki.version = COAP_DTLS_PKI_SETUP_VERSION;
    if (ca_file)
    {
        dtls_pki.verify_peer_cert = verify_peer_cert;
        dtls_pki.check_common_ca = 1;
        dtls_pki.allow_self_signed = 1;
        dtls_pki.allow_expired_certs = 1;
        dtls_pki.cert_chain_validation = 1;
        dtls_pki.cert_chain_verify_depth = 2;
        dtls_pki.check_cert_revocation = 1;
        dtls_pki.allow_no_crl = 1;
        dtls_pki.allow_expired_crl = 1;
        dtls_pki.allow_bad_md_hash       = 1;
        dtls_pki.allow_short_rsa_length  = 1;
    }

    dtls_pki.is_rpk_not_cert = 0;
    dtls_pki.validate_cn_call_back = verify_cn_callback;
    dtls_pki.cn_call_back_arg = (void *) role;
    dtls_pki.validate_sni_call_back = NULL;
    dtls_pki.sni_call_back_arg = NULL;

    update_pki_key(&dtls_pki.pki_key, key_file, cert_file, ca_file);
    return &dtls_pki;
}


void fill_keystore(coap_context_t *ctx)
{

    if (!coap_dtls_is_supported())
    {
        coap_log(LOG_DEBUG,
                 "(D)TLS not enabled as neither -k or -c options specified\n");
        return;
    }

    if (cert_file)
    {
        coap_dtls_pki_t *dtls_pki = setup_pki(COAP_DTLS_ROLE_SERVER);
        if (!coap_context_set_pki(ctx, dtls_pki))
        {
            coap_log(LOG_INFO, "Unable to set up PKIkeys\n");
        }
    }
}

void set_certificate(char *full_file_path)
{
    cert_file = full_file_path;
}

void set_certificate_authority(char *full_file_path)
{
    ca_file = full_file_path;
}

void set_private_key(char *full_file_path)
{
    key_file = full_file_path;
}
