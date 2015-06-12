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

#if defined(NO_PRECOMPUTE_MULT2)
static uint8_t
mult_2(uint8_t a)
{
    return (a << 1) ^ (((a >> 7) & 1) * 0x1b);
}
#else
static const uint8_t mult2[256];

#define mult_2(x) mult2[x]
#endif

static uint8_t
mult_9(uint8_t a)
{
    return a ^ mult_2(mult_2(mult_2(a)));
}

static uint8_t
mult_b(uint8_t a)
{
    return a ^ mult_2(mult_2(mult_2(a))) ^ mult_2(a);
}

static uint8_t
mult_d(uint8_t a)
{
    return a ^ mult_2(mult_2(mult_2(a))) ^ mult_2(mult_2(a));
}

static uint8_t
mult_e(uint8_t a)
{
    return mult_2(a) ^ mult_2(mult_2(a)) ^ mult_2(mult_2(mult_2(a)));
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
        uint8_t m[] = {mult2[s[0]],    mult2[s[1]],    mult2[s[2]],   mult2[s[3]]};

        // mt[x] ^ st[x] == mult(st[x] , 3)
        state[i] = (m[0]      ^ m[1]^s[1] ^ s[2]      ^ s[3])      << 24 |
                   (s[0]      ^ m[1]      ^ m[2]^s[2] ^ s[3])      << 16 |
                   (s[0]      ^ s[1]      ^ m[2]      ^ m[3]^s[3]) << 8 |
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
    STR_TO_WORD_ARRAY(keytext, key);

    for (uint8_t i = 1, j = 4; i < 11; i++, j += 4) {
        key[j] =
            SBOX_AT(LSHIFT(key[j - 1], 1), sbox) ^ (RCON[i] << 24) ^ key[j - 4];

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

#if !defined(NO_PRECOMPUTE_MULT2)
// clang-format off
static const uint8_t mult2[] = {
    // 0   1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
    0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e,  // 0
    0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e,  // 1
    0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e,  // 2
    0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e,  // 3
    0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e,  // 4
    0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe,  // 5
    0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde,  // 6
    0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee, 0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xfe,  // 7
    0x1b, 0x19, 0x1f, 0x1d, 0x13, 0x11, 0x17, 0x15, 0x0b, 0x09, 0x0f, 0x0d, 0x03, 0x01, 0x07, 0x05,  // 8
    0x3b, 0x39, 0x3f, 0x3d, 0x33, 0x31, 0x37, 0x35, 0x2b, 0x29, 0x2f, 0x2d, 0x23, 0x21, 0x27, 0x25,  // 9
    0x5b, 0x59, 0x5f, 0x5d, 0x53, 0x51, 0x57, 0x55, 0x4b, 0x49, 0x4f, 0x4d, 0x43, 0x41, 0x47, 0x45,  // A
    0x7b, 0x79, 0x7f, 0x7d, 0x73, 0x71, 0x77, 0x75, 0x6b, 0x69, 0x6f, 0x6d, 0x63, 0x61, 0x67, 0x65,  // B
    0x9b, 0x99, 0x9f, 0x9d, 0x93, 0x91, 0x97, 0x95, 0x8b, 0x89, 0x8f, 0x8d, 0x83, 0x81, 0x87, 0x85,  // C
    0xbb, 0xb9, 0xbf, 0xbd, 0xb3, 0xb1, 0xb7, 0xb5, 0xab, 0xa9, 0xaf, 0xad, 0xa3, 0xa1, 0xa7, 0xa5,  // D
    0xdb, 0xd9, 0xdf, 0xdd, 0xd3, 0xd1, 0xd7, 0xd5, 0xcb, 0xc9, 0xcf, 0xcd, 0xc3, 0xc1, 0xc7, 0xc5,  // E
    0xfb, 0xf9, 0xff, 0xfd, 0xf3, 0xf1, 0xf7, 0xf5, 0xeb, 0xe9, 0xef, 0xed, 0xe3, 0xe1, 0xe7, 0xe5,  // F
};
// clang-format on
#endif
