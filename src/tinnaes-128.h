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

static const uint8_t SBOX[256] = {
  // 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,  // 0
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,  // 1
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,  // 2
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,  // 3
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,  // 4
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,  // 5
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,  // 6
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,  // 7
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,  // 8
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,  // 9
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,  // A
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,  // B
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,  // C
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,  // D
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,  // E
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16   // F
};

static const uint8_t R_SBOX[256] = {
  // 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,  // 0
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,  // 1
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,  // 2
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,  // 3
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,  // 4
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,  // 5
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,  // 6
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,  // 7
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,  // 8
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,  // 9
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,  // A
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,  // B
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,  // C
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,  // D
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,  // E
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d   // F
};

static const uint8_t RCON[255] = {
  // 0     1     2     3     4     5     6     7     8     9     A
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36,
};

#define LSHIFT(word, n) ((word << (n*8)) | (word >> ((4-n) * 8)))
#define RSHIFT(word, n) ((word >> (n*8)) | (word << ((4-n) * 8)))

#define SBOX_AT(idx, sbox)                                                   \
    ((sbox[0xff & (idx >> 24)] << 24) | (sbox[0xff & (idx >> 16)] << 16) |   \
     (sbox[0xff & (idx >>  8)] <<  8) | (sbox[0xff & (idx      )]      ))


#define ADD_ROUND_KEY(state, round_key)                                      \
do {                                                                         \
    state[0] ^= round_key[0];                                                \
    state[1] ^= round_key[1];                                                \
    state[2] ^= round_key[2];                                                \
    state[3] ^= round_key[3];                                                \
} while(0);                                                                  \


#define SUB_BYTES(state, sbox)                                               \
do {                                                                         \
    state[0] = SBOX_AT(state[0], sbox);                                      \
    state[1] = SBOX_AT(state[1], sbox);                                      \
    state[2] = SBOX_AT(state[2], sbox);                                      \
    state[3] = SBOX_AT(state[3], sbox);                                      \
} while(0);                                                                  \


#define KEY_EXP(round_key)                                                   \
for(int i = 1, j = 4; i < 11; i++) {                                         \
    uint32_t k = SBOX_AT(LSHIFT(round_key[j-1], 1), SBOX) ^ (RCON[i] << 24); \
    round_key[j] = round_key[j - 4] ^ k;                j++;                 \
    round_key[j] = round_key[j - 4] ^ round_key[j - 1]; j++;                 \
    round_key[j] = round_key[j - 4] ^ round_key[j - 1]; j++;                 \
    round_key[j] = round_key[j - 4] ^ round_key[j - 1]; j++;                 \
}                                                                            \


#define MIX_COLUMNS(state, a, b, c, d)                                       \
for (int i = 0; i < 4; i++) {                                                \
    uint8_t st[] = { state[i]>>24, state[i]>>16, state[i]>>8, state[i] };    \
                    /* byte one*/                                            \
    state[i] = ((gf_mult(st[0], a) ^ gf_mult(st[1], b)                       \
               ^ gf_mult(st[2], c) ^ gf_mult(st[3], d)) & 0xff) << 24        \
                    /* byte two */                                           \
             | ((gf_mult(st[0], d) ^ gf_mult(st[1], a)                       \
               ^ gf_mult(st[2], b) ^ gf_mult(st[3], c)) & 0xff) << 16        \
                    /* byte three */                                         \
             | ((gf_mult(st[0], c) ^ gf_mult(st[1], d)                       \
               ^ gf_mult(st[2], a) ^ gf_mult(st[3], b)) & 0xff) << 8         \
                    /* byte four */                                          \
             | ((gf_mult(st[0], b) ^ gf_mult(st[1], c)                       \
               ^ gf_mult(st[2], d) ^ gf_mult(st[3], a)) & 0xff);             \
}


#define SHIFT_ROWS(state, new_state)                                         \
do {                                                                         \
    new_state[0] =  state[0] & 0xff000000 | state[1] & 0x00ff0000            \
                  | state[2] & 0x0000ff00 | state[3] & 0x000000ff;           \
                                                                             \
    new_state[1] =  state[1] & 0xff000000 | state[2] & 0x00ff0000            \
                  | state[3] & 0x0000ff00 | state[0] & 0x000000ff;           \
                                                                             \
    new_state[2] =  state[2] & 0xff000000 | state[3] & 0x00ff0000            \
                  | state[0] & 0x0000ff00 | state[1] & 0x000000ff;           \
                                                                             \
    new_state[3] =  state[3] & 0xff000000 | state[0] & 0x00ff0000            \
                  | state[1] & 0x0000ff00 | state[2] & 0x000000ff;           \
                                                                             \
} while(0);                                                                  \


#define INV_SHIFT_ROWS(state, new_state)                                     \
do {                                                                         \
    new_state[0] =  state[0] & 0xff000000 | state[3] & 0x00ff0000            \
                  | state[2] & 0x0000ff00 | state[1] & 0x000000ff;           \
                                                                             \
    new_state[1] =  state[1] & 0xff000000 | state[0] & 0x00ff0000            \
                  | state[3] & 0x0000ff00 | state[2] & 0x000000ff;           \
                                                                             \
    new_state[2] =  state[2] & 0xff000000 | state[1] & 0x00ff0000            \
                  | state[0] & 0x0000ff00 | state[3] & 0x000000ff;           \
                                                                             \
    new_state[3] =  state[3] & 0xff000000 | state[2] & 0x00ff0000            \
                  | state[1] & 0x0000ff00 | state[0] & 0x000000ff;           \
} while(0);                                                                  \


#define STR_TO_WORD_ARRAY(str, word)                                         \
do {                                                                         \
    word[0] = str[0]  << 24 | str[1]  << 16 | str[2]  << 8 | str[3];         \
    word[1] = str[4]  << 24 | str[5]  << 16 | str[6]  << 8 | str[7];         \
    word[2] = str[8]  << 24 | str[9]  << 16 | str[10] << 8 | str[11];        \
    word[3] = str[12] << 24 | str[13] << 16 | str[14] << 8 | str[15];        \
} while(0);                                                                  \


#define WORD_TO_STR(word, str)                                               \
do {                                                                         \
    str[0] = (unsigned char)(word >> 24);                                    \
    str[1] = (unsigned char)(word >> 16);                                    \
    str[2] = (unsigned char)(word >> 8);                                     \
    str[3] = (unsigned char)(word);                                          \
} while(0);


#define ENCRYPT_BLOCK(plaintext, key, temp_pt)                               \
do {                                                                         \
    ADD_ROUND_KEY(plaintext, key);                                           \
                                                                             \
    for(int i = 4; i < 40; i+=4) {                                           \
        SHIFT_ROWS(plaintext, temp_pt);                                      \
        memcpy(plaintext, temp_pt, sizeof(temp_pt));                         \
                                                                             \
        SUB_BYTES(plaintext, SBOX);                                          \
        MIX_COLUMNS(plaintext, 2, 3, 1, 1);                                  \
        ADD_ROUND_KEY(plaintext, (key + i));                                 \
    }                                                                        \
    SHIFT_ROWS(plaintext, temp_pt);                                          \
    memcpy(plaintext, temp_pt, sizeof(temp_pt));                             \
                                                                             \
    SUB_BYTES(plaintext, SBOX);                                              \
    ADD_ROUND_KEY(plaintext, (key + 40));                                    \
} while(0);                                                                  \


#define DECRYPT_BLOCK(ciphertext, key, temp_pt)                              \
do {                                                                         \
    ADD_ROUND_KEY(ciphertext, (key + 40));                                   \
                                                                             \
    for(int i = 36; i > 0; i-=4) {                                           \
        INV_SHIFT_ROWS(ciphertext, temp_pt);                                 \
        memcpy(ciphertext, temp_pt, sizeof(temp_pt));                        \
                                                                             \
        SUB_BYTES(ciphertext, R_SBOX);                                       \
        ADD_ROUND_KEY(ciphertext, (key + i));                                \
        MIX_COLUMNS(ciphertext, 0x0e, 0x0b, 0x0d, 0x09);                     \
    }                                                                        \
    INV_SHIFT_ROWS(ciphertext, temp_pt);                                     \
    memcpy(ciphertext, temp_pt, sizeof(temp_pt));                            \
                                                                             \
    SUB_BYTES(ciphertext, R_SBOX);                                           \
    ADD_ROUND_KEY(ciphertext, key);                                          \
} while(0);                                                                  \


static
unsigned char
gf_mult(unsigned char a, unsigned char b) {
    unsigned char p = 0;
    do {
        p ^= (a * (b & 1));
        a = ((a << 1) ^ (0x1b * ((a >> 7) & 1)));
    } while (b >>= 1);
    return p;
}
