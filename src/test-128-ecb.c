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

#include "../src/tinnaes-128-ecb.c"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned char key[]    = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c, '\0' };
static const unsigned char plain[]  = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, '\0' };
static const unsigned char cipher[] = { 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97, '\0' };

void
test_encrypt(unsigned char *retbuf)
{
    encrypt(plain, key, retbuf);
}

void
test_decrypt(unsigned char *retbuf)
{
    decrypt(cipher, key, retbuf);
}

int
main(void)
{
    unsigned char encbuf[17];
    unsigned char decbuf[17];
    for (int i = 0; i < NITER; i++) {
        test_encrypt(encbuf);
        test_decrypt(decbuf);
    }
    for (int i = 0; i < 16; i++) { printf("%c", encbuf[i] == cipher[i] ? '.' : '!'); }
    printf("\n");

    for (int i = 0; i < 16; i++) { printf("%c", decbuf[i] == plain[i] ? '.' : '!'); }
    printf("\n");
}
