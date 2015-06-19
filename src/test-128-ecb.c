/*
 * Copyright (c) 2014, Prajjwal Bhandari <pbhandari@pbhandari.ca>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef NITER
#define NITER 1
#endif

#include "tinnaes-128.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/resource.h>

#ifdef USE_AES_IMPL
#include <openssl/aes.h>

void
ssl_encrypt(const uint8_t *plaintext, const uint8_t *keytext, uint8_t *cipher,
        size_t length)
{
    AES_KEY key;
    AES_set_encrypt_key(keytext, 128, &key);

    for (size_t i = 0; i < length; i += 16) {
        AES_encrypt(plaintext + i, cipher + i, &key);
    }
}

void
ssl_decrypt(const uint8_t *ciphertext, const uint8_t *keytext, uint8_t *plain,
        size_t length)
{
    AES_KEY key;
    AES_set_decrypt_key(keytext, 128, &key);

    for (size_t i = 0; i < length; i += 16) {
        AES_decrypt(ciphertext + i, plain + i, &key);
    }
}
#else
void encrypt(const uint8_t *plaintext, const uint8_t *keytext, uint8_t *cipher,
             size_t length);
void decrypt(const uint8_t *ciphertext, const uint8_t *keytext, uint8_t *plain,
             size_t length);
#endif

// clang-format off
static const unsigned char key[]    =
{ 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,};

static const unsigned char plain[]  = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10,
};
static const unsigned char cipher[] = {
    0x7d, 0xf7, 0x6b, 0x0c, 0x1a, 0xb8, 0x99, 0xb3, 0x3e, 0x42, 0xf0, 0x47, 0xb9, 0x1b, 0x54, 0x6f,
    0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
    0xf5, 0xd3, 0xd5, 0x85, 0x03, 0xb9, 0x69, 0x9d, 0xe7, 0x85, 0x89, 0x5a, 0x96, 0xfd, 0xba, 0xaf,
    0x43, 0xb1, 0xcd, 0x7f, 0x59, 0x8e, 0xce, 0x23, 0x88, 0x1b, 0x00, 0xe3, 0xed, 0x03, 0x06, 0x88,
    0x7b, 0x0c, 0x78, 0x5e, 0x27, 0xe8, 0xad, 0x3f, 0x82, 0x23, 0x20, 0x71, 0x04, 0x72, 0x5d, 0xd4,
};
// clang-format on

int
main(void)
{
    struct rusage usage;

    unsigned char encbuf[sizeof(plain)];
    unsigned char decbuf[sizeof(cipher)];
    for (int i = 0; i < NITER; i++) {
#ifndef USE_AES_IMPL
        encrypt(plain, key, encbuf, sizeof(plain));
        decrypt(cipher, key, decbuf, sizeof(cipher));
#else
        ssl_encrypt(plain, key, encbuf, sizeof(plain));
        ssl_decrypt(cipher, key, decbuf, sizeof(cipher));
#endif
    }

    for (size_t i = 0; i < sizeof(plain) - 1; i++) {
        printf("%c", encbuf[i] == cipher[i] ? '.' : '!');
    }
    printf("\n");

    for (size_t i = 0; i < sizeof(cipher) - 1; i++) {
        printf("%c", decbuf[i] == plain[i] ? '.' : '!');
    }
    printf("\n");
    getrusage(RUSAGE_SELF, &usage);

    printf("Total time: %lu.%lu seconds.\n", usage.ru_utime.tv_sec,
           usage.ru_utime.tv_usec);
}
