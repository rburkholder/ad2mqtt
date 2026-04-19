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
 * File:    Pump.cpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2026/04/19 15:55:19
 */

#include <boost/log/trivial.hpp>

#include "GpioState.hpp"
#include "Pump.hpp"

Pump::Pump( unsigned int gpio_line, uint16_t upper, uint16_t lower )
: m_nUpper( upper ), m_nLower( lower )
{

  BOOST_LOG_TRIVIAL(info) << "gpio version = " << ::gpiod::api_version() << " (pump)";

  m_fHysteresis_jump =
    [this]( uint16_t value ){ // Hysteresis_start
      if ( m_nLower > value ) {
        BOOST_LOG_TRIVIAL(trace) << "pump disable  (" << m_nLower << " > " << value << ") - init";
        m_pPumpState->RelayOn();
        m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_lt( value ); };
      }
      else {
        if ( m_nUpper < value ) {
          BOOST_LOG_TRIVIAL(trace) << "pump enable (" << m_nUpper << " < " << value << ") - init";
          m_pPumpState->RelayOff();
          m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_gt( value ); };
        }
      }
    };

  m_pPumpState = std::make_unique<GpioState>( "pump_enable", gpio_line );

}

Pump::~Pump() {
  m_pPumpState.reset();
}

void Pump::Process( uint16_t value ) {
  m_fHysteresis_jump( value );
}

void Pump::Hysteresis_gt( uint16_t value ) {
  if ( m_nLower > value ) {
    BOOST_LOG_TRIVIAL(trace) << "pump disable (" << m_nLower << " > " << value << ")";
    m_pPumpState->RelayOn(); // normally closed (n/c) - open - disallow pump
    m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_lt( value ); };
  }
}

void Pump::Hysteresis_lt( uint16_t value ) {
  if ( m_nUpper < value ) {
    BOOST_LOG_TRIVIAL(trace) << "pump enable  (" << m_nUpper << " < " << value << ")";
    m_pPumpState->RelayOff(); // normally closed (n/c) - close - allow pump
    m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_gt( value ); };
  }
}

