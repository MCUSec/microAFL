/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _CAU_API_H_
#define _CAU_API_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @addtogroup mmcau MMCAU Peripheral Driver
 * @{
 */
#define cau_aes_set_key mmcau_aes_set_key
#define cau_aes_encrypt mmcau_aes_encrypt
#define cau_aes_decrypt mmcau_aes_decrypt
#define cau_des_chk_parity mmcau_des_chk_parity
#define cau_des_encrypt mmcau_des_encrypt
#define cau_des_decrypt mmcau_des_decrypt
#define cau_md5_initialize_output mmcau_md5_initialize_output
#define cau_md5_hash_n mmcau_md5_hash_n
#define cau_md5_update mmcau_md5_update
#define cau_md5_hash mmcau_md5_hash
#define cau_sha1_initialize_output mmcau_sha1_initialize_output
#define cau_sha1_hash_n mmcau_sha1_hash_n
#define cau_sha1_update mmcau_sha1_update
#define cau_sha1_hash mmcau_sha1_hash
#define cau_sha256_initialize_output mmcau_sha256_initialize_output
#define cau_sha256_hash_n mmcau_sha256_hash_n
#define cau_sha256_update mmcau_sha256_update
#define cau_sha256_hash mmcau_sha256_hash

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief AES: Performs an AES key expansion
 *
 * This function performs an AES key expansion
 *
 * @param key Pointer to input key (128, 192, 256 bits in length).
 * @param key_size Key size in bits (128, 192, 256)
 * @param[out] key_sch Pointer to key schedule output (44, 52, 60 longwords)
 * @note All pointers must have word (4 bytes) alignment
 * @note Table below shows the requirements for the cau_aes_set_key() function when using AES128, AES192 or AES256.
 * | [in] Key Size (bits)    |  [out] Key Schedule Size (32 bit data values)  |
 * | :---------------------: | :--------------------------------------------: |
 * |          128            |                      44                        |
 * |          192            |                      52                        |
 * |          256            |                      60                        |
 */
void cau_aes_set_key(const unsigned char *key, const int key_size, unsigned char *key_sch);

/*!
 * @brief AES: Encrypts a single 16 byte block
 *
 *  This function encrypts a single 16-byte block for AES128, AES192 and AES256
 *
 * @param in Pointer to 16-byte block of input plaintext
 * @param key_sch Pointer to key schedule (44, 52, 60 longwords)
 * @param nr Number of AES rounds (10, 12, 14 = f(key_schedule))
 * @param[out] out Pointer to 16-byte block of output ciphertext
 * @note All pointers must have word (4 bytes) alignment
 * @note   Input and output blocks may overlap.@n
 *         Table below shows the requirements for the cau_aes_encrypt()/cau_aes_decrypt()
 *         function when using AES128, AES192 or AES256.
 * | Block Cipher | [in] Key Schedule Size (longwords) | [in] Number of AES rounds |
 * | :----------: | :--------------------------------: | :-----------------------: |
 * |    AES128    |               44                   |             10            |
 * |    AES192    |               52                   |             12            |
 * |    AES256    |               60                   |             14            |
 */
void cau_aes_encrypt(const unsigned char *in, const unsigned char *key_sch, const int nr, unsigned char *out);

/*!
 * @brief AES: Decrypts a single 16-byte block
 *
 * This function decrypts a single 16-byte block for AES128, AES192 and AES256
 *
 * @param in Pointer to 16-byte block of input ciphertext
 * @param key_sch Pointer to key schedule (44, 52, 60 longwords)
 * @param nr Number of AES rounds (10, 12, 14 = f(key_schedule))
 * @param[out] out Pointer to 16-byte block of output plaintext
 * @note All pointers must have word (4 bytes) alignment
 * @note   Input and output blocks may overlap.@n
 *         Table below shows the requirements for the cau_aes_encrypt()/cau_aes_decrypt()
 *         function when using AES128, AES192 or AES256.
 * | Block Cipher | [in] Key Schedule Size (longwords) | [in] Number of AES rounds |
 * | :----------: | :--------------------------------: | :-----------------------: |
 * |    AES128    |               44                   |             10            |
 * |    AES192    |               52                   |             12            |
 * |    AES256    |               60                   |             14            |
 */
void cau_aes_decrypt(const unsigned char *in, const unsigned char *key_sch, const int nr, unsigned char *out);

/*!
 * @brief DES: Checks key parity
 *
 * This function checks the parity of a DES key
 *
 * @param key 64-bit DES key with parity bits. Must have word (4 bytes) alignment.
 * @return  0 no error
 * @return -1 parity error
 */
int cau_des_chk_parity(const unsigned char *key);

/*!
 * @brief DES: Encrypts a single 8-byte block
 *
 * This function encrypts a single 8-byte block with DES algorithm.
 *
 * @param in Pointer to 8-byte block of input plaintext
 * @param key Pointer to 64-bit DES key with parity bits
 * @param[out] out Pointer to 8-byte block of output ciphertext
 * @note All pointers must have word (4 bytes) alignment
 * @note Input and output blocks may overlap.
 */
void cau_des_encrypt(const unsigned char *in, const unsigned char *key, unsigned char *out);

/*!
 * @brief DES: Decrypts a single 8-byte block
 *
 * This function decrypts a single 8-byte block with DES algorithm.
 *
 * @param in Pointer to 8-byte block of input ciphertext
 * @param key Pointer to 64-bit DES key with parity bits
 * @param[out] out Pointer to 8-byte block of output plaintext
 * @note All pointers must have word (4 bytes) alignment
 * @note  Input and output blocks may overlap.
 */
void cau_des_decrypt(const unsigned char *in, const unsigned char *key, unsigned char *out);

/*!
 * @brief MD5: Initializes the MD5 state variables
 *
 * This function initializes the MD5 state variables.
 * The output can be used as input to cau_md5_hash() and cau_md5_hash_n().
 *
 * @param[out] md5_state Pointer to 128-bit block of md5 state variables: a,b,c,d
 * @note All pointers must have word (4 bytes) alignment
 */
void cau_md5_initialize_output(const unsigned char *md5_state);

/*!
 * @brief MD5: Updates MD5 state variables with n message blocks
 *
 * This function updates MD5 state variables for one or more input message blocks
 *
 * @param msg_data Pointer to start of input message data
 * @param num_blks Number of 512-bit blocks to process
 * @param[in,out] md5_state Pointer to 128-bit block of MD5 state variables: a,b,c,d
 * @note All pointers must have word (4 bytes) alignment
 * @note  Input message and digest output blocks must not overlap.
 *        The cau_md5_initialize_output() function must be called when starting a new hash.
 *        Useful when handling non-contiguous input message blocks.
 */
void cau_md5_hash_n(const unsigned char *msg_data, const int num_blks, unsigned char *md5_state);

/*!
 * @brief MD5: Updates MD5 state variables
 *
 * This function updates MD5 state variables for one or more input message blocks.
 * It starts a new hash as it internally calls cau_md5_initialize_output() first.
 *
 * @param msg_data Pointer to start of input message data
 * @param num_blks Number of 512-bit blocks to process
 * @param[out] md5_state Pointer to 128-bit block of MD5 state variables: a,b,c,d
 * @note All pointers must have word (4 bytes) alignment
 * @note  Input message and digest output blocks must not overlap.
 *        The cau_md5_initialize_output() function is not required to be called
 *        as it is called internally to start a new hash.
 *        All input message blocks must be contiguous.
 */
void cau_md5_update(const unsigned char *msg_data, const int num_blks, unsigned char *md5_state);

/*!
 * @brief MD5: Updates MD5 state variables with one message block
 *
 * This function updates MD5 state variables for one input message block
 *
 * @param msg_data Pointer to start of 512-bits of input message data
 * @param[in,out] md5_state Pointer to 128-bit block of MD5 state variables: a,b,c,d
 * @note All pointers must have word (4 bytes) alignment
 * @note  Input message and digest output blocks must not overlap.
 *        The cau_md5_initialize_output() function must be called when starting a new hash.
 */
void cau_md5_hash(const unsigned char *msg_data, unsigned char *md5_state);

/*!
 * @brief SHA1: Initializes the SHA1 state variables
 *
 * This function initializes the SHA1 state variables.
 * The output can be used as input to cau_sha1_hash() and cau_sha1_hash_n().
 *
 * @param[out] sha1_state Pointer to 160-bit block of SHA1 state variables: a,b,c,d,e
 * @note All pointers must have word (4 bytes) alignment
 */
void cau_sha1_initialize_output(const unsigned int *sha1_state);

/*!
 * @brief SHA1: Updates SHA1 state variables with n message blocks
 *
 * This function updates SHA1 state variables for one or more input message blocks
 *
 * @param msg_data Pointer to start of input message data
 * @param num_blks Number of 512-bit blocks to process
 * @param[in,out] sha1_state Pointer to 160-bit block of SHA1 state variables: a,b,c,d,e
 * @note All pointers must have word (4 bytes) alignment
 * @note  Input message and digest output blocks must not overlap.
 *        The cau_sha1_initialize_output() function must be called when starting a new hash.
 *        Useful when handling non-contiguous input message blocks.
 */
void cau_sha1_hash_n(const unsigned char *msg_data, const int num_blks, unsigned int *sha1_state);

/*!
 * @brief SHA1: Updates SHA1 state variables
 *
 * This function updates SHA1 state variables for one or more input message blocks.
 * It starts a new hash as it internally calls cau_sha1_initialize_output() first.
 *
 * @param msg_data Pointer to start of input message data
 * @param num_blks Number of 512-bit blocks to process
 * @param[out] sha1_state Pointer to 160-bit block of SHA1 state variables: a,b,c,d,e
 * @note All pointers must have word (4 bytes) alignment
 * @note  Input message and digest output blocks must not overlap.
 *        The cau_sha1_initialize_output() function is not required to be called
 *        as it is called internally to start a new hash.
 *        All input message blocks must be contiguous.
 */
void cau_sha1_update(const unsigned char *msg_data, const int num_blks, unsigned int *sha1_state);

/*!
 * @brief SHA1: Updates SHA1 state variables with one message block
 *
 * This function updates SHA1 state variables for one input message block
 *
 * @param msg_data Pointer to start of 512-bits of input message data
 * @param[in,out] sha1_state Pointer to 160-bit block of SHA1 state variables: a,b,c,d,e
 * @note All pointers must have word (4 bytes) alignment
 * @note  Input message and digest output blocks must not overlap.
 *        The cau_sha1_initialize_output() function must be called when starting a new hash.
 */
void cau_sha1_hash(const unsigned char *msg_data, unsigned int *sha1_state);

/*!
 * @brief SHA256: Initializes the SHA256 state variables
 *
 * This function initializes the SHA256 state variables.
 * The output can be used as input to cau_sha256_hash() and cau_sha256_hash_n().
 *
 * @param[out] output Pointer to 256-bit block of SHA2 state variables a,b,c,d,e,f,g,h
 * @note All pointers must have word (4 bytes) alignment
 * @return 0 No error. CAU hardware support for SHA256 is present.
 * @return -1 Error. CAU hardware support for SHA256 is not present.
 */
int cau_sha256_initialize_output(const unsigned int *output);

/*!
 * @brief SHA256: Updates SHA256 state variables with n message blocks
 *
 * This function updates SHA256 state variables for one or more input message blocks
 *
 * @param input Pointer to start of input message data
 * @param num_blks Number of 512-bit blocks to process
 * @param[in,out] output Pointer to 256-bit block of SHA2 state variables: a,b,c,d,e,f,g,h
 * @note All pointers must have word (4 bytes) alignment
 * @note  Input message and digest output blocks must not overlap.
 *        The cau_sha256_initialize_output() function must be called when starting a new hash.
 *        Useful when handling non-contiguous input message blocks.
 */
void cau_sha256_hash_n(const unsigned char *input, const int num_blks, unsigned int *output);

/*!
 * @brief SHA256: Updates SHA256 state variables
 *
 * This function updates SHA256 state variables for one or more input message blocks.
 * It starts a new hash as it internally calls cau_sha256_initialize_output() first.
 *
 * @param input Pointer to start of input message data
 * @param num_blks Number of 512-bit blocks to process
 * @param[out] output Pointer to 256-bit block of SHA2 state variables: a,b,c,d,e,f,g,h
 * @note All pointers must have word (4 bytes) alignment
 * @note  Input message and digest output blocks must not overlap.
 *        The cau_sha256_initialize_output() function is not required to be called
 *        as it is called internally to start a new hash.
 *        All input message blocks must be contiguous.
 */
void cau_sha256_update(const unsigned char *input, const int num_blks, unsigned int *output);

/*!
 * @brief SHA256: Updates SHA256 state variables with one message block
 *
 * This function updates SHA256 state variables for one input message block
 *
 * @param input Pointer to start of 512-bits of input message data
 * @param[in,out] output Pointer to 256-bit block of SHA2 state variables: a,b,c,d,e,f,g,h
 * @note All pointers must have word (4 bytes) alignment
 * @note  Input message and digest output blocks must not overlap.
 *        The cau_sha256_initialize_output() function must be called when starting a new hash.
 */
void cau_sha256_hash(const unsigned char *input, unsigned int *output);

#if defined(__cplusplus)
}
#endif

/*! @}*/
#endif /* _CAU_API_H_ */
