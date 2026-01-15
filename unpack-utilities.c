// Utilities for unpacking files
// PackLab - CS213 - Northwestern University
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "unpack-utilities.h"

// --- public functions ---

void error_and_exit(const char* message) {
  fprintf(stderr, "%s", message);
  exit(1);
}

void* malloc_and_check(size_t size) {
  void* pointer = malloc(size);
  if (pointer == NULL) {
    error_and_exit("ERROR: malloc failed\n");
  }
  return pointer;
}


void parse_header(uint8_t* input_data, size_t input_len, packlab_config_t* config) {

  // TODO
  // Validate the header and set configurations based on it
  // Look at unpack-utilities.h to see what the fields of config are
  // Set the is_valid field of config to false if the header is invalid
  // or input_len (length of the input_data) is shorter than expected

  // Verify that the magic (Address 0-1, 0x0213) and version (Address 2, 0x03) are correct.
  uint16_t magic_verify = 0x0213;
  uint8_t version_verify = 0x03;


  uint8_t version_value = 0;

  int base = 2;

  // Magic
  uint8_t magic_1st_byte = input_data[0] << 8; ; // Access first address. Shift by 8, going from two bites to four bites with trailing zeros
  uint8_t magic_2nd_byte = input_data[1]; // Access second address
  uint16_t magic_value = magic_1st_byte + magic_2nd_byte; // Add together to make it 16-bit

  // Version
  uint8_t version_value = input_data[2];

  // Verifying Magic
  if (magic_value == magic_verify) {
      config->is_valid = true;
      printf("Magic is valid. Value is",magic_value,"matches 0x0213 or 531");

      // Now, check version
      if (version_value == version_verify) {
          config->is_valid = true;
          printf("Version is valid. Decimal",version_value,"matches 0x0213 or 531");}
      else {
          config->is_valid = false;
          printf("Version is invalid. Decimal",version_value,"do not matches 0x0213 or 531");}
      }
  else {
    config->is_valid = false;
    printf("Magic is invalid. Decimal",magic_value,"do not matches 0x0213 or 531");}    

  //  Check which options are set in Flags, set the appropriate fields in the struct, and determine how
  //  many more bytes need to be read from the header.
  config->is_compressed = input_data[3] >> 7; // Take the MSB (7 bit)
  config->is_encrypted = (input_data[3] ^ 191) >> 6; // Mask and take the 6 bit
  config->is_checksummed = (input_data[3] ^ 223) >> 5; // Mask and take the 5 bit
  config->should_continue = (input_data[3] ^ 239) >> 4; // Mask and take the 4 bit
  config->should_float = (input_data[3] ^ 247) >> 3; // Mask and take the 3 bit
  config->should_float3 = (input_data[3] ^ 251) >> 2; // Mask and take the 3 bit
  
  //   Get the length of this stream and the length of the original data.

  config->orig_data_size = ((uint64_t)input_data[11] << 56) + ((uint64_t)input_data[10] << 48) + ((uint64_t)input_data[9] << 40) + ((uint64_t)input_data[8] << 32) + ((uint64_t)input_data[7] << 24)+ ((uint64_t)input_data[6] << 16) + ((uint64_t)input_data[5] << 8) + ((uint64_t)input_data[4]); // Cast into 64-bit to avoid undefine when shifting; Left shift to create trailing zero and add up
  config->data_size = ((uint64_t)input_data[19] << 56) + ((uint64_t)input_data[18] << 48) + ((uint64_t)input_data[17] << 40) + ((uint64_t)input_data[16] << 32) + ((uint64_t)input_data[15] << 24)+ ((uint64_t)input_data[14] << 16) + ((uint64_t)input_data[13] << 8) + ((uint64_t)input_data[12]); // Cast into 64-bit to avoid undefine when shifting; Left shift to create trailing zero and add up
  
  // Pull out the compression dictionary for this stream if Compression? is enabled.
  if (config->is_compressed == true ) {

  }

  // Pull out the checksum value for this stream if Checksummed? is enabled.
    if (config->is_checksummed == true ) {

  }
}

uint16_t calculate_checksum(uint8_t* input_data, size_t input_len) {

  // TODO
  // Calculate a checksum over input_data
  // Return the checksum value

  return 0;
}

uint16_t lfsr_step(uint16_t oldstate) {

  // TODO
  // Calculate the new LFSR state given previous state
  // Return the new LFSR state

  return 0;
}

void decrypt_data(uint8_t* input_data, size_t input_len,
                  uint8_t* output_data, size_t output_len,
                  uint16_t encryption_key) {

  // TODO
  // Decrypt input_data and write result to output_data
  // Uses lfsr_step() to calculate psuedorandom numbers, initialized with encryption_key
  // Step the LFSR once before encrypting data
  // Apply psuedorandom number with an XOR in little-endian order
  // Beware: input_data may be an odd number of bytes

}

size_t decompress_data(uint8_t* input_data, size_t input_len,
                       uint8_t* output_data, size_t output_len,
                       uint8_t* dictionary_data) {

  // TODO
  // Decompress input_data and write result to output_data
  // Return the length of the decompressed data

  return 0;
}

void join_float_array(uint8_t* input_signfrac, size_t input_len_bytes_signfrac,
                      uint8_t* input_exp, size_t input_len_bytes_exp,
                      uint8_t* output_data, size_t output_len_bytes) {

  // TODO
  // Combine two streams of bytes, one with signfrac data and one with exp data,
  // into one output stream of floating point data
  // Output bytes are in little-endian order

}
/* End of mandatory implementation. */

/* Extra credit */
void join_float_array_three_stream(uint8_t* input_frac,
                                   size_t   input_len_bytes_frac,
                                   uint8_t* input_exp,
                                   size_t   input_len_bytes_exp,
                                   uint8_t* input_sign,
                                   size_t   input_len_bytes_sign,
                                   uint8_t* output_data,
                                   size_t   output_len_bytes) {

  // TODO
  // Combine three streams of bytes, one with frac data, one with exp data,
  // and one with sign data, into one output stream of floating point data
  // Output bytes are in little-endian order

}

