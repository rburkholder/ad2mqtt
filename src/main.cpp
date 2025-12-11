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

//#include <boost/asio/io_context.hpp>

//#include "Loop.hpp"
//#include "Config.hpp"

//namespace asio = boost::asio;

// referenced from Exploring BeagleBone 2e 2019 Derek Molloy
static const std::string c_analog_in_path( "/sys/bus/iio/devices/iio:device0/in_voltage" );

uint16_t readAnalog( uint16_t ix ) { // range 0 - 4095 / 12 bits
  uint16_t value;
  std::stringstream ss;
  ss << c_analog_in_path << ix << "_raw";
  std::fstream fs;
  fs.open( ss.str().c_str(), std::fstream::in );
  fs >> value;
  fs.close();
  return value;
}

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

int main( int argc, char **argv ) {

  static const std::string c_sConfigFilename( "ad2mqtt.cfg" );

  std::cout << "AD2MQTT (c)2025 One Unified Net Limited" << std::endl;

  //config::Values choices;

  //if ( Load( c_sConfigFilename, choices ) ) {
  //}
  //else {
  //  return EXIT_FAILURE;
  //}

  //asio::io_context io_context;

  //try {
  //  Loop loop( choices, io_context );
  //  io_context.run();
  //}
  //catch(...) {
  //  return EXIT_FAILURE;
  //}

  minmax mm;
  
  while( true ) {
    const uint16_t ani0 = readAnalog( 0 );
    mm.update( ani0 );

    const uint16_t ani1 = readAnalog( 1 );
    mm.update( ani1 );

    std::cout << mm.min << ',' << mm.max << ':' << ani0 << ',' << ani1 << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
  }

  return EXIT_SUCCESS;
}

