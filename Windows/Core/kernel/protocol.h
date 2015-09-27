#ifndef _OPENNUIG3_KERNEL_PROTOCOL_H_
#define _OPENNUIG3_KERNEL_PROTOCOL_H_

#include <array>

namespace kernel {
namespace protocol {
struct stream_constant
{
    static const int life_socket_port = 8000;
    static const int opennui_packet_identifier = 0xFD42;
    static const int pipe_in_buffer_capacity = 4096;
    static const int pipe_out_buffer_capacity = 4096;
};

enum class opcode: int16_t
{
    undefined = -1,

    stc_request_platform_type = 0x0001,
    stc_request_pipe_connection = 0x0002,
    stc_request_websocket_connection = 0x0002,
    stc_test_textmessage = 0x0003,
    stc_response_color_frame = 0x0004,
    stc_response_depth_frame = 0x0005,
    stc_response_body_frame = 0x0006,
    stc_send_all_sensor_info = 0x0007,
    stc_send_new_sensor_info = 0x0008,
    stc_send_changed_sensor_info = 0x0009,
    stc_send_trigger_event_data = 0x0010,

    cts_hello_ack = 0x0001,
    cts_ready_connection_ack = 0x0002,
    cts_test_textmessage = 0x0003,
    cts_request_color_frame = 0x0004,
    cts_request_depth_frame = 0x0005,
    cts_request_body_frame = 0x0006,
    cts_request_sensor_data = 0x0007,
};

enum class platform: uint32_t
{
    win32 = 0,
    web = 1,
};

struct data_frame_header
{
    static const int header_size = 18;
    uint16_t identifier = stream_constant::opennui_packet_identifier;
    uint32_t data_size = 0;
    opcode   opcode = opcode::undefined;
    ::std::array<unsigned char, 10> padding;
    ::std::array<unsigned char, header_size> to_buffer() const;
    void to_buffer(unsigned char* dest) const;
    void from_raw_data(unsigned char* raw_data);
};

// Returns formatted packet size
uint32_t packet_formatting(unsigned char* dst, data_frame_header& header, const unsigned char* data, uint32_t data_length);
// Returns parsed data frame header
data_frame_header packet_parsing(unsigned char* raw_data, unsigned char* data_buffer);

} // namespace protocol
}

#endif // !_OPENNUIG3_KERNEL_PROTOCOL_H_