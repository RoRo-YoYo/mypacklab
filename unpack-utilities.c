// Utilities for unpacking files
// PackLab - CS213 - Northwestern University
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int bit_to_decimal(){
  
  
}

void parse_header(uint8_t* input_data, size_t input_len, packlab_config_t* config) {

  // TODO
  // Validate the header and set configurations based on it
  // Look at unpack-utilities.h to see what the fields of config are
  // Set the is_valid field of config to false if the header is invalid
  // or input_len (length of the input_data) is shorter than expected

  // Verify that the magic (Address 0-1, 0x215) and version (Address 2, 0x03) are correct.
  uint32_t magic_verify = 0x0213;
  uint32_t version_verify = 0x03;

  int address_place = 0;
  for (address_place; address_place < 16; ++address_place) {
    if (input_data[address_place] != magic_verify) {
        config->is_valid = false;
        break;}
    config->is_valid = true;
  }


  for (address_place; address_place < 24; ++address_place) {
    if (input_data[address_place] != version_verify) {
        config->is_valid = false;
        break;
    }
    config->is_valid = true;
    }

  //  Check which options are set in Flags, set the appropriate fields in the struct, and determine how
  //  many more bytes need to be read from the header.
  for (address_place; address_place < 32; ++address_place) {
    int address_place = 31 - address_place; // Starts from MSB (7 bit) while still using i 
    if (address_place == 7) {
        if (input_data[address_place] == 1) {
        config->is_compressed = true;
        }
        else {
        config->is_compressed = true;
        }
    }
  }
 
  
  //   Get the length of this stream and the length of the original data.

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

