#ifndef _FLAC_DECODER_H
#define _FLAC_DECODER_H

#include <cstdint>
#include <vector>

namespace flac {

// 'fLaC'
const static uint32_t FLAC_MAGIC_NUMBER = 0x664C6143;

const static uint32_t FLAC_UINT_MASK[] = {
    0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000f, 0x0000001f,
    0x0000003f, 0x0000007f, 0x00000ff, 0x000001ff, 0x000003ff, 0x000007ff,
    0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff, 0x0001ffff,
    0x0003ffff, 0x0007ffff, 0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
    0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff, 0x1fffffff,
    0x3fffffff, 0x7fffffff, 0xffffffff};

enum FLACDecoderResult {
  FLAC_DECODER_SUCCESS = 0,
  FLAC_DECODER_NO_MORE_FRAMES = 1,
  FLAC_DECODER_ERROR_OUT_OF_DATA = 2,
  FLAC_DECODER_ERROR_BAD_MAGIC_NUMBER = 3,
  FLAC_DECODER_ERROR_SYNC_NOT_FOUND = 4,
  FLAC_DECODER_ERROR_BAD_BLOCK_SIZE_CODE = 5,
  FLAC_DECODER_ERROR_BAD_HEADER = 6,
  FLAC_DECODER_ERROR_RESERVED_CHANNEL_ASSIGNMENT = 7,
  FLAC_DECODER_ERROR_RESERVED_SUBFRAME_TYPE = 8,
  FLAC_DECODER_ERROR_BAD_FIXED_PREDICTION_ORDER = 9,
  FLAC_DECODER_ERROR_RESERVED_RESIDUAL_CODING_METHOD = 10,
  FLAC_DECODER_ERROR_BLOCK_SIZE_NOT_DIVISIBLE_RICE = 11,
};

// Coefficients for fixed linear prediction
const static std::vector<int32_t> FLAC_FIXED_COEFFICIENTS[] = {{},
                                                               {
                                                                   1,
                                                               },
                                                               {2, -1},
                                                               {3, -3, 1},
                                                               {4, -6, 4, -1}};

class FLACDecoder {

public:
  FLACDecoder(uint8_t *buffer, const std::size_t buffer_size,
              const std::size_t min_buffer_size)
      : buffer_(buffer), buffer_size_(buffer_size),
        min_buffer_size_(min_buffer_size) {}

  ~FLACDecoder() { this->free_buffers(); }

  FLACDecoderResult read_header();

  FLACDecoderResult decode_frame(int32_t *output_buffer, uint32_t *num_samples);

  void free_buffers();

  uint32_t get_sample_rate() { return this->sample_rate_; }
  uint32_t get_sample_depth() { return this->sample_depth_; }
  uint32_t get_num_channels() { return this->num_channels_; }
  uint32_t get_num_samples() { return this->num_samples_; }

  uint32_t get_output_buffer_size() {
    return this->max_block_size_ * this->num_channels_;
  }

protected:
  std::size_t fill_buffer();
  FLACDecoderResult decode_subframes(uint32_t block_size, uint32_t sample_depth,
                                     uint32_t channel_assignment);
  FLACDecoderResult decode_subframe(uint32_t block_size, uint32_t sample_depth,
                                    std::size_t block_samples_offset);
  FLACDecoderResult decode_fixed_subframe(uint32_t block_size,
                                          std::size_t block_samples_offset,
                                          uint32_t pre_order,
                                          uint32_t sample_depth);
  FLACDecoderResult decode_lpc_subframe(uint32_t block_size,
                                        std::size_t block_samples_offset,
                                        uint32_t lpc_order,
                                        uint32_t sample_depth);
  FLACDecoderResult decode_residuals(uint32_t block_size);
  void restore_linear_prediction(const std::vector<int32_t> &coefs,
                                 int32_t shift);
  uint32_t read_uint(std::size_t num_bits);
  int32_t read_sint(std::size_t num_bits);
  int64_t read_rice_sint(uint8_t param);
  std::size_t get_bytes_left() { return this->bytes_left_; }
  void align_to_byte();

private:
  uint8_t *buffer_ = nullptr;
  const std::size_t buffer_size_;
  const std::size_t min_buffer_size_;
  std::size_t buffer_index_ = 0;
  std::size_t buffer_total_read_ = 0;
  std::size_t bytes_left_ = 0;
  std::size_t bit_buffer_length_ = 0;
  uint64_t bit_buffer_ = 0;
  bool out_of_data_ = false;
  uint32_t min_block_size_ = 0;
  uint32_t max_block_size_ = 0;
  uint32_t sample_rate_ = 0;
  uint32_t num_channels_ = 0;
  uint32_t sample_depth_ = 0;
  uint32_t num_samples_ = 0;
  int32_t *block_samples_ = nullptr;
  std::vector<int32_t> block_result_;
};

} // namespace flac

#endif