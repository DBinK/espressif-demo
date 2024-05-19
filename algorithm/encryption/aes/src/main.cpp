#include <Arduino.h>
#include <mbedtls/aes.h>
#include <aes_alt.h>

#include <cstdio>
#include <cstring>

// AES key size can be 128, 192, or 256 bits
#define AES_KEY_SIZE 128
#define AES_BLOCK_SIZE 16

void print_hex(const char *label, const unsigned char *data, int len) {
    printf("%s: ", label);
    for (int i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

void aes_cfb128_encrypt_decrypt() {
    // 密钥 cfb128 则需要 16个 U8
    unsigned char key[AES_KEY_SIZE / 8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    unsigned char iv[AES_BLOCK_SIZE] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
                                        0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF};

    unsigned char iv_copy[AES_BLOCK_SIZE]; // Copy of IV for decryption

    // Example plaintext
    unsigned char plaintext[] = "This is a test message.";
    size_t plaintext_len = strlen((char *)plaintext);

    // Buffer for ciphertext and decrypted text
    unsigned char ciphertext[128];
    unsigned char decryptedtext[128];

    // Initialize AES context
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    // Set AES encryption key
    mbedtls_aes_setkey_enc(&aes, key, AES_KEY_SIZE);

    // Encrypt the plaintext using AES CFB128
    size_t iv_offset = 0;
    memcpy(iv_copy, iv, AES_BLOCK_SIZE); // Copy IV for encryption
    mbedtls_aes_crypt_cfb128(&aes, MBEDTLS_AES_ENCRYPT, plaintext_len, &iv_offset, iv_copy, plaintext, ciphertext);

    print_hex("Ciphertext", ciphertext, plaintext_len);

    // Reset IV offset and copy IV for decryption
    iv_offset = 0;
    memcpy(iv_copy, iv, AES_BLOCK_SIZE); // Copy IV for decryption

    // Set AES decryption key (it's the same as the encryption key for AES)
    mbedtls_aes_setkey_dec(&aes, key, AES_KEY_SIZE);

    // Decrypt the ciphertext using AES CFB128
    mbedtls_aes_crypt_cfb128(&aes, MBEDTLS_AES_DECRYPT, plaintext_len, &iv_offset, iv_copy, ciphertext, decryptedtext);

    decryptedtext[plaintext_len] = '\0'; // Null-terminate the decrypted string
    printf("Decrypted text: %s\n", decryptedtext);

    // Free AES context
    mbedtls_aes_free(&aes);
}

void setup() {
    aes_cfb128_encrypt_decrypt();
}

void loop() {
    delay(10 * 1000);
}