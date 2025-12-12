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
 * File:    main.cpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2025/12/10 21:23:48
 */

#include <iostream>
#include <sstream>
#include <fstream>

#include <string>

#include <chrono>
#include <thread>

#include <vector>

//#include <boost/asio/io_context.hpp>

//#include "Loop.hpp"
#include "Config.hpp"

//namespace asio = boost::asio;

// referenced from Exploring BeagleBone 2e 2019 Derek Molloy
static const std::string c_analog_in_path( "/sys/bus/iio/devices/iio:device0/in_voltage" );

struct minmax {
  uint16_t max;
  uint16_t min;
  bool bFirst;

  minmax(): bFirst( true ) {}

  void update( const uint16_t value ) {
    if ( bFirst ) {
      min = max = value;
      bFirst = false;
    }
    else {
      if ( min > value ) min = value;
      else {
        if ( max < value ) max = value;
      }
    }
  }

};

class AnalogIn {
public:
  AnalogIn( uint16_t ix_ )
  : ix( ix_ )
  {
    ss << c_analog_in_path << ix << "_raw";
  }
  AnalogIn( AnalogIn&& rhs )
  : ix( rhs.ix )
  , ss( std::move( rhs.ss ) )
  , fs( std::move( rhs.fs ) )
  {}

  uint16_t Read() {
    uint16_t value;
    fs.open( ss.str().c_str(), std::fstream::in );
    fs >> value;
    fs.close();
    return value;
  }

protected:
private:
  const uint16_t ix; // not used
  std::stringstream ss;
  std::fstream fs;
};

int main( int argc, char **argv ) {

  static const std::string c_sConfigFilename( "ad2mqtt.cfg" );

  std::cout << "AD2MQTT (c)2025 One Unified Net Limited" << std::endl;

  config::Values choices;

  if ( Load( c_sConfigFilename, choices ) ) {
  }
  else {
    return EXIT_FAILURE;
  }

  using vAnalogIn_t = std::vector<AnalogIn>;
  vAnalogIn_t vAnalogIn;

  for ( uint16_t ix: choices.setAnalogInIx ) {
    vAnalogIn.emplace_back( AnalogIn( ix ) );
  }

  //asio::io_context io_context;

  //try {
  //  Loop loop( choices, io_context );
  //  io_context.run();
  //}
  //catch(...) {
  //  return EXIT_FAILURE;
  //}

  minmax mm;

  using vValue_t = std::vector<uint16_t>;
  vValue_t vValue;

  while( true ) {

    uint16_t value;

    uint16_t value_max {};

    for ( AnalogIn& ain: vAnalogIn ) {
      value = ain.Read();
      mm.update( value );
      if ( value > value_max ) value_max = value;
      vValue.push_back( value );
    }

    std::cout << mm.min << ',' << mm.max << ':';

    bool bComma( false );
    for ( const uint16_t value: vValue ) {

      if ( bComma ) std::cout << ',';
      else bComma = true;

      std::cout << value;
    }
    vValue.clear();

    if ( 2000 < value_max ) {
      std::cout << " ********";
      std::cout << '\a';
    }

    std::cout << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
  }

  return EXIT_SUCCESS;
}

