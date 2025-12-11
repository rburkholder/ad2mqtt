/*
  project: ad2mqtt
  author:  raymond@burkholder.net
  date:    2025/12/10 21:23:48
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

// keep the device open, may lock out other devices
struct iio_device {
  std::fstream fs;
  std::stringstream ss;
  iio_device( const uint16_t ix ) {
    ss << c_analog_in_path << ix << "_raw";
    fs.open( ss.str().c_str(), std::fstream::in );
  }
  ~iio_device() {
    fs.close();
  }
  uint16_t get() {
    uint16_t value;
    fs >> value;
    return value;
  }
};

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
  iio_device iio0( 0 );
  iio_device iio1( 1 );
  
  while( true ) {
    const uint16_t ani0 = iio0.get();
    mm.update( ani0 );

    const uint16_t ani1 = iio1.get();
    mm.update( ani1 );

    std::cout << mm.min << ',' << mm.max << ':' << ani0 << ',' << ani1 << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
  }

  return EXIT_SUCCESS;
}

