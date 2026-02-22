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

#include <boost/log/trivial.hpp>

#include "GasValve.hpp"

GasValve::GasValve( const std::string& gpio, uint16_t upper, uint16_t lower )
: m_nUpper( upper ), m_nLower( lower )
, m_sGPIO( gpio )
{
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
}

GasValve::~GasValve() {}

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
