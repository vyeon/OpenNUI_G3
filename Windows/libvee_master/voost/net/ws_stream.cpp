#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <vee/voost/ws_stream.h>
#include <vee/voost/string.h>
#include <boost/tokenizer.hpp>

namespace vee {
namespace voost {
namespace net {
namespace websocket {

namespace /* unnamed */ {
inline ::boost::asio::ip::address string_to_ipaddr(const char* str)
{
    return ::boost::asio::ip::address::from_string(str);
}
} // unnamed namespace

handshake_client_request::handshake_client_request():
request_uri(""),
host(""),
upgrade(""),
connection(""),
origin(""),
sec_websocket_key(""),
sec_websocket_protocol(""),
sec_web_socket_version("")
{

}

handshake_client_request::~handshake_client_request()
{

}

handshake_client_request::handshake_client_request(const handshake_client_request& other):
request_uri(other.request_uri),
host(other.host),
upgrade(other.upgrade),
connection(other.connection),
origin(other.origin),
sec_websocket_key(other.sec_websocket_key),
sec_websocket_protocol(other.sec_websocket_protocol),
sec_web_socket_version(other.sec_web_socket_version)
{

}

handshake_client_request::handshake_client_request(handshake_client_request&& other):
request_uri(static_cast<string&&>(other.request_uri)),
host(static_cast<string&&>(other.host)),
upgrade(static_cast<string&&>(other.upgrade)),
connection(static_cast<string&&>(other.connection)),
origin(static_cast<string&&>(other.origin)),
sec_websocket_key(static_cast<string&&>(other.sec_websocket_key)),
sec_websocket_protocol(static_cast<string&&>(other.sec_websocket_protocol)),
sec_web_socket_version(static_cast<string&&>(other.sec_web_socket_version))
{

}

handshake_client_request& handshake_client_request::operator=(const handshake_client_request& rhs)
{
    request_uri = rhs.request_uri;
    host = rhs.host;
    upgrade = rhs.upgrade;
    connection = rhs.connection;
    origin = rhs.origin;
    sec_websocket_key = rhs.sec_websocket_key;
    sec_websocket_protocol = rhs.sec_websocket_protocol;
    sec_web_socket_version = rhs.sec_web_socket_version;
    return *this;
}

handshake_client_request& handshake_client_request::operator=(handshake_client_request&& rhs)
{
    request_uri = static_cast<string&&>(rhs.request_uri);
    host = static_cast<string&&>(rhs.host);
    upgrade = static_cast<string&&>(rhs.upgrade);
    connection = static_cast<string&&>(rhs.connection);
    origin = static_cast<string&&>(rhs.origin);
    sec_websocket_key = static_cast<string&&>(rhs.sec_websocket_key);
    sec_websocket_protocol = static_cast<string&&>(rhs.sec_websocket_protocol);
    sec_web_socket_version = static_cast<string&&>(rhs.sec_web_socket_version);
    return *this;
}

void handshake_client_request::print() const
{
    printf("%s\nHost: %s\nUpgrade: %s\nConnection: %s\nOrigin: %s\nSec-Websocket-Key: %s\nSec-Websocket-Protocol: %s\nSec-Websocket-Version: %s\n",
           request_uri.data(),
           host.data(),
           upgrade.data(),
           connection.data(),
           origin.data(),
           sec_websocket_key.data(),
           sec_websocket_protocol.data(),
           sec_web_socket_version.data()
);
}

void handshake_client_request::parse(const char* data)
{
    return parse(make_string(data));
}

void handshake_client_request::parse(string& data)
{
    using char_separator = ::boost::char_separator<char>;
    using tokenizer = boost::tokenizer<char_separator>;

    // Parse raw data per lines (token: \n)
    ::std::vector<string> data_by_lines;
    {
        char_separator sep("\r\n");
        tokenizer tok(data, sep);
        for (auto& it : tok)
        {
            data_by_lines.push_back(it);
        }
    }

    auto get_value = [](const string& dst)-> std::pair<bool, string>
    {
        auto pos = dst.find(':');
        if (pos == string::npos)
        {
            return std::make_pair(false, "");
        }
        else
        {
            string buffer(dst.substr(pos + 1));
            string result = trim(buffer);
            return std::make_pair(true, std::move(result));
        }
    };

    /*printf("DATA RECIEVED --------------\n");
    for (auto& it : data_by_lines)
    {
    printf("%s\n", it.data());
    }*/

    for (auto& it : data_by_lines)
    {
        auto set_value = [](::std::pair<bool, string>& result, string& dst) -> void
        {
            if (result.first == true)
            {
                dst = std::move(result.second);
            }
        };

        if (strstr(it.data(), "GET"))
        {
            request_uri = it;
        }
        else if (strstr(it.data(), "Host:"))
        {
            set_value(get_value(it), host);
        }
        else if (strstr(it.data(), "Upgrade:"))
        {
            set_value(get_value(it), upgrade);
        }
        else if (strstr(it.data(), "Connection:"))
        {
            set_value(get_value(it), connection);
        }
        else if (strstr(it.data(), "Sec-WebSocket-Key:"))
        {
            set_value(get_value(it), sec_websocket_key);
        }
        else if (strstr(it.data(), "Origin:"))
        {
            set_value(get_value(it), origin);
        }
        else if (strstr(it.data(), "Sec-WebSocket-Protocol:"))
        {
            set_value(get_value(it), sec_websocket_protocol);
        }
        else if (strstr(it.data(), "Sec-WebSocket-Version:"))
        {
            set_value(get_value(it), sec_web_socket_version);
        }
        else
        {
            continue;
        }
    }
}

bool handshake_client_request::is_valid() const
{
    //! 다음이 생략됨
    //! HOST 헤더 유효성 검사
    //! Sec-Websocket-Key 헤더 유효성 검사
    //! Origin 헤더 유효성 검사
    if (
       (strtool::find_case_insensitive(request_uri.data(), "GET") == strtool::not_found) ||
       (strtool::find_case_insensitive(request_uri.data(), "HTTP/1.1") == strtool::not_found) ||
       (strtool::find_case_insensitive(upgrade.data(), "Websocket") == strtool::not_found) ||
       (strtool::find_case_insensitive(connection.data(), "Upgrade") == strtool::not_found) ||
       (strtool::find_case_insensitive(sec_web_socket_version.data(), "13") == strtool::not_found)
       )
    {
        return false;
    }
    return true;
}

void handshake_client_request::clear()
{
    request_uri.clear();
    host.clear();
    upgrade.clear();
    connection.clear();
    origin.clear();
    sec_websocket_key.clear();
    sec_web_socket_version.clear();
    sec_websocket_protocol.clear();
}

handshake_server_response::handshake_server_response():
http_status("HTTP/1.1 101 Switching Protocols"),
upgrade("websocket"),
connection("Upgrade"),
sec_websocket_accept("")
{

}

handshake_server_response::handshake_server_response(const handshake_server_response& other):
http_status(other.http_status),
upgrade(other.upgrade),
connection(other.connection),
sec_websocket_accept(other.sec_websocket_accept)
{

}

handshake_server_response::handshake_server_response(handshake_server_response&& other):
http_status(static_cast<string&&>(other.http_status)),
upgrade(static_cast<string&&>(other.upgrade)),
connection(static_cast<string&&>(other.connection)),
sec_websocket_accept(static_cast<string&&>(other.sec_websocket_accept))
{

}

handshake_server_response::handshake_server_response(string& secret_key):
http_status("HTTP/1.1 101 Switching Protocols"),
upgrade("websocket"),
connection("Upgrade"),
sec_websocket_accept(secret_key)
{

}

handshake_server_response::~handshake_server_response()
{

}

handshake_server_response& handshake_server_response::operator=(const handshake_server_response& rhs)
{
    http_status = rhs.http_status;
    upgrade = rhs.upgrade;
    connection = rhs.connection;
    sec_websocket_accept = rhs.sec_websocket_accept;
    return *this;
}

handshake_server_response& handshake_server_response::operator=(handshake_server_response&& rhs)
{
    http_status = std::move(rhs.http_status);
    upgrade = std::move(rhs.upgrade);
    connection = std::move(rhs.connection);
    sec_websocket_accept = std::move(rhs.sec_websocket_accept);
    return *this;
}

void handshake_server_response::print() const
{
    printf("%s\nUpgrade: %s\nConnection: %s\nSec-Websocket-Accept: %s\n", http_status.data(), upgrade.data(), connection.data(), sec_websocket_accept.data());
}

void handshake_server_response::clear()
{
    http_status = "HTTP/1.1 101 Switching Protocols";
    upgrade = "websocket";
    connection = "Upgrade";
    sec_websocket_accept.clear();
}

string handshake_server_response::binary_pack() const
{
    string ret;
    //ret += "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nUpgrade: websocket\r\nSec-WebSocket-Origin: http://jjoriping.ufree.kr\r\nSec-WebSocket-Location: ws://localhost:8001/\r\nSec-WebSocket-Accept: ";
    ret.append(http_status);
    ret.append("\r\nUpgrade: ");
    ret.append(upgrade);
    ret.append("\r\nConnection: ");
    ret.append(connection);
    ret.append("\r\nSec-WebSocket-Accept: ");
    ret.append(sec_websocket_accept);
    ret.append("\r\n\r\n");
    return ret;
}

void data_frame_header::analyze(const unsigned char* raw_data, net::size_t length)
{
    if (!raw_data) return;

    unsigned int shift = 0;
    // analyze a first byte
    memmove(&fin_opcode, raw_data + shift++, 1);
    fin = ((fin_opcode & 0x80) >> 7) ? true : false;
    switch ((fin_opcode & 0x0f))
    {
    case 0x0:
        opcode = opcode_t::continuation_frame;
        break;
    case 0x1:
        opcode = opcode_t::text_frame;
        break;
    case 0x2:
        opcode = opcode_t::binary_frame;
        break;
    case 0x8:
        opcode = opcode_t::connnection_close;
        break;
    case 0x9:
        opcode = opcode_t::ping;
        break;
    case 0xA:
        opcode = opcode_t::pong;
        break;
    default:
        opcode = opcode_t::reserved_for_further;
        break;
    }
    //TODO: RSV1~3 (Extension flag) processing 

    // analyze a second byte
    memmove(&mask_payload_len, raw_data + shift++, 1);
    use_mask = ((mask_payload_len & 0x80) >> 7) ? true : false;
    payload_len = (mask_payload_len & 0x7f);
    if (payload_len == 0x7e)
    {
        memmove(&extended_payload_len16, raw_data + shift, 2);
        payload_len = extended_payload_len16;
        shift += 2;
        //x use_extended16_payload = true;
    }
    else if (payload_len == 0x7f)
    {
        memmove(&extended_payload_len64, raw_data + shift, 8);
        payload_len = extended_payload_len64;
        shift += 8;
        //x use_extended64_payload = true;
    }
    else
    {
        //x use_extended16_payload = false;
        //x use_extended64_payload = false;
    }

    // Getting a mask key
    if (use_mask == true)
    {
        //x ::std::array<unsigned char, 4> mask_key_buffer;
        //memmove(mask_key_buffer.data(), raw_data + shift, 4);
        //x std::reverse_copy(mask_key_buffer.begin(), mask_key_buffer.end(), masking_key.begin());
        memmove(masking_key.data(), raw_data + shift, 4);
        shift += 4;
    }

    payload_pos = shift;

    //x Getting a payload data
    //x payload.clear();
    //x payload.resize(length - shift);
    //x memmove(&payload[0], raw_data + shift, length - shift);

    return;
}

net::size_t data_frame_header::binary_pack(unsigned char* out_buffer) const
{
    net::size_t shift = 0;

    int8_t fin_opcode_block = (int8_t)0x1; // must be text frame (net_stream interface requirement)
    (fin) ? (fin_opcode_block |= 0x80) : (fin_opcode_block |= 0x00);
    memmove(out_buffer + shift++, &fin_opcode_block, 1);

    int8_t mask_payload_len_block = 0;
    if (payload_len >= 0x7e)
    {
        if (payload_len > INT16_MAX)
            mask_payload_len_block = 0x7e;
        else
            mask_payload_len_block = 0x7f;
    }
    else
        mask_payload_len_block = (int8_t)payload_len;
    (use_mask) ? (mask_payload_len_block |= 0x80) : (mask_payload_len_block |= 0x00);
    memmove(out_buffer + shift++, &mask_payload_len_block, 1);

    if ((mask_payload_len_block & (~0x80)) == 0x7e)
    {
        int16_t extended_payload_len_16 = (int16_t)payload_len;
        memmove(out_buffer + shift, &extended_payload_len_16, 2);
        shift += 2;
    }
    else if ((mask_payload_len_block & (~0x80)) == 0x7f)
    {
        int64_t extended_payload_len_64 = (int64_t)payload_len;
        memmove(out_buffer + shift, &extended_payload_len_64, 8);
        shift += 8;
    }

    if (use_mask)
    {
        memmove(out_buffer + shift, masking_key.data(), 4);
        shift += 4;
    }

    return shift; // same as length of header
}

net::size_t data_frame_header::binary_pack_size() const
{
    // FOLLOWING PROCESS as same as binary_pack() method, but This function doesn't call memmove()
    net::size_t shift = 0;

    int8_t fin_opcode_block = (int8_t)0x1; // must be binary frame (net_stream interface requirement)
    (fin) ? (fin_opcode_block |= 0x80) : (fin_opcode_block |= 0x00);
    //x memmove(out_buffer + shift++, &fin_opcode_block, 1);
    shift += 1;

    int8_t mask_payload_len_block = 0;
    if (payload_len >= 0x7e)
    {
        if (payload_len > INT16_MAX)
            mask_payload_len_block = 0x7e;
        else
            mask_payload_len_block = 0x7f;
    }
    else
        mask_payload_len_block = (int8_t)payload_len;
    (use_mask) ? (mask_payload_len_block |= 0x80) : (mask_payload_len_block |= 0x00);
    //x memmove(out_buffer + shift++, &mask_payload_len_block, 1);
    shift += 1;

    if ((mask_payload_len_block & (~0x80)) == 0x7e)
    {
        int16_t extended_payload_len_16 = (int16_t)payload_len;
        //x memmove(out_buffer + shift, &extended_payload_len_16, 2);
        shift += 2;
    }
    else if ((mask_payload_len_block & (~0x80)) == 0x7f)
    {
        int64_t extended_payload_len_64 = (int64_t)payload_len;
        //x memmove(out_buffer + shift, &extended_payload_len_64, 8);
        shift += 8;
    }

    if (use_mask)
    {
        //x memmove(out_buffer + shift, masking_key.data(), 4);
        shift += 4;
    }

    return shift; // same as length of header
}

websocket_server::websocket_server(unsigned short port, io_service_t& io_service /* = io_service_sigleton::get().io_service() */):
_host_io_service(io_service),
_tcp_server(port, io_service)
{
    
}

websocket_server::websocket_server(websocket_server&& other):
_host_io_service(other._host_io_service),
_tcp_server(static_cast<websocket_server::tcp_server&&>(other._tcp_server))
{

}

websocket_server::~websocket_server()
{

}

void websocket_server::close()
{

}

::std::shared_ptr<net_stream> websocket_server::accept() throw(...)
{
    ::std::shared_ptr<tcp_stream> raw_tcp_stream = ::std::static_pointer_cast<tcp_stream>(_tcp_server.accept());
    bool handshake_result = _handshake(*raw_tcp_stream);
    if (handshake_result == true)
    {
        ::std::shared_ptr<net_stream> stream = ::std::make_shared<websocket_stream>( static_cast<tcp_stream&&>(*raw_tcp_stream) );
        return stream;
    }
    else
    {
        ::std::shared_ptr<net_stream> stream(nullptr);
        return stream;
    }
}

bool websocket_server::_handshake(net_stream& raw_socket)
{
    try
    {
        ::std::array<char, 4096> buffer;
        raw_socket.read(buffer.data(), buffer.size());
        
        // Parsing HTTP header
        handshake_client_request header;
        header.parse(make_string(buffer.data()));
        
        // Validating the client header
        if (header.is_valid() == false)
        {
            return false;
        }

        /*{
            printf("RFC6455 HANDSHAKE HASHING + ENCODING TEST PROCESS ----------------\n");
            string key("dGhlIHNhbXBsZSBub25jZQ==");
            printf("key: %s\n", key.data());
            key.append(RFC4122_GUID::get());
            printf("append guid: %s\n", key.data());
            auto hashing = sha1_hashing(key);
            print_hash_code(hashing);
            auto base64_encoded = base64::encode(hashing);
            printf("Base64 encoded: %s\n\n", base64_encoded.data());
            }*/
        
        printf("websocket_server> [RFC6455 Handshake] Websocket client request\n");
        PRINT_LINE;
        header.print();
        PRINT_LINE;

        // Append magic GUID
        string magic_string(header.sec_websocket_key);
        magic_string.append(RFC4122_GUID::get());
        // Hashing via SHA-1
        auto hash_code = sha1_hashing(magic_string);
        //x print_hash_code(hash_code);
        // Encoding via Base64
        string hash_code_base64 = base64::encode(hash_code);
        //x printf("Base64: %s\n", hash_code_base64.data());
        // Make data for response
        handshake_server_response response(hash_code_base64);
        string response_data = response.binary_pack();

        printf("websocket_server> [RFC6455 Handshake] Websocket server response\n");
        PRINT_LINE;
        puts(response_data.data());
        PRINT_LINE;
        // Send response data to client
        raw_socket.write((void*)response_data.data(), response_data.size() + 1);
    }
    catch (vee::exception& e)
    {
        printf("websocket_server> exception occured at _handshake() %s\n", e.what());
        return false;
    }
    return true;
}

websocket_stream::websocket_stream():
_tcp_stream(io_service_sigleton::get().io_service()),
_host_io_service_ptr(&(_tcp_stream.get_io_service()))
{

}

websocket_stream::websocket_stream(tcp_stream&& stream):
_tcp_stream(static_cast<tcp_stream&&>(stream)),
_host_io_service_ptr(&(_tcp_stream.get_io_service()))
{

}

websocket_stream::~websocket_stream()
{

}

websocket_stream& websocket_stream::operator=(websocket_stream&& rhs)
{
    _tcp_stream = static_cast<tcp_stream&&>(rhs._tcp_stream);
    _host_io_service_ptr = &(_tcp_stream.get_io_service());
    return *this;
}

void websocket_stream::connect(const char* ip_addr, port_t port) throw(...)
{
    //nothing to do.
}

void websocket_stream::disconnect()
{
    _tcp_stream.disconnect();
}

net::size_t websocket_stream::write(void* data, net::size_t len) throw(...)
{
    data_frame_header header;
    header.fin = true; //TODO: 쪼개서 보내는 함수 지원하기, 지금은 무조건 한번에 다! 보낸다! 스펙상 INT64_MAX만큼 한번에 보낼 수 있지만 브라우저가 뻗을 듯
    header.use_mask = false; //TODO: 커스텀 마스크 사용하는 함수 지원하기, 지금은 고정된 마스크
    /*header.masking_key[0] = 0x37;
    header.masking_key[1] = 0xfa;
    header.masking_key[2] = 0x21;
    header.masking_key[3] = 0x3d;*/
    header.payload_len = len;
    net::size_t header_size = header.binary_pack_size();
    std::vector<unsigned char> packet((uint32_t)(len + header_size), 0); //TODO: 매번 벡터를 안만드는 방법을 생각해보자.
    //header.binary_pack(packet.data());
    /*for (net::size_t i = 0; i < len; ++i)
    {
        unsigned char mask_it = *((unsigned char*)data + i);
        mask_it ^= header.masking_key[i%4];
        memmove(packet.data() + header_size + i, &mask_it, 1);
    }*/
    memmove(packet.data() + header.binary_pack(packet.data()), data, (uint32_t)len);
    return _tcp_stream.write(packet.data(), packet.size());
}

net::size_t websocket_stream::read(void* buffer, net::size_t buf_capacity) throw(...)
{
    //TODO: FIN 패킷이 아닐 때 인터페이스에 맞춰주는 코드.... 필요할까?
    net::size_t bytes_transferred = 0;
    try
    {
        bytes_transferred = _tcp_stream.read(buffer, buf_capacity);
    }
    catch (vee::exception& e)
    {
        printf("websocket_stream> exception occured at read() %s\n", e.what());
        return 0;
    }
    unsigned char* raw_data = (unsigned char *)buffer;
    data_frame_header header;
    header.analyze(raw_data, bytes_transferred);

    // Check the connection close operation
    if (header.opcode == data_frame_header::opcode_t::connnection_close)
    {
        throw ::vee::exception("Connection is closed by host", (int)error_code::connection_closed);
    }

    //memset(raw_data, 0, buf_capacity);
    // Copy binary data from buffer (DO NOT USE EXTEISNION FLAG!)
    //memmove(raw_data, raw_data + header.payload_pos, header.payload_len);
    // Unmask
    {
        for (unsigned int i = 0; i < header.payload_len; ++i)
        {
            unsigned char mask = header.masking_key[i % 4];
            raw_data[i + header.payload_pos] ^= mask;
        }
    }
    memmove(raw_data, raw_data + header.payload_pos, (uint32_t)(header.payload_len));
    memset(raw_data + header.payload_len, 0, (uint32_t)(buf_capacity - header.payload_len));
    //printf("payload_data: %s\n", raw_data);

    return header.payload_len;
}

void websocket_stream::conversion(tcp_stream&& stream)
{
    _tcp_stream = static_cast<tcp_stream&&>(stream);
    _host_io_service_ptr = &(_tcp_stream.get_io_service());
}

::std::shared_ptr<server_interface> create_server(unsigned short port)
{
    ::std::shared_ptr<server_interface> server = ::std::make_shared<websocket_server>(port);
    return server;
}

::std::shared_ptr<net_stream> create_stream()
{
    //TODO: IO_SERVICE SELECTION
    ::std::shared_ptr<net_stream> stream = ::std::make_shared<websocket_stream>();
    return stream;
}

} // namespace ws
} // namespace net
} // namespace voost
} // namespace vee