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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tinnaes.c"

uint8_t**
make_state(uint8_t state[4][4])
{
    uint8_t **new_state = malloc(4*(sizeof(uint8_t*)));
    for (int i = 0; i < 4; i++) {
        uint8_t *row_state = (uint8_t*)malloc(4*(sizeof(uint8_t)));

        row_state[0] = state[i][0];
        row_state[1] = state[i][1];
        row_state[2] = state[i][2];
        row_state[3] = state[i][3];

        new_state[i] = row_state;
    }

    return new_state;
}

void
delete_state(uint8_t **state)
{
    free(state[0]);
    free(state[1]);
    free(state[2]);
    free(state[3]);
    free(state);
}


int
test_key_expansion(void)
{
    uint32_t keys[4][4] = { {0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c},
                            {0x00000000, 0x00000000, 0x00000000, 0x00000000},
                            {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
                            {0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f} };
    uint32_t expand[4][44] = {{0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c,
                              0xa0fafe17, 0x88542cb1, 0x23a33939, 0x2a6c7605,
                              0xf2c295f2, 0x7a96b943, 0x5935807a, 0x7359f67f,
                              0x3d80477d, 0x4716fe3e, 0x1e237e44, 0x6d7a883b,
                              0xef44a541, 0xa8525b7f, 0xb671253b, 0xdb0bad00,
                              0xd4d1c6f8, 0x7c839d87, 0xcaf2b8bc, 0x11f915bc,
                              0x6d88a37a, 0x110b3efd, 0xdbf98641, 0xca0093fd,
                              0x4e54f70e, 0x5f5fc9f3, 0x84a64fb2, 0x4ea6dc4f,
                              0xead27321, 0xb58dbad2, 0x312bf560, 0x7f8d292f,
                              0xac7766f3, 0x19fadc21, 0x28d12941, 0x575c006e,
                              0xd014f9a8, 0xc9ee2589, 0xe13f0cc8, 0xb6630ca6},

                            {0x00000000, 0x00000000, 0x00000000, 0x00000000,
                             0x62636363, 0x62636363, 0x62636363, 0x62636363,
                             0x9b9898c9, 0xf9fbfbaa, 0x9b9898c9, 0xf9fbfbaa,
                             0x90973450, 0x696ccffa, 0xf2f45733, 0x0b0fac99,
                             0xee06da7b, 0x876a1581, 0x759e42b2, 0x7e91ee2b,
                             0x7f2e2b88, 0xf8443e09, 0x8dda7cbb, 0xf34b9290,
                             0xec614b85, 0x1425758c, 0x99ff0937, 0x6ab49ba7,
                             0x21751787, 0x3550620b, 0xacaf6b3c, 0xc61bf09b,
                             0x0ef90333, 0x3ba96138, 0x97060a04, 0x511dfa9f,
                             0xb1d4d8e2, 0x8a7db9da, 0x1d7bb3de, 0x4c664941,
                             0xb4ef5bcb, 0x3e92e211, 0x23e951cf, 0x6f8f188e},

                            {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                             0xe8e9e9e9, 0x17161616, 0xe8e9e9e9, 0x17161616,
                             0xadaeae19, 0xbab8b80f, 0x525151e6, 0x454747f0,
                             0x090e2277, 0xb3b69a78, 0xe1e7cb9e, 0xa4a08c6e,
                             0xe16abd3e, 0x52dc2746, 0xb33becd8, 0x179b60b6,
                             0xe5baf3ce, 0xb766d488, 0x045d3850, 0x13c658e6,
                             0x71d07db3, 0xc6b6a93b, 0xc2eb916b, 0xd12dc98d,
                             0xe90d208d, 0x2fbb89b6, 0xed5018dd, 0x3c7dd150,
                             0x96337366, 0xb988fad0, 0x54d8e20d, 0x68a5335d,
                             0x8bf03f23, 0x3278c5f3, 0x66a027fe, 0x0e0514a3,
                             0xd60a3588, 0xe472f07b, 0x82d2d785, 0x8cd7c326},

                            {0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f,
                             0xd6aa74fd, 0xd2af72fa, 0xdaa678f1, 0xd6ab76fe,
                             0xb692cf0b, 0x643dbdf1, 0xbe9bc500, 0x6830b3fe,
                             0xb6ff744e, 0xd2c2c9bf, 0x6c590cbf, 0x0469bf41,
                             0x47f7f7bc, 0x95353e03, 0xf96c32bc, 0xfd058dfd,
                             0x3caaa3e8, 0xa99f9deb, 0x50f3af57, 0xadf622aa,
                             0x5e390f7d, 0xf7a69296, 0xa7553dc1, 0x0aa31f6b,
                             0x14f9701a, 0xe35fe28c, 0x440adf4d, 0x4ea9c026,
                             0x47438735, 0xa41c65b9, 0xe016baf4, 0xaebf7ad2,
                             0x549932d1, 0xf0855768, 0x1093ed9c, 0xbe2c974e,
                             0x13111d7f, 0xe3944a17, 0xf307a78b, 0x4d2b30c5}
    };

    for(int i = 0; i < 4; i++) {
        uint32_t *actual = key_exp(keys[i]);
        for (int j = 0; j < 44; j++) {
            if (expand[i][j] != actual[j]) {
                printf("key_exp(%d, %d): Using 0x%x %x %x %x\n"
                        "\t expected: 0x%x, actual: 0x%x\n",
                                i, j, keys[i][0], keys[i][1], keys[i][2],
                                keys[i][3], expand[i][j], actual[j]);
                return 1;
            }
        }
        free(actual);
    }
    return 0;
}

int
test_gf_mult(void)
{
    uint8_t a = 0x87;
    uint8_t b[7] = {0x01, 0x02, 0x03, 0x09, 0x0b, 0x0d, 0x0e};
    uint8_t expected[7] = {0x87, 0x15, 0x92, 0xd3, 0xc6, 0xf9, 0x6b};

    for (int i = 0; i < 7; i++) {
        uint8_t actual = 0;
        GF_MULT(a, b[i], actual);

        if (actual != expected[i]) {
            printf("gf_mult:(%d, %d)\n\texpected 0x%x, actual 0x%x\n", a, b[i], expected[i], actual);
            return 1;
        }

        a = 0x87;
    }

    return 0;
}

int
test_add_round_key(void)
{
    uint32_t sched[4]  = {0xf2c295f2, 0x7a96b943, 0x5935807a, 0x7359f67f};
    uint32_t expect[4] = {0xaa8f5f03, 0x61dde3ef, 0x82d24ad2, 0x6832469a};
    uint32_t state[4]  = {0x584dcaf1, 0x1b4b5aac, 0xdbe7caa8, 0x1b6bb0e5};

    ADD_ROUND_KEY(state, sched);
    for (int i = 0; i < 4; i++) {
        if (state[i] != expect[i]) {
                printf("add_round_key(%d): Using 0x%x\n"
                        "\t expected: 0x%x, actual: 0x%x\n",
                        i, sched[i], expect[i], state[i]);
            return 1;
        }
    }

    return 0;
}

int
test_sub_bytes(void)
{

    uint32_t state[4] = {0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff};
    uint32_t expect[4] = {0x638293c3, 0x1bfc33f5, 0xc4eeacea, 0x4bc12816};

    SUB_BYTES(state, SBOX);
    for (int i = 0; i < 4; i++) {
        if (state[i] != expect[i]) {
                printf("sub_bytes(%d):\n"
                        "\t expected: 0x%x, actual: 0x%x\n",
                        i, expect[i], state[i]);
            return 1;
        }
    }

    uint32_t state2[4] = {0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff};
    uint32_t expect2[4] = {0x52e39466, 0x86edd302, 0x97f962fe, 0x27c9997d};

    SUB_BYTES(state2, R_SBOX);
    for (int i = 0; i < 4; i++) {
        if (state2[i] != expect2[i]) {
                printf("sub_bytes(%d):\n"
                        "\t expected: 0x%x, actual: 0x%x\n",
                        i, expect2[i], state2[i]);
            return 1;
        }
    }

    return 0;
}

int
main(void)
{
    test_key_expansion();
    test_gf_mult();
    test_add_round_key();
    test_sub_bytes();

    return 0;
}
