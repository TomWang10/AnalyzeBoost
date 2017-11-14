#include "boost/asio.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include "boost/bind.hpp"
#include <string>
#include <iostream>
#include <vector>

using boost::asio::ip::tcp;

typedef boost::shared_ptr<tcp::socket> Session;
typedef boost::shared_ptr <std::vector<char>> Buff;

void HandleAccept(Session sess, const boost::system::error_code& error);
void HandleRead(Session sess, Buff buff, const boost::system::error_code& error, std::size_t recv);
void HandleWrite(Session sess, const boost::system::error_code& error, std::size_t recv);

int main()
{
    boost::asio::io_service ioService;
	boost::asio::io_service::work worker(ioService);
	boost::asio::ip::tcp::endpoint serverEndpoint(boost::asio::ip::tcp::v4(), 2000);
    boost::asio::ip::tcp::acceptor acceptors(ioService, serverEndpoint);
	Session newSession(boost::make_shared<Session::element_type>(ioService));
	acceptors.async_accept(*newSession.get(), boost::bind(HandleAccept, newSession, boost::asio::placeholders::error));
	ioService.run();
    getchar();
    return 0;
}

void HandleAccept(Session sess, const boost::system::error_code& error)
{
	if (!error)
	{
		Buff readBuff(boost::make_shared<Buff::element_type>(100, 0));
		(*sess.get()).async_read_some(boost::asio::buffer(readBuff->data(), 100),
			boost::bind(HandleRead, sess, readBuff, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		std::cout << "error!!!!!!!!!!!!!\n";
	}
}

void HandleRead(Session sess, Buff buff, const boost::system::error_code& error, std::size_t recv)
{
	if (!error)
	{
		std::cout << "Read some: " << buff.get() << std::endl;
		(*sess.get()).async_send(boost::asio::buffer(buff->data(), recv),
			boost::bind(HandleWrite, sess, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

void HandleWrite(Session sess, const boost::system::error_code& error, std::size_t recv)
{
	if (!error)
	{
		std::cout << "send " << recv << std::endl;
	}
	else
	{

	}
}
