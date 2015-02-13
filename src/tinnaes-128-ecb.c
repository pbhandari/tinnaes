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
encrypt(const uint8_t* plaintext, const uint8_t* keytext, uint8_t* cipher)
{
    uint32_t key[44];
    STR_TO_WORD_ARRAY(keytext, key);
    KEY_EXP(key);

    size_t length = strlen((const char*)plaintext);

    uint32_t plain[4];
    uint32_t temp_pt[4] = {0, 0, 0, 0};

    for (size_t i = 0; i < length; i+=16) {
        STR_TO_WORD_ARRAY((plaintext + i), plain);
        ENCRYPT_BLOCK(plain, key, temp_pt);

        WORD_TO_STR(plain[0], (cipher + i + 0));
        WORD_TO_STR(plain[1], (cipher + i + 4));
        WORD_TO_STR(plain[2], (cipher + i + 8));
        WORD_TO_STR(plain[3], (cipher + i + 12));
    }
    cipher[length] = '\0';
}


void
decrypt(const uint8_t* ciphertext, const uint8_t* keytext, uint8_t* plain)
{
    uint32_t key[44];
    STR_TO_WORD_ARRAY(keytext, key);
    KEY_EXP(key);

    size_t length = strlen((const char*)ciphertext);
    uint32_t cipher[4];
    uint32_t temp_pt[4] = {0, 0, 0, 0};

    for (size_t i = 0; i < length; i+=16) {
        STR_TO_WORD_ARRAY((ciphertext + i), cipher);
        DECRYPT_BLOCK(cipher, key, temp_pt);

        WORD_TO_STR(cipher[0], (plain + i + 0));
        WORD_TO_STR(cipher[1], (plain + i + 4));
        WORD_TO_STR(cipher[2], (plain + i + 8));
        WORD_TO_STR(cipher[3], (plain + i + 12));
    }
    plain[length] = '\0';
}
