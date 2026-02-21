/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    Loop.cpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2025/12/11 21:10:03
 */

#include <unistd.h>
#include <limits.h>

#include <chrono>
#include <iostream>

#include <boost/log/trivial.hpp>

#include <boost/asio/post.hpp>

#include <ou/mqtt/mqtt.hpp>

#include "Loop.hpp"
#include "Config.hpp"

// TODO:
//   accept 'set' for writes
//   accept 'get' for refresh, specific key for single value
//   track changes and emit delta

Loop::Loop( const config::Values& choices, asio::io_context& io_context )
: m_choices( choices ), m_io_context( io_context )
, m_signals( io_context, SIGINT ) // SIGINT is called '^C'
, m_timerPollInterval( io_context )
, m_AnalogChannels( choices )
{
/*
  int rc;
  char szHostName[ HOST_NAME_MAX + 1 ];
  rc = gethostname( szHostName, HOST_NAME_MAX + 1 );
  if ( 0 != rc ) {
    m_sMqttId = szHostName;
  }
  else {
    assert( 0 < choices.mqtt.sId.size() );
    m_sMqttId = choices.mqtt.sId;
  }
*/
  m_pWorkGuard = std::make_unique<work_guard_t>( asio::make_work_guard( io_context ) );

  // https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/signal_set.html

  //signals.add( SIGKILL ); // not allowed here
  m_signals.add( SIGHUP ); // use this as a config change?
  //signals.add( SIGINFO ); // control T - doesn't exist on linux
  m_signals.add( SIGTERM );
  m_signals.add( SIGQUIT );
  m_signals.add( SIGABRT );

  m_signals.async_wait( [this]( const boost::system::error_code& error_code, int signal_number ){
    Signals( error_code, signal_number );
  } );

  try {
    m_pMqtt = std::make_unique<ou::Mqtt>( choices.mqtt );
  }
  catch ( const ou::Mqtt::runtime_error& e ) {
    BOOST_LOG_TRIVIAL(error) << "mqtt error: " << e.what() << '(' << e.rc << ')';
    throw e;
  }

  std::cout << "ctrl-c to end" << std::endl;

  asio::post( m_io_context, std::bind( &Loop::Poll, this ) );

}

void Loop::Signals( const boost::system::error_code& error_code, int signal_number ) {

  BOOST_LOG_TRIVIAL(info)
    << "signal"
    << "(" << error_code.category().name()
    << "," << error_code.value()
    << "," << signal_number
    << "): "
    << error_code.message()
    ;

  bool bContinue( true );

  switch ( signal_number ) {
    case SIGHUP:
      BOOST_LOG_TRIVIAL(info) << "sig hup no-op";
      break;
    case SIGTERM:
      BOOST_LOG_TRIVIAL(info) << "sig term";
      bContinue = false;
      break;
    case SIGQUIT:
      BOOST_LOG_TRIVIAL(info) << "sig quit";
      bContinue = false;
      break;
    case SIGABRT:
      BOOST_LOG_TRIVIAL(info) << "sig abort";
      bContinue = false;
      break;
    case SIGINT:
      BOOST_LOG_TRIVIAL(info) << "sig int";
      bContinue = false;
      break;
    case SIGPIPE:
      BOOST_LOG_TRIVIAL(info) << "sig pipe";
      bContinue = false;
      break;
    default:
      break;
  }

  if ( bContinue ) {
    m_signals.async_wait( [this]( const boost::system::error_code& error_code, int signal_number ){
    Signals( error_code, signal_number );
  } );
  }
  else {
    m_pWorkGuard.reset();
    m_timerPollInterval.cancel(); // comes after the reset
    bContinue = false;
  }
}

void Loop::Poll() {

  try {

    std::string sMessage;

    m_AnalogChannels.Process();
    m_AnalogChannels.ComposeMessage( sMessage );

    const std::string sTopic = m_choices.mqtt.sTopic;
    sMessage = '{' + sMessage + '}';

    //std::cout << "topic: " << sTopic << std::endl;
    //std::cout << sMessage << std::endl;

    m_pMqtt->Publish(
      sTopic, sMessage,
      []( bool bStatus, int code ){
        if ( bStatus ); // ok
        else {
          BOOST_LOG_TRIVIAL(error) << "mqtt publish error: " << code;
        }
      } );

  }
  catch( const std::logic_error& e ) {
    BOOST_LOG_TRIVIAL(error) << "open problems: " << e.what();
  }
  catch( ... ) {
    BOOST_LOG_TRIVIAL(error) << "unknown problem: ";
  }

  if ( m_pWorkGuard ) {
    m_timerPollInterval.expires_after( std::chrono::milliseconds( m_choices.nPollInterval) );
    m_timerPollInterval.async_wait( [this]( const boost::system::error_code& e ){
      if ( 0 == e.value() ) {
        Poll();
      }
      else {
        // Operation canceled [system:125]
        //BOOST_LOG_TRIVIAL(warning) << "timer msg: " << e.what(); // boost 1.81
        BOOST_LOG_TRIVIAL(warning) << "timer msg: " << e.message(); // boost 1.74
      }
    } );
  }
}

Loop::~Loop() {
  m_pWorkGuard.reset();
  m_pMqtt.reset();
}

