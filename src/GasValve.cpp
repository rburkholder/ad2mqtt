/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    Gas.cpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2026/02/21 18:47:09
 */

#include <unistd.h> // For usleep

#include <boost/log/trivial.hpp>

#include <gpiod.hpp>

#include "GasValve.hpp"

namespace {
  static const std::filesystem::path chip_path("/dev/gpiochip3");
  static const ::gpiod::line::offset line_offset = 5;
}

GasValve::GasValve( const std::string& gpio, uint16_t upper, uint16_t lower )
: m_nUpper( upper ), m_nLower( lower )
, m_sGPIO( gpio )
{

  BOOST_LOG_TRIVIAL(info) << "gpio version = " << ::gpiod::api_version();

  m_fHysteresis_jump =
    [this]( uint16_t value ){ // Hysteresis_start
      if ( m_nLower > value ) {
        m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_lt( value ); };
      }
      else {
        if ( m_nUpper < value ) {
          m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_gt( value ); };
        }
      }
    };

  try {
    ::gpiod::chip chip( chip_path );
    auto line_request = chip.prepare_request()
      .set_consumer("gas_valve_hi") // A label for the consumer
      .add_line_settings( line_offset, ::gpiod::line_settings()
        .set_direction( ::gpiod::line::direction::OUTPUT ) // Set as output
        .set_output_value( ::gpiod::line::value::ACTIVE )) // Set initial value (HIGH)
      .do_request(); // Request the line(s)
    m_pLineRequest->set_value( line_offset, ::gpiod::line::value::ACTIVE);
    usleep( 1000000 ); // Wait for 1,000,000 microseconds (0.1 seconds)
    m_pLineRequest = std::make_unique<gpiod::line_request>( std::move( line_request ) );
  }
  catch ( const std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << "GPIO error: " << e.what() << std::endl;
  }

}

GasValve::~GasValve() {
  try {
    m_pLineRequest->set_value( line_offset, ::gpiod::line::value::INACTIVE);
    m_pLineRequest->release();
  }
  catch ( const std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << "GPIO error: " << e.what() << std::endl;
  }
}

void GasValve::Process( uint16_t value ) {
  m_fHysteresis_jump( value );
}

void GasValve::Hysteresis_gt( uint16_t value ) {
  if ( m_nLower > value ) {
    BOOST_LOG_TRIVIAL(trace) << "gas valve on (" << value << ")";
    m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_lt( value ); };
  }
}

void GasValve::Hysteresis_lt( uint16_t value ) {
  if ( m_nUpper < value ) {
    BOOST_LOG_TRIVIAL(trace) << "gas valve off (" << value << ")";
    m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_gt( value ); };
  }
}
