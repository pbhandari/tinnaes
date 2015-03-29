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

#include <stdint.h>
#include <string.h>

#include "tinnaes-128.h"

void
encrypt(const uint8_t* plaintext, const uint8_t* keytext, uint8_t* cipher,
        size_t length)
{
    uint32_t key[44];
    STR_TO_WORD_ARRAY(keytext, key);
    key_expansion(key);

    uint32_t plain[4];

    for (size_t i = 0; i < length; i+=16) {
        STR_TO_WORD_ARRAY((plaintext + i), plain);
        encrypt_block(plain, key);
        WORD_ARRAY_TO_STR(plain, (cipher + i));
    }
}


void
decrypt(const uint8_t* ciphertext, const uint8_t* keytext, uint8_t* plain,
        size_t length)
{
    uint32_t key[44];
    STR_TO_WORD_ARRAY(keytext, key);
    key_expansion(key);

    uint32_t cipher[4];

    for (size_t i = 0; i < length; i+=16) {
        STR_TO_WORD_ARRAY((ciphertext + i), cipher);
        decrypt_block(cipher, key);
        WORD_ARRAY_TO_STR(cipher, (plain + i));
    }
}
