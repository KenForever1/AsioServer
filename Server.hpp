#include <boost/asio/buffer.hpp>
#include <unordered_map>
#include <numeric>
#include "RWHandler.hpp"
#include <list>

const int MaxConnectionNum = 65536;
const int MaxRecvSize = 65536;

class Server {
public:
    Server(io_service &ios, short port) : m_ios(ios),
                                          m_acceptor(ios, tcp::endpoint(tcp::v4(), port)),
                                          m_cnnIdPool(MaxConnectionNum) {

        m_cnnIdPool.resize(MaxConnectionNum);
        std::iota(m_cnnIdPool.begin(), m_cnnIdPool.end(), 1);
    }

    ~Server() {

    }

    void Accept() {
        std::cout<<"Start Listening"<<std::endl;
        std::shared_ptr<RWHandler> handler = CreateHandler();

        m_acceptor.async_accept(handler->GetSocket(),[this,handler](const boost::system::error_code& ec){
            if (ec){
                std::cout<<ec.value()<<" "<<ec.message()<<std::endl;
                HandleAcpError(handler,ec);
                return;
            }

            m_handlers.insert(std::make_pair(handler->GetConnId(),handler));
            std::cout<< "current connect count: "<<m_handlers.size()<<std::endl;

            handler->HandleRead();

            Accept();
        });

    }

private:
    void HandleAcpError(std::shared_ptr<RWHandler> eventHandler,
                        const boost::system::error_code &ec) {
        std::cout << "Error ,error reason: " << ec.value() << ec.message() << std::endl;
        eventHandler->CloseSocket();
        StopAccept();
    }

    void StopAccept() {
        boost::system::error_code ec;
        m_acceptor.cancel(ec);
        m_acceptor.close(ec);
        m_ios.stop();
    }

    std::shared_ptr<RWHandler> CreateHandler() {
        int connId = m_cnnIdPool.front();
        m_cnnIdPool.pop_front();

        std::shared_ptr<RWHandler> handler = std::make_shared<RWHandler>(m_ios);
        handler->SetConnId(connId);
        handler->SetCallBackError([this](int connId) {
            RecyclConnid(connId);
        });
        return handler;
    }

    void RecyclConnid(int connId) {
            auto it = m_handlers.find(connId);
            if(it != m_handlers.end()){
                m_handlers.erase(it);
            }
            std::cout<<"current connect count: "<<m_handlers.size()<<std::endl;
            m_cnnIdPool.push_back(connId);
    }

private:
    io_service &m_ios;
    tcp::acceptor m_acceptor;
    std::unordered_map<int, std::shared_ptr<RWHandler>> m_handlers;
    std::list<int> m_cnnIdPool;
};