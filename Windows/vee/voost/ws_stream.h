#ifndef _VEE_VOOST_WS_STREAM_H_
#define _VEE_VOOST_WS_STREAM_H_
#include <cstdint>
#include <vee/voost/tcp_stream.h>
#include <vee/string.h>

namespace vee {
namespace voost {
namespace net {
namespace websocket {

class websocket_server;
class websocket_stream;

struct RFC4122_GUID
{
    // Returns Globally Unique Identifier, GUID
    inline static const char* get()
    {
        return "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    }
};

//TODO: HTTP header로 통합
struct handshake_client_request
{
    handshake_client_request();
    ~handshake_client_request();
    handshake_client_request(const handshake_client_request&);
    handshake_client_request(handshake_client_request&&);
    handshake_client_request& operator=(const handshake_client_request&);
    handshake_client_request& operator=(handshake_client_request&&);
    void print() const;
    void parse(const char* data);
    void parse(string& data);
    bool is_valid() const;
    void clear();
    string binary_pack() const;
    string request_uri;
    string host;
    string upgrade;
    string connection;
    string origin;
    string sec_websocket_key;
    string sec_websocket_protocol;
    string sec_web_socket_version;
};

struct handshake_server_response
{
    handshake_server_response();
    ~handshake_server_response();
    handshake_server_response(const handshake_server_response&);
    handshake_server_response(handshake_server_response&&);
    handshake_server_response(string& secret_key);
    handshake_server_response& operator=(const handshake_server_response&);
    handshake_server_response& operator=(handshake_server_response&&);
    void parse(const char* data);
    void parse(string& data);
    //TODO: bool is_valid() const; // 현재는 RFC6455의 예시 응답으로 비교
    void print() const;
    void clear();
    string binary_pack() const;
    string http_status;
    string upgrade;
    string connection;
    string sec_websocket_accept;
};

struct data_frame_header
{
    // analzyzed datas
    bool     fin  = true;
    bool     use_mask = false;
    uint64_t payload_len = 0;
    uint32_t payload_pos = 0;
    //x bool     use_extended16_payload = false;
    //x bool     use_extended64_payload = false;

    // data sections
    int8_t  fin_opcode = 0;
    int8_t  mask_payload_len = 0;
    int16_t extended_payload_len16 = 0;
    int64_t extended_payload_len64 = 0;
    opcode_id opcode = opcode_id::undefined;
    std::array<unsigned char, 4> masking_key;
    //std::vector<unsigned char> payload;
    void analyze(const unsigned char* raw_data, net::size_t length);
    net::size_t binary_pack(opcode_id opcode, unsigned char* out_buffer) const;
    net::size_t binary_pack_size() const;
};

class websocket_server: public net_server
{
    DISALLOW_COPY_AND_ASSIGN(websocket_server);
public:
    using tcp_server = tcp::tcp_server;
    using tcp_stream = tcp::tcp_stream;
    using socket_t = ::boost::asio::ip::tcp::socket;
    using endpoint = ::boost::asio::ip::tcp::endpoint;
    using io_service_t = ::boost::asio::io_service;
    websocket_server(unsigned short port, io_service_t& io_service = io_service_sigleton::get().io_service());
    websocket_server(websocket_server&& other);
    virtual ~websocket_server();
    virtual void close() override;
    virtual ::std::shared_ptr<net_stream> accept() throw(...) override;
    inline io_service_t& get_io_service() const { return _host_io_service; }
protected:
    bool _handshake(net_stream& raw_socket);
protected:
    ::boost::asio::io_service& _host_io_service;
    tcp_server _tcp_server;
};

class websocket_stream: public ws_stream
{
    DISALLOW_COPY_AND_ASSIGN(websocket_stream);
public:
    using tcp_stream = tcp::tcp_stream;
    using socket_t = ::boost::asio::ip::tcp::socket;
    using endpoint = ::boost::asio::ip::tcp::endpoint;
    using io_service_t = ::boost::asio::io_service;
    websocket_stream();
    explicit websocket_stream(tcp_stream&& stream);
    virtual ~websocket_stream();
    websocket_stream& operator=(websocket_stream&& rhs);
    virtual void connect(const char* ip_addr, port_t port) throw(...) override;
    virtual void disconnect() override;
    //! Read and Write operations require additional space to accommodate the Websocket data frame header in the data buffer.
    virtual io_result write(opcode_id opcode, const byte* data, net::size_t len) throw(...) override;
    virtual io_result read_payload_only(byte* const buffer, net::size_t buf_capacity) throw(...) override;
    virtual io_result read_all(byte* const buffer, net::size_t buf_capacity) throw(...) override;
    inline io_service_t& get_io_service() const
    {
        return *_host_io_service_ptr;
    }
    void conversion(tcp_stream&& stream);
protected:
    io_service_t* _host_io_service_ptr;
    tcp_stream _tcp_stream;
};

} // namespace ws
} // namespace net
} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_WS_STREAM_H_