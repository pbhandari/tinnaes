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

#include "tinnaes-128.h"
#include <stdint.h>

#ifdef __cplusplus
#define restrict
#endif

#define STR_TO_INT_ARRAY(str, word)                                            \
    do {                                                                       \
        word[0] = (str[0] << 24) | (str[1] << 16) | (str[2] << 8) | str[3];    \
        word[1] = (str[4] << 24) | (str[5] << 16) | (str[6] << 8) | str[7];    \
        word[2] = (str[8] << 24) | (str[9] << 16) | (str[10] << 8) | str[11];  \
        word[3] =                                                              \
            (str[12] << 24) | (str[13] << 16) | (str[14] << 8) | str[15];      \
    } while (0);

#define INT_ARRAY_TO_STR(wd, st)                                               \
    do {                                                                       \
        st[0] = wd[0] >> 24;                                                   \
        st[1] = wd[0] >> 16;                                                   \
        st[2] = wd[0] >> 8;                                                    \
        st[3] = wd[0];                                                         \
        st[4] = wd[1] >> 24;                                                   \
        st[5] = wd[1] >> 16;                                                   \
        st[6] = wd[1] >> 8;                                                    \
        st[7] = wd[1];                                                         \
        st[8] = wd[2] >> 24;                                                   \
        st[9] = wd[2] >> 16;                                                   \
        st[10] = wd[2] >> 8;                                                   \
        st[11] = wd[2];                                                        \
        st[12] = wd[3] >> 24;                                                  \
        st[13] = wd[3] >> 16;                                                  \
        st[14] = wd[3] >> 8;                                                   \
        st[15] = wd[3];                                                        \
    } while (0);

#define LSHIFT(word) ((word << 8) | (word >> 24))
#define RSHIFT(word) ((word >> 8) | (word << 24))

typedef uint8_t sbox_t[256];
extern sbox_t sbox;
extern sbox_t rev_sbox;
extern sbox_t mult2;
extern sbox_t mult9;

#if defined(NO_PRECOMPUTE_MULT2)
static uint8_t
mult_2(uint8_t a)
{
    return (a << 1) ^ (((a >> 7) & 1) * 0x1b);
}
#else
#define mult_2(x) mult2[x]
#endif

#if defined(NO_PRECOMPUTE_MULT9)
static inline uint8_t
mult_9(uint8_t a)
{
    return a ^ mult_2(mult_2(mult_2(a)));
}
#else
#define mult_9(x) mult9[x]
#endif

static inline uint8_t
mult_b(uint8_t a)
{
    return mult_9(a) ^ mult_2(a);
}

static inline uint8_t
mult_d(uint8_t a)
{
    return mult_9(a) ^ mult_2(mult_2(a));
}

static inline uint8_t
mult_e(uint8_t a)
{
    return mult_2(a) ^ mult_2(mult_2(a)) ^ mult_9(a) ^ a;
}

static inline uint32_t
sbox_u32(uint32_t idx, const sbox_t sb)
{
    return (sb[0xff & (idx >> 24)] << 24) | (sb[0xff & (idx >> 16)] << 16) |
           (sb[0xff & (idx >> 8)] << 8) | (sb[0xff & (idx)]);
}

static void
inv_mix_columns(uint32_t *state)
{
    for (uint32_t *end = state + 4; state < end; state++) {
        uint8_t st[] = {*state >> 24, *state >> 16, *state >> 8, *state};
        uint8_t tmp[] = {
            mult_e(st[0]) ^ mult_b(st[1]) ^ mult_d(st[2]) ^ mult_9(st[3]),
            mult_9(st[0]) ^ mult_e(st[1]) ^ mult_b(st[2]) ^ mult_d(st[3]),
            mult_d(st[0]) ^ mult_9(st[1]) ^ mult_e(st[2]) ^ mult_b(st[3]),
            mult_b(st[0]) ^ mult_d(st[1]) ^ mult_9(st[2]) ^ mult_e(st[3]),
        };

        *state = tmp[0] << 24 | tmp[1] << 16 | tmp[2] << 8 | tmp[3];
    }
}

static void
mix_columns(uint32_t *state)
{
    // clang-format off
    for (uint32_t *end = state + 4; state < end; state++) {
        uint8_t s[] = { *state >> 24, *state >> 16, *state >> 8, *state };
        uint8_t m[] = { mult_2(s[0]), mult_2(s[1]), mult_2(s[2]), mult_2(s[3]) };

        // mt[x] ^ st[x] == mult(st[x] , 3)
        *state = (m[0]      ^ m[1]^s[1] ^ s[2]      ^ s[3])      << 24 |
                 (s[0]      ^ m[1]      ^ m[2]^s[2] ^ s[3])      << 16 |
                 (s[0]      ^ s[1]      ^ m[2]      ^ m[3]^s[3]) << 8  |
                 (m[0]^s[0] ^ s[1]      ^ s[2]      ^ m[3]);
    }
    // clang-format on
}

void
key_expansion(const uint8_t *restrict keytext, uint32_t *restrict key)
{
    STR_TO_INT_ARRAY(keytext, key);

    uint32_t rcon = 0x01000000;
    do {
        key += 4;

        *key = sbox_u32(LSHIFT(*(key - 1)), sbox) ^ rcon ^ *(key - 4);

        *(key + 1) = *(key - 3) ^ *(key + 0);
        *(key + 2) = *(key - 2) ^ *(key + 1);
        *(key + 3) = *(key - 1) ^ *(key + 2);
    } while (rcon <<= 1);

    key += 4;

    *key = sbox_u32(LSHIFT(*(key - 1)), sbox) ^ 0x1b000000 ^ *(key - 4);

    *(key + 1) = *(key - 3) ^ *(key + 0);
    *(key + 2) = *(key - 2) ^ *(key + 1);
    *(key + 3) = *(key - 1) ^ *(key + 2);

    key += 4;

    *key = sbox_u32(LSHIFT(*(key - 1)), sbox) ^ 0x36000000 ^ *(key - 4);

    *(key + 1) = *(key - 3) ^ *(key + 0);
    *(key + 2) = *(key - 2) ^ *(key + 1);
    *(key + 3) = *(key - 1) ^ *(key + 2);
}

#define SHIFT_ROW(a, b, c, d)                                                  \
    ((a & 0xff000000) | (b & 0x00ff0000) | (c & 0x0000ff00) | (d & 0x000000ff))
void
encrypt_block(const uint8_t *restrict plain, const uint32_t *restrict key,
              uint8_t *restrict cipher)
{
    uint32_t plaintext[4];
    STR_TO_INT_ARRAY(plain, plaintext);

    const uint32_t *end = key + 36;
    do {
        uint32_t tmp[] = {
            sbox_u32(plaintext[0] ^ key[0], sbox),
            sbox_u32(plaintext[1] ^ key[1], sbox),
            sbox_u32(plaintext[2] ^ key[2], sbox),
            sbox_u32(plaintext[3] ^ key[3], sbox),
        };

        plaintext[0] = SHIFT_ROW(tmp[0], tmp[1], tmp[2], tmp[3]);
        plaintext[1] = SHIFT_ROW(tmp[1], tmp[2], tmp[3], tmp[0]);
        plaintext[2] = SHIFT_ROW(tmp[2], tmp[3], tmp[0], tmp[1]);
        plaintext[3] = SHIFT_ROW(tmp[3], tmp[0], tmp[1], tmp[2]);

        mix_columns(plaintext);
    } while ((key += 4) < end);

    uint32_t tmp[] = {
        sbox_u32(plaintext[0] ^ key[0], sbox),
        sbox_u32(plaintext[1] ^ key[1], sbox),
        sbox_u32(plaintext[2] ^ key[2], sbox),
        sbox_u32(plaintext[3] ^ key[3], sbox),
    };

    plaintext[0] = SHIFT_ROW(tmp[0], tmp[1], tmp[2], tmp[3]) ^ key[4];
    plaintext[1] = SHIFT_ROW(tmp[1], tmp[2], tmp[3], tmp[0]) ^ key[5];
    plaintext[2] = SHIFT_ROW(tmp[2], tmp[3], tmp[0], tmp[1]) ^ key[6];
    plaintext[3] = SHIFT_ROW(tmp[3], tmp[0], tmp[1], tmp[2]) ^ key[7];

    INT_ARRAY_TO_STR(plaintext, cipher);
}

void
decrypt_block(const uint8_t *restrict cipher, const uint32_t *restrict key,
              uint8_t *restrict plain)
{
    uint32_t ciphertext[4];
    STR_TO_INT_ARRAY(cipher, ciphertext);

    ciphertext[0] = ciphertext[0] ^ key[40];
    ciphertext[1] = ciphertext[1] ^ key[41];
    ciphertext[2] = ciphertext[2] ^ key[42];
    ciphertext[3] = ciphertext[3] ^ key[43];

    const uint32_t *curr = key + 36;
    do {
        uint32_t tmp[] = {
            SHIFT_ROW(ciphertext[0], ciphertext[3], ciphertext[2],
                      ciphertext[1]),
            SHIFT_ROW(ciphertext[1], ciphertext[0], ciphertext[3],
                      ciphertext[2]),
            SHIFT_ROW(ciphertext[2], ciphertext[1], ciphertext[0],
                      ciphertext[3]),
            SHIFT_ROW(ciphertext[3], ciphertext[2], ciphertext[1],
                      ciphertext[0]),
        };

        ciphertext[0] = sbox_u32(tmp[0], rev_sbox) ^ curr[0];
        ciphertext[1] = sbox_u32(tmp[1], rev_sbox) ^ curr[1];
        ciphertext[2] = sbox_u32(tmp[2], rev_sbox) ^ curr[2];
        ciphertext[3] = sbox_u32(tmp[3], rev_sbox) ^ curr[3];

        inv_mix_columns(ciphertext);
    } while ((curr -= 4) > key);

    uint32_t tmp[] = {
        SHIFT_ROW(ciphertext[0], ciphertext[3], ciphertext[2], ciphertext[1]),
        SHIFT_ROW(ciphertext[1], ciphertext[0], ciphertext[3], ciphertext[2]),
        SHIFT_ROW(ciphertext[2], ciphertext[1], ciphertext[0], ciphertext[3]),
        SHIFT_ROW(ciphertext[3], ciphertext[2], ciphertext[1], ciphertext[0]),
    };

    ciphertext[0] = sbox_u32(tmp[0], rev_sbox) ^ curr[0];
    ciphertext[1] = sbox_u32(tmp[1], rev_sbox) ^ curr[1];
    ciphertext[2] = sbox_u32(tmp[2], rev_sbox) ^ curr[2];
    ciphertext[3] = sbox_u32(tmp[3], rev_sbox) ^ curr[3];

    INT_ARRAY_TO_STR(ciphertext, plain);
}
