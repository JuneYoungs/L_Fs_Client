#include "stdafx.h"
#include "client.h"

void L::TCP_Client::uploadFile(std::string file_name) {
	char send_buf[1024] = { 0, };
	std::ifstream ifs;
	ifs.open(file_name);

	//std::string fileSize = std::to_string(sizeof(file_name));
	//m_Socket.write_some(boost::asio::buffer(fileSize));

	while (ifs.getline(send_buf, sizeof(send_buf))) {
		if (send_buf == NULL) break;
		std::cout << sizeof(send_buf) << std::endl;
		m_Socket.write_some(boost::asio::buffer(send_buf));
		std::cout << " sendData : " << send_buf << std::endl;
	}
	
	strcpy_s(send_buf, "END");
	m_Socket.write_some(boost::asio::buffer(send_buf));

	ifs.close();
}

void L::TCP_Client::PostWrite() {
	Menu();

	if (m_Socket.is_open() == false) {
		return;
	}
	getline(std::cin, m_WriteMessage);

	std::stringstream msgBuffer(m_WriteMessage);
	msgBuffer >> mbuffer;

	if (mbuffer == "upload") {
		boost::asio::async_write(m_Socket, boost::asio::buffer(m_WriteMessage),
			boost::bind(&TCP_Client::handle_write, 
			    this,
			    boost::asio::placeholders::error,
			    boost::asio::placeholders::bytes_transferred)
		);
		msgBuffer >> mbuffer;
		uploadFile(mbuffer);
		std::cout << "upload is End" << std::endl;
	}
	else {
		boost::asio::async_write(m_Socket, boost::asio::buffer(m_WriteMessage),
			boost::bind(&TCP_Client::handle_write, 
			    this,
			    boost::asio::placeholders::error,
			    boost::asio::placeholders::bytes_transferred)
		);
	}
	PostReceive();
}

void L::TCP_Client::PostReceive() {
	memset(&m_ReceiveBuffer, 0x00, sizeof(m_ReceiveBuffer));
	//std::cout << "Memset Okey" << std::endl;

	m_Socket.async_read_some(boost::asio::buffer(m_ReceiveBuffer),
		boost::bind(&TCP_Client::handle_receive, this,
		    boost::asio::placeholders::error,
		    boost::asio::placeholders::bytes_transferred)
	);
}

void L::TCP_Client::handle_connect(const boost::system::error_code& error) {
	if (error) {
		std::cout << "connect failed : " << error.message() << std::endl;
	}
	else {
		std::cout << "connected" << std::endl;

		PostWrite();
	}
}

void L::TCP_Client::handle_write(const boost::system::error_code& error, size_t bytes_transferred) {}
void L::TCP_Client::handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
	if (error) {
		if (error == boost::asio::error::eof) {
			std::cout << "Server disconnect." << std::endl;
		}
		else {
			std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
		}
	}
	else {
		const std::string strRecvMessage = m_ReceiveBuffer;
		std::cout << "====== Message from Server ====== \n" << strRecvMessage << std::endl;
		PostWrite();
	}
}

void L::TCP_Client::Menu() {
	std::cout << "\n\n========== Menual ==========" << std::endl;
	std::cout << "1. Path (pwd)" << std::endl;
	std::cout << "2. File / Forder List (ls)" << std::endl;
	std::cout << "3. Create Forder (mkdir ForderName)" << std::endl;
	std::cout << "4. Remove Forder (rm ForderName || rmdir ForderName)" << std::endl;
	std::cout << "5. Create File (mkfile FileName)" << std::endl;
	std::cout << "6. Remove File (rm FileName)" << std::endl;
	std::cout << "7. Rename (rename cur dest)" << std::endl;
	std::cout << "8. Upload File (upload FileName)" << std::endl;
}