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
 * File:    GpioState.cpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2026/04/19 14:21:56
 */

#include "GpioState.hpp"

namespace {
  static const std::filesystem::path chip_path("/dev/gpiochip3");
  // based upon gpioinfo -c gpiochip3
  //static const ::gpiod::line::offset line_offset = 5;
}

GpioState::GpioState( const std::string& sName, unsigned int line_offset )
: chip( chip_path )
, lo( line_offset )
, lr(
   chip.prepare_request() // https://libgpiod.readthedocs.io/en/latest/cpp_chip.html
  .set_consumer( sName ) // A label for the consumer in gpioinfo
  .add_line_settings(
    line_offset,
    ::gpiod::line_settings()
      .set_direction( ::gpiod::line::direction::OUTPUT ) // Set as output
      .set_active_low( true )
   )
  .do_request() // Request the line(s)
 )
{}

GpioState::~GpioState() {
  RelayOff();
  lr.release();
}

void GpioState::RelayOn() {
  lr.set_value( lo, ::gpiod::line::value::ACTIVE );
}

void GpioState::RelayOff() {
  lr.set_value( lo, ::gpiod::line::value::INACTIVE );
}

