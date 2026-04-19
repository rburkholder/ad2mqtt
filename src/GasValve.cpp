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
 * File:    GasValve.cpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2026/02/21 18:47:09
 */

#include <boost/log/trivial.hpp>

#include "GpioState.hpp"
#include "GasValve.hpp"

GasValve::GasValve( unsigned int gpio_line, uint16_t upper, uint16_t lower )
: m_nUpper( upper ), m_nLower( lower )
{

  BOOST_LOG_TRIVIAL(info) << "gpio version = " << ::gpiod::api_version() << " (gas valve)";

  m_fHysteresis_jump =
    [this]( uint16_t value ){ // Hysteresis_start
      if ( m_nLower > value ) {
        BOOST_LOG_TRIVIAL(trace) << "heat call enable  (" << m_nLower << " > " << value << ") - init";
        m_pGasValveState->RelayOff();
        m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_lt( value ); };
      }
      else {
        if ( m_nUpper < value ) {
          BOOST_LOG_TRIVIAL(trace) << "heat call disable (" << m_nUpper << " < " << value << ") - init";
          m_pGasValveState->RelayOn();
          m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_gt( value ); };
        }
      }
    };

  m_pGasValveState = std::make_unique<GpioState>( "heat_call_enable", gpio_line );
  //m_pGasValveState->Disable(); // wait until we know water temperature

}

GasValve::~GasValve() {
  m_pGasValveState.reset();
}

void GasValve::Process( uint16_t value ) {
  m_fHysteresis_jump( value );
}

void GasValve::Hysteresis_gt( uint16_t value ) {
  if ( m_nLower > value ) {
    BOOST_LOG_TRIVIAL(trace) << "heat call enable  (" << m_nLower << " > " << value << ")";
    m_pGasValveState->RelayOff(); // normally closed (n/c) - closed - allow heat call
    m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_lt( value ); };
  }
}

void GasValve::Hysteresis_lt( uint16_t value ) {
  if ( m_nUpper < value ) {
    BOOST_LOG_TRIVIAL(trace) << "heat call disable (" << m_nUpper << " < " << value << ")";
    m_pGasValveState->RelayOn(); // normally closed (n/c) - open - disallow heat call
    m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_gt( value ); };
  }
}
