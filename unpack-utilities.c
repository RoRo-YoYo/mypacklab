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

  // Magic
  uint16_t magic_1st_byte = input_data[0] << 8; ; // Access first address. Shift by 8, going from two bites to four bites with trailing zeros
  uint16_t magic_2nd_byte = input_data[1]; // Access second address
  uint16_t magic_value = magic_1st_byte + magic_2nd_byte; // Add together to make it 16-bit

  // Version
  uint8_t version_value = input_data[2];

  // Verifying valid length for header; To avoid out of bound access
  if (input_len >= 20 ) { // Minimal header goes from address 0 - 19, expected length is 20
    printf("Valid input data length: %ld\n", input_len);

    // Verifying Magic
    if (magic_value == magic_verify) {
        printf("Magic is valid. Value is %d matches 0x0213 or 531\n", magic_value);

        // Now, check version and configure true at last if valid
        if (version_value == version_verify) {
            config->is_valid = true;
            config->header_len = 20;// Set up default header len without conditions (address 0 - 19)
            printf("Version is valid. Decimal %d matches 0x0213 or 531. Current header_len is %ld\n", version_value, config->header_len);}
        else {
            config->is_valid = false;
            printf("Version is invalid. Decimal %d do not matches 0x0213 or 531\n", version_value);}
    }
    else {
      config->is_valid = false;
      printf("Magic is invalid. Decimal %d do not matches 0x0213 or 531\n", magic_value);}    
  }  
  
  else {
    config->is_valid = false;
    printf("Invalid input data length: %ld\n", input_len);}


  //  Check which options are set in Flags, set the appropriate fields in the struct, and determine how
  //  many more bytes need to be read from the header.
  if (config->is_valid == true) {
    config->is_compressed = input_data[3] >> 7; // Take the MSB (7 bit)
    config->is_encrypted = (input_data[3] & 64) >> 6; // Mask with 01000000 and take the 6 bit
    config->is_checksummed = (input_data[3] & 32) >> 5; // Mask with 00100000and take the 5 bit
    config->should_continue = (input_data[3] & 16) >> 4; // Mask with 00010000 and take the 4 bit
    config->should_float = (input_data[3] & 8) >> 3; // Mask with 00001000 and take the 3 bit
    config->should_float3 = (input_data[3] & 4) >> 2; // Mask with 00000100 and take the 3 bit
    
    //   Get the length of this stream and the length of the original data.
    config->orig_data_size = ((uint64_t)input_data[11] << 56) + ((uint64_t)input_data[10] << 48) + ((uint64_t)input_data[9] << 40) + ((uint64_t)input_data[8] << 32) + ((uint64_t)input_data[7] << 24)+ ((uint64_t)input_data[6] << 16) + ((uint64_t)input_data[5] << 8) + ((uint64_t)input_data[4]); // Cast into 64-bit to avoid undefine when shifting; Left shift to create trailing zero and add up
    config->data_size = ((uint64_t)input_data[19] << 56) + ((uint64_t)input_data[18] << 48) + ((uint64_t)input_data[17] << 40) + ((uint64_t)input_data[16] << 32) + ((uint64_t)input_data[15] << 24)+ ((uint64_t)input_data[14] << 16) + ((uint64_t)input_data[13] << 8) + ((uint64_t)input_data[12]); // Cast into 64-bit to avoid undefine when shifting; Left shift to create trailing zero and add up

    if (config->is_checksummed == true) { // Check if checksum is enabled alone; Pull out the checksum value for this stream if Checksummed? is enabled. 
      if (input_len >=22) {
          // Big-Endian Order
          // Sign extend first byte then shift 8 bits;
          // AND the secong byte woth 0x00FF. Concanate together
          config->checksum_value = (((uint16_t)input_data[20] << 8) | (input_data[21] & 255));
          config->header_len = 22;

        printf("Checksummed is valid. Current len is %ld. Header len is %ld \n", input_len, config->header_len);
        printf("Checksummed value is %d.\n", config->checksum_value );} // Update header len with checksum condition; (Address 0 - 22)
      else {
            config->is_valid = false;
            printf("Length is invalid. Current len is %ld; Not enough for do not compression nor checksummed\n", input_len);}
    }

    // Pull out the compression dictionary for this stream if Compression? is enabled.
    if (config->is_compressed == true) { // Put value in Address 20, 24, 28, 32 (20-35)
      if (input_len >=36) {
        for (int i = 0; i < 16 ; ++i) { // Put values by looping
          config->dictionary_data[0+i] = input_data[20+i];}

          config->header_len = 36; // Update header len with compression condition; (Address 0 - 35)
          printf("Compressed is valid. Current len is %ld. Header len is %ld \n", input_len, config->header_len);

        // Check if both checksum and compressed is enabled; 16-bits separated into two bytes (8-bits)
        if (config->is_checksummed == true) {
          if (input_len >=38) {
            // Big-Endian Order
            // Sign extend first byte then shift 8 bits;
            // AND the secong byte woth 0x00FF. Concanate together
            config->checksum_value = (((uint16_t)input_data[36] << 8) | (input_data[37] & 255));
            config->header_len = 38;
            printf("Checksummed and Compressed is valid. Current len is %ld. Header len is %ld \n", input_len, config->header_len);
            printf("Checksummed value is %d.\n", config->checksum_value );
          }
          else {
            config->is_valid = false;
            printf("Length is invalid. Current len is %ld; Not enough for both compression nor checksummed.", input_len); }
        }
        } 
        else {
          config->is_valid = false;
          printf("Length is invalid. Current len is %ld; Not enough for compression\n", input_len);}   
      }
  }    
}

uint16_t calculate_checksum(uint8_t* input_data, size_t input_len) {

  // TODO
  // Calculate a checksum over input_data
  // Return the checksum value
  uint16_t checksum = 0;

  // Figure out configuration
  // checksum is enabled, then iterate through the array and add the bytes to the checksum
  for (size_t i = 0; i < input_len; i++) {
      checksum = checksum + (input_data[i]);}

  printf("Checksummed value at checksum is %d.\n", checksum );

  return checksum;
}


uint16_t lfsr_step(uint16_t oldstate) {

  // TODO
  // Calculate the new LFSR state given previous state
  // Return the new LFSR state


  // Find the bits value by position
  int Bit_By_Position[16];
  uint16_t bit_value = oldstate;

  //uint16_t position = 15; Don't need since index already correspond to the bit position
  for (uint16_t i = 0; i < 16; i++) { // Stil works even if reach zero before the loop ends as it would be remainder of 0
    uint16_t remainder = bit_value % 2; 
    bit_value = bit_value / 2;

    Bit_By_Position[i] = remainder; // Since the first remainder is LSB, and so on, it have a little endian like structure
  }

  uint16_t new_state = oldstate >> 1; // Right shift by one bit

  // XOR the bits; Result is simply either 1 or 0
  uint16_t XOR_result = Bit_By_Position[0] ^ Bit_By_Position[6] ^ Bit_By_Position[9] ^ Bit_By_Position[13];
  XOR_result = XOR_result << 15; // Shift by 15 to make it the MSB

  // Set the MSB by the XOR_result. 
  new_state = new_state | XOR_result;

  return new_state;
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
  uint16_t new_LFSR_state = lfsr_step(encryption_key);
  int current_byte = 0;

  for (int i = 0; i < input_len; i++) {
      if (i >= output_len) { // Boundary check
        break;}

      if (current_byte == 0) {
        output_data[i] = input_data[i] ^ (new_LFSR_state & 255) ; // XOR with new_LFSR_state[7:0]. AND Mask by 000000011111111 (255)
        current_byte++;}

      else if (current_byte == 1)  {
        output_data[i] = input_data[i] ^ (new_LFSR_state >> 8) ; // XOR with new_LFSR_state[15:8] vie right shift by 8
        new_LFSR_state = lfsr_step(new_LFSR_state); // Get another new_LFSR_state
        current_byte = 0;} // Reset tracker
  } // If-else should already account for odd number of bytes;
}

size_t decompress_data(uint8_t* input_data, size_t input_len,
                       uint8_t* output_data, size_t output_len,
                       uint8_t* dictionary_data) {
  uint8_t Repeat_Count;
  uint8_t Dictionary_Index;
  size_t written_bytes = 0;

  // TODO
  // Decompress input_data and write result to output_data
  // Return the length of the decompressed data
  for (size_t i = 0; i < input_len; i++) {
    if (written_bytes >= output_len) { // Boundary check
        break;}

    if ((input_data[i] == 0x07) & (i == (input_len-1))) { // If value is an escape byte 0x0700 (1792) and it the last byte; treat it normally
        output_data[written_bytes] = input_data[i];
        written_bytes++;
    } 
    else if ((input_data[i] == 0x07)) { // Else, if value is an escape byte 
      i = i + 1; // Go the second byte within the loop; And update the i itselt to avoid duplicating
      if (input_data[i] == 0x00) { // if value is an escape byte, write it to output
        output_data[written_bytes] =  0x07;
        written_bytes++;
      }
      else { // It's a repeat output via dictionary
        Repeat_Count = (input_data[i] >> 4); // Right shift to get input_data[7:4]
        Dictionary_Index = (input_data[i] & 15); // Get input_data[3:0] via AND Mask by 00001111 (15)
        for (int j = 0; j < Repeat_Count; j++) { 
          if (written_bytes < output_len) { // Boundary check
            output_data[written_bytes] = dictionary_data[Dictionary_Index];
            written_bytes++;}
        }
      }
    }
    else {
      output_data[written_bytes] = input_data[i]; // Write to output
      written_bytes++;}
  }  

  return written_bytes;
}

void join_float_array(uint8_t* input_signfrac, size_t input_len_bytes_signfrac,
                      uint8_t* input_exp, size_t input_len_bytes_exp,
                      uint8_t* output_data, size_t output_len_bytes) {

  // TODO
  // Combine two streams of bytes, one with signfrac data and one with exp data,
  // into one output stream of floating point data
  // Output bytes are in little-endian order
  
  uint8_t sign_bit;
  int byte_tracker = 1;
  size_t output_index = 0;
  int exp_index = 0;

  for (size_t i = 0; i < input_len_bytes_signfrac ; i++) { 
    if (output_index >= output_len_bytes) { // Boundary check
      break;}  

    if (byte_tracker == 3) {

      // Clear all but the MSB 
      sign_bit = (input_signfrac[i] & 128); 

      // Take input_signfrac[i][6:0] via AND mask with 127 (01111111)
      // Take input_exp[exp_index][0] via AND mark with 1 (000000001). Then shift 7 bits to make it the MSB
      output_data[output_index] = (input_signfrac[i] & 127) + ((input_exp[exp_index] & 1) << 7);
    
      output_index++; // Move to the next byte output

      //check
      if (output_index >= output_len_bytes) {
        break;}  

      // Take input_exp[exp_index][7:1] by shifting 1 bit. Add in MSB sign bit
      output_data[output_index] = sign_bit + (input_exp[exp_index] >> 1);

      // Update 
      output_index++; // Move to the next byte output
      exp_index++;
      byte_tracker = 1; // Reset
    }
    else {
      output_data[output_index] = input_signfrac[i];
      output_index++; // Move to the next byte output
      byte_tracker++;}
  }           
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

