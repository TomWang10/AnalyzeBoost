#include "boost/asio.hpp"
#include <string>
#include <iostream>



int main()
{
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::socket sock;
    boost::asio::ip::tcp::resolver resolver(ioService);
    std::string outPut;

#if defined(BOOST_ASIO_HAS_IOCP)
    outPut = "iocp";
#else
    outPut = "select";
#endif
    std::cout << outPut << std::endl;
    getchar();
    return 0;
}