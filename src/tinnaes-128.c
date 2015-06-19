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

#define STR_TO_WORD_ARRAY(str, word)                                           \
    do {                                                                       \
        word[0] = (str[0] << 24) | (str[1] << 16) | (str[2] << 8) | str[3];    \
        word[1] = (str[4] << 24) | (str[5] << 16) | (str[6] << 8) | str[7];    \
        word[2] = (str[8] << 24) | (str[9] << 16) | (str[10] << 8) | str[11];  \
        word[3] =                                                              \
            (str[12] << 24) | (str[13] << 16) | (str[14] << 8) | str[15];      \
    } while (0);

#define WORD_ARRAY_TO_STR(wd, st)                                              \
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

#define LSHIFT(word, n) ((word << (n * 8)) | (word >> ((4 - n) * 8)))
#define RSHIFT(word, n) ((word >> (n * 8)) | (word << ((4 - n) * 8)))

#define SBOX_AT(idx, sbox)                                                     \
    ((sbox[0xff & (idx >> 24)] << 24) | (sbox[0xff & (idx >> 16)] << 16) |     \
     (sbox[0xff & (idx >> 8)] << 8) | (sbox[0xff & (idx)]))


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

static inline void
add_round_key(uint32_t *restrict state, const uint32_t *restrict round_key)
{
    state[0] ^= round_key[0];
    state[1] ^= round_key[1];
    state[2] ^= round_key[2];
    state[3] ^= round_key[3];
}

static inline void
sub_bytes(uint32_t *state, const sbox_t sb)
{
    state[0] = SBOX_AT(state[0], sb);
    state[1] = SBOX_AT(state[1], sb);
    state[2] = SBOX_AT(state[2], sb);
    state[3] = SBOX_AT(state[3], sb);
}

static void
inv_mix_columns(uint32_t *state)
{
    for (int i = 0; i < 4; i++) {
        uint8_t st[] = {state[i] >> 24, state[i] >> 16, state[i] >> 8,
                        state[i]};
        uint8_t tmp[] = {
            mult_e(st[0]) ^ mult_b(st[1]) ^ mult_d(st[2]) ^ mult_9(st[3]),
            mult_9(st[0]) ^ mult_e(st[1]) ^ mult_b(st[2]) ^ mult_d(st[3]),
            mult_d(st[0]) ^ mult_9(st[1]) ^ mult_e(st[2]) ^ mult_b(st[3]),
            mult_b(st[0]) ^ mult_d(st[1]) ^ mult_9(st[2]) ^ mult_e(st[3]),
        };

        state[i] = tmp[0] << 24 | tmp[1] << 16 | tmp[2] << 8 | tmp[3];
    }
}

static void
mix_columns(uint32_t *state)
{
    // clang-format off
    for (int i = 0; i < 4; i++) {
        uint8_t s[] = {state[i] >> 24, state[i] >> 16, state[i] >> 8, state[i]};
        uint8_t m[] = {mult2[s[0]], mult2[s[1]], mult2[s[2]], mult2[s[3]]};

        // mt[x] ^ st[x] == mult(st[x] , 3)
        state[i] = (m[0]      ^ m[1]^s[1] ^ s[2]      ^ s[3])      << 24 |
                   (s[0]      ^ m[1]      ^ m[2]^s[2] ^ s[3])      << 16 |
                   (s[0]      ^ s[1]      ^ m[2]      ^ m[3]^s[3]) << 8  |
                   (m[0]^s[0] ^ s[1]      ^ s[2]      ^ m[3]);
    }
    // clang-format on
}

static void
shift_rows(uint32_t *state)
{
    // clang-format off
    uint32_t tmp[] = {
        (state[0] & 0xff000000) | (state[1] & 0x00ff0000) |
        (state[2] & 0x0000ff00) | (state[3] & 0x000000ff),

        (state[1] & 0xff000000) | (state[2] & 0x00ff0000) |
        (state[3] & 0x0000ff00) | (state[0] & 0x000000ff),

        (state[2] & 0xff000000) | (state[3] & 0x00ff0000) |
        (state[0] & 0x0000ff00) | (state[1] & 0x000000ff),

        (state[3] & 0xff000000) | (state[0] & 0x00ff0000) |
        (state[1] & 0x0000ff00) | (state[2] & 0x000000ff),
    };
    // clang-format on

    state[0] = tmp[0];
    state[1] = tmp[1];
    state[2] = tmp[2];
    state[3] = tmp[3];
}

static void
inv_shift_rows(uint32_t *state)
{
    // clang-format off
    uint32_t tmp[] = {
        (state[0] & 0xff000000) | (state[3] & 0x00ff0000) |
        (state[2] & 0x0000ff00) | (state[1] & 0x000000ff),

        (state[1] & 0xff000000) | (state[0] & 0x00ff0000) |
        (state[3] & 0x0000ff00) | (state[2] & 0x000000ff),

        (state[2] & 0xff000000) | (state[1] & 0x00ff0000) |
        (state[0] & 0x0000ff00) | (state[3] & 0x000000ff),

        (state[3] & 0xff000000) | (state[2] & 0x00ff0000) |
        (state[1] & 0x0000ff00) | (state[0] & 0x000000ff),
    };
    // clang-format on

    state[0] = tmp[0];
    state[1] = tmp[1];
    state[2] = tmp[2];
    state[3] = tmp[3];
}

void
key_expansion(const uint8_t *restrict keytext, uint32_t *restrict key)
{
    static const uint8_t rcon[] = {
        // 0   1     2     3     4     5     6     7     8     9     A
        0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36,
    };
    STR_TO_WORD_ARRAY(keytext, key);

    for (uint8_t i = 1, j = 4; i < 11; i++, j += 4) {
        key[j] =
            SBOX_AT(LSHIFT(key[j - 1], 1), sbox) ^ (rcon[i] << 24) ^ key[j - 4];

        key[j + 1] = key[j - 3] ^ key[j];
        key[j + 2] = key[j - 2] ^ key[j + 1];
        key[j + 3] = key[j - 1] ^ key[j + 2];
    }
}

void
encrypt_block(const uint8_t *restrict plain, const uint32_t *restrict key,
              uint8_t *restrict cipher)
{
    uint32_t plaintext[4];
    STR_TO_WORD_ARRAY(plain, plaintext);

    add_round_key(plaintext, &key[0]);

    for (int i = 4; i < 40; i += 4) {
        shift_rows(plaintext);

        sub_bytes(plaintext, sbox);
        mix_columns(plaintext);
        add_round_key(plaintext, &key[i]);
    }
    shift_rows(plaintext);
    sub_bytes(plaintext, sbox);
    add_round_key(plaintext, &key[40]);

    WORD_ARRAY_TO_STR(plaintext, cipher);
}

void
decrypt_block(const uint8_t *restrict cipher, const uint32_t *restrict key,
              uint8_t *restrict plain)
{
    uint32_t ciphertext[4];
    STR_TO_WORD_ARRAY(cipher, ciphertext);

    add_round_key(ciphertext, &key[40]);

    for (int i = 36; i > 0; i -= 4) {
        inv_shift_rows(ciphertext);

        sub_bytes(ciphertext, rev_sbox);
        add_round_key(ciphertext, &key[i]);
        inv_mix_columns(ciphertext);
    }
    inv_shift_rows(ciphertext);

    sub_bytes(ciphertext, rev_sbox);
    add_round_key(ciphertext, &key[0]);

    WORD_ARRAY_TO_STR(ciphertext, plain);
}
