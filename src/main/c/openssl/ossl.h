/*
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2001-2002  Michal Rokos <m.rokos@sh.cvut.cz>
 * All rights reserved.
 */
/*
 * This program is licensed under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#if !defined(_OSSL_H_)
#define _OSSL_H_

#include RUBY_EXTCONF_H

#include <assert.h>
#include <errno.h>
#include <ruby.h>
#include <ruby/io.h>
#include <ruby/thread.h>
#include <openssl/opensslv.h>
#include <openssl/err.h>
#include <openssl/asn1.h>
#include <openssl/x509v3.h>
#include <openssl/ssl.h>
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/conf.h>
#include <openssl/conf_api.h>
#include <openssl/crypto.h>
#if !defined(OPENSSL_NO_ENGINE)
#  include <openssl/engine.h>
#endif
#if !defined(OPENSSL_NO_OCSP)
#  include <openssl/ocsp.h>
#endif

/*
 * Common Module
 */
extern VALUE mOSSL;

/*
 * Common Error Class
 */
extern VALUE eOSSLError;

/*
 * CheckTypes
 */
// TruffleRuby: avoid passing managed to var-args
#define OSSL_Check_Kind(obj, klass) do {\
  if (!rb_obj_is_kind_of((obj), (klass))) {\
    ossl_raise(rb_eTypeError, "wrong argument (%s)! (Expected kind of %s)",\
               RSTRING_PTR(rb_class_name(rb_obj_class(obj))), RSTRING_PTR(rb_class_name(klass)));\
  }\
} while (0)

// TruffleRuby: avoid passing managed to var-args
#define OSSL_Check_Instance(obj, klass) do {\
  if (!rb_obj_is_instance_of((obj), (klass))) {\
    ossl_raise(rb_eTypeError, "wrong argument (%s)! (Expected instance of %s)",\
               RSTRING_PTR(rb_class_name(rb_obj_class(obj))), RSTRING_PTR(rb_class_name(klass)));\
  }\
} while (0)

#define OSSL_Check_Same_Class(obj1, obj2) do {\
  if (!rb_obj_is_instance_of((obj1), rb_obj_class(obj2))) {\
    ossl_raise(rb_eTypeError, "wrong argument type");\
  }\
} while (0)

/*
 * Data Conversion
 */
STACK_OF(X509) *ossl_x509_ary2sk0(VALUE);
STACK_OF(X509) *ossl_x509_ary2sk(VALUE);
STACK_OF(X509) *ossl_protect_x509_ary2sk(VALUE,int*);
VALUE ossl_x509_sk2ary(const STACK_OF(X509) *certs);
VALUE ossl_x509crl_sk2ary(const STACK_OF(X509_CRL) *crl);
VALUE ossl_x509name_sk2ary(const STACK_OF(X509_NAME) *names);
VALUE ossl_buf2str(char *buf, int len);
#define ossl_str_adjust(str, p) \
do{\
    long len = RSTRING_LEN(str);\
    long newlen = (long)((p) - (unsigned char*)RSTRING_PTR(str));\
    assert(newlen <= len);\
    rb_str_set_len((str), newlen);\
}while(0)
/*
 * Convert binary string to hex string. The caller is responsible for
 * ensuring out has (2 * len) bytes of capacity.
 */
void ossl_bin2hex(unsigned char *in, char *out, size_t len);

/*
 * Our default PEM callback
 */
/* Convert the argument to String and validate the length. Note this may raise. */
VALUE ossl_pem_passwd_value(VALUE);
/* Can be casted to pem_password_cb. If a password (String) is passed as the
 * "arbitrary data" (typically the last parameter of PEM_{read,write}_
 * functions), uses the value. If not, but a block is given, yields to it.
 * If not either, fallbacks to PEM_def_callback() which reads from stdin. */
int ossl_pem_passwd_cb(char *, int, int, void *);

/*
 * Clear BIO* with this in PEM/DER fallback scenarios to avoid decoding
 * errors piling up in OpenSSL::Errors
 */
#define OSSL_BIO_reset(bio) do { \
    (void)BIO_reset((bio)); \
    ossl_clear_error(); \
} while (0)

/*
 * ERRor messages
 */
#define OSSL_ErrMsg() ERR_reason_error_string(ERR_get_error())
NORETURN(void ossl_raise(VALUE, const char *, ...));
/* Clear OpenSSL error queue. If dOSSL is set, rb_warn() them. */
void ossl_clear_error(void);

/*
 * String to DER String
 */
extern ID ossl_s_to_der;
VALUE ossl_to_der(VALUE);
VALUE ossl_to_der_if_possible(VALUE);

/*
 * Debug
 */
extern VALUE dOSSL;

#if defined(HAVE_VA_ARGS_MACRO)
#define OSSL_Debug(...) do { \
  if (dOSSL == Qtrue) { \
    fprintf(stderr, "OSSL_DEBUG: "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, " [%s:%d]\n", __FILE__, __LINE__); \
  } \
} while (0)

#define OSSL_Warning(fmt, ...) do { \
  OSSL_Debug((fmt), ##__VA_ARGS__); \
  rb_warning((fmt), ##__VA_ARGS__); \
} while (0)

#define OSSL_Warn(fmt, ...) do { \
  OSSL_Debug((fmt), ##__VA_ARGS__); \
  rb_warn((fmt), ##__VA_ARGS__); \
} while (0)
#else
void ossl_debug(const char *, ...);
#define OSSL_Debug ossl_debug
#define OSSL_Warning rb_warning
#define OSSL_Warn rb_warn
#endif

/*
 * Include all parts
 */
#include "openssl_missing.h"
#include "ruby_missing.h"
#include "ossl_asn1.h"
#include "ossl_bio.h"
#include "ossl_bn.h"
#include "ossl_cipher.h"
#include "ossl_config.h"
#include "ossl_digest.h"
#include "ossl_hmac.h"
#include "ossl_ns_spki.h"
#include "ossl_ocsp.h"
#include "ossl_pkcs12.h"
#include "ossl_pkcs7.h"
#include "ossl_pkcs5.h"
#include "ossl_pkey.h"
#include "ossl_rand.h"
#include "ossl_ssl.h"
#include "ossl_version.h"
#include "ossl_x509.h"
#include "ossl_engine.h"

void Init_openssl(void);

#endif /* _OSSL_H_ */
