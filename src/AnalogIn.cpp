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
 * File:    AnalogIn.cpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2025/12/11 22:01:23
 */

#include <boost/lexical_cast.hpp>

#include "AnalogIn.hpp"

namespace {
  // referenced from Exploring BeagleBone 2e 2019 Derek Molloy
  static const std::string c_analog_in_path( "/sys/bus/iio/devices/iio:device0/in_voltage" );
}

AnalogIn::AnalogIn( uint16_t ix_ )
: ix( ix_ )
, path( c_analog_in_path + boost::lexical_cast<std::string>( ix_ ) + "_raw" )
, value {}
{
}

AnalogIn::AnalogIn( AnalogIn&& rhs )
: ix( rhs.ix )
, path( std::move( rhs.path ) )
, value( rhs.value )
{}

uint16_t AnalogIn::Read( std::fstream& fs ) {
  fs.open( path, std::fstream::in );
  fs >> value;
  fs.close();
  return value;
}

// =======

AnalogChannels::AnalogChannels( const config::Values& choices ) {
  for ( uint16_t ix: choices.setAnalogInIx ) {
    vAnalogIn.emplace_back( AnalogIn( ix ) );
  }
}

void AnalogChannels::Process( std::string& sMessage ) {

  uint16_t value;
  bool bComma( false );

  for ( AnalogIn& ain: vAnalogIn ) {
    value = ain.Read( fs );

    if ( bComma ) sMessage += ',';
    else bComma = true;

    sMessage
      += std::string()
       + '"' + "ain"
       + boost::lexical_cast<std::string>( ain.Ix() )
       + '"' + ':'
       + boost::lexical_cast<std::string>( value )
      ;
  }
}

