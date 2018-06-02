/*
 * Copyright (c) 2015, Wieden+Kennedy
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ban the Rewind nor the names of its
 * contributors may be used to endorse or promote products
 * derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include "WebSocketConnection.h"

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

class WebSocketServer : public WebSocketConnection
{
public:
	typedef websocketpp::server<websocketpp::config::asio>	Server;
	typedef Server::connection_ptr							ConnectionRef;
	typedef Server::message_ptr								MessageRef;

	WebSocketServer();
	~WebSocketServer();
	
	void			cancel();
	void			listen( uint16_t port = 80 );
	void			ping( const std::string& msg = "" );
	void			poll();
	void			run();
	void			write( const std::string& msg );
	void			write( void const * msg, size_t len );

	Server&			getServer();
	const Server&	getServer() const;
protected:
	Server			mServer;
	
	void			onClose( Server* server, websocketpp::connection_hdl handle );
	void			onFail( Server* server, websocketpp::connection_hdl handle );
	void			onHttp( Server* server, websocketpp::connection_hdl handle );
	void			onInterrupt( Server* server, websocketpp::connection_hdl handle );
	void			onMessage( Server* server, websocketpp::connection_hdl handle, MessageRef msg );
	void			onOpen( Server* server, websocketpp::connection_hdl handle );
	bool			onPing( Server* server, websocketpp::connection_hdl handle, std::string msg );
	void			onSocketInit( Server* server, websocketpp::connection_hdl handle, asio::ip::tcp::socket& socket );
	void			onTcpPostInit( Server* server, websocketpp::connection_hdl handle );
	void			onTcpPreInit( Server* server, websocketpp::connection_hdl handle );
	bool			onValidate( Server* server, websocketpp::connection_hdl handle );
};
