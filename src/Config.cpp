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
 * File:    Config.cpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2025/12/10 23:24:17
 */

#include <vector>
#include <fstream>
#include <exception>
//#include <type_traits>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {

  static const std::string sValue_Mqtt_Id( "mqtt_id" );
  static const std::string sValue_Mqtt_Host( "mqtt_host" );
  static const std::string sValue_Mqtt_UserName( "mqtt_username" );
  static const std::string sValue_Mqtt_Password( "mqtt_password" );
  static const std::string sValue_Mqtt_Topic( "mqtt_topic" );

  static const std::string sValue_PollInterval( "poll_interval" );

  static const std::string sValue_AnalogInIx( "analog_in_ix" );

  //template<typename T>
  //void log( const std::string& name, typename std::enable_if<std::is_pod<T&>::value>::type& dest ) {
  //  BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
  //}

  void log( const std::string& name, int dest ) {
    BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
  }

  void log( const std::string& name, const std::string& dest ) {
    BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
  }

  //void log( const std::string& name, const vName_t& dest ) {
  //  for ( const vName_t::value_type& value: dest ) {
  //    BOOST_LOG_TRIVIAL(info) << name << " = " << value;
  //  }
  //}

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = vm[name].as<T>();
      //BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
      log( name, dest );
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << name << "='";
      bOk = false;
    }
    return bOk;
  }
}

namespace config {

bool Load( const std::string& sFileName, Values& values ) {

  bool bOk( true );

  using vAnalogInIx_t = std::vector<uint16_t>;
  vAnalogInIx_t vAnalogInIx;

  try {

    po::options_description config( "AD2MQTT Config" );
    config.add_options()

      ( sValue_Mqtt_Id.c_str(), po::value<std::string>( &values.mqtt.sId ), "mqtt client id" )
      ( sValue_Mqtt_Host.c_str(), po::value<std::string>( &values.mqtt.sHost )->default_value( "localhost" ), "mqtt host address or name" )
      ( sValue_Mqtt_UserName.c_str(), po::value<std::string>( &values.mqtt.sUserName ), "mqtt username" )
      ( sValue_Mqtt_Password.c_str(), po::value<std::string>( &values.mqtt.sPassword ), "mqtt password" )
      ( sValue_Mqtt_Topic.c_str(), po::value<std::string>( &values.mqtt.sTopic )->default_value( "analog_in" ), "mqtt topic" )

      ( sValue_PollInterval.c_str(), po::value<uint16_t>( &values.nPollInterval ), "poll interval (milliseconds)" )

      ( sValue_AnalogInIx.c_str(), po::value<vAnalogInIx_t>( &vAnalogInIx ), "analog-in index" )
      ;
    po::variables_map vm;
    //po::store( po::parse_command_line( argc, argv, config ), vm );

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "ad2mqtt config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Id, values.mqtt.sId );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Host, values.mqtt.sHost );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_UserName, values.mqtt.sUserName );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Password, values.mqtt.sPassword );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Topic, values.mqtt.sTopic );

      bOk &= parse<uint16_t>( sFileName, vm, sValue_PollInterval, values.nPollInterval );
      if ( 10 > values.nPollInterval ) {
        BOOST_LOG_TRIVIAL(error)
          << "polling interval " << values.nPollInterval
          << " must be 10 or higher";
        bOk = false;
      }

      if ( 0 < vm.count( sValue_AnalogInIx ) ) {
        vAnalogInIx = vm[ sValue_AnalogInIx ].as<vAnalogInIx_t>();
      }
      else {
        bOk = false;
      }
      //bOk &= parse<setAnalogInIx_t>( sFileName, vm, sValue_AnalogInIx, values.set_ani );
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  values.setAnalogInIx.clear();
  for ( const auto ix: vAnalogInIx ) {
    values.setAnalogInIx.insert( ix );
  }

  return bOk;
}

} // namespace config
