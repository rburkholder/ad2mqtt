/*
  project: ad2mqtt
  author:  raymond@burkholder.net
  date:    2025/12/10 21:23:48
*/

#include <iostream>
#include <sstream>
#include <fstream>

#include <chrono>
#include <thread>

//#include <boost/asio/io_context.hpp>

//#include "Loop.hpp"
//#include "Config.hpp"

//namespace asio = boost::asio;

#define ADC_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"
#define ADC 0

int readAnalog( int number ) {
  std::stringstream ss;
  ss << ADC_PATH << number << "_raw";
  std::fstream fs;
  fs.open( ss.str().c_str(), std::fstream::in );
  fs >> number;
  fs.close();
  return number;
}

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
  
  while( true ) {
    std::cout << "ani: " << readAnalog( 0 ) << ',' << readAnalog( 1 ) << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
  }

  return EXIT_SUCCESS;
}

