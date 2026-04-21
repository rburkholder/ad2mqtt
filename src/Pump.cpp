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

#include <vector>

#include <boost/log/trivial.hpp>

#include "GpioState.hpp"
#include "Pump.hpp"

Pump::Pump( unsigned int gpio_line, uint16_t upper, uint16_t lower )
: m_nUpper( upper ), m_nLower( lower )
, m_cntHeatCall {}
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

// when temperature is below setpoint, and no heat call, relay on for n/c connection, turns off pump
void Pump::Hysteresis_gt( uint16_t value ) {
  if ( ( m_nLower > value ) && ( 0 == m_cntHeatCall ) ) {
    BOOST_LOG_TRIVIAL(trace) << "pump disable (" << m_nLower << " > " << value << ")";
    m_pPumpState->RelayOn(); // normally closed (n/c) - open - disallow pump
    m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_lt( value ); };
  }
}

// when temperature is above setpoint, relay open for n/c connection, turns on pump
void Pump::Hysteresis_lt( uint16_t value ) {
  if ( m_nUpper < value ) {
    BOOST_LOG_TRIVIAL(trace) << "pump enable  (" << m_nUpper << " < " << value << ")";
    m_pPumpState->RelayOff(); // normally closed (n/c) - close - allow pump
    m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_gt( value ); };
  }
}

void Pump::Process( const std::string_view& svTopic, const std::string_view& svMessage ) {

  static const char comma( ',' );
  using vkv_t = std::vector<std::string_view>;

  vkv_t vkv;

  //BOOST_LOG_TRIVIAL(trace) << "pump trigger " << svTopic << " " << svMessage;

  bool bOk = true;
  if ( 2 < svMessage.size() ) {
    if ( '{' == svMessage[ 0 ] ) {
      size_t begin = 1;
      size_t end = svMessage.find( comma );
      while ( end != std::string::npos ) {
        vkv.push_back( svMessage.substr( begin, end - begin ) );
        begin = end + 1;
        end = svMessage.find( comma, begin );
      }
      vkv.push_back( svMessage.substr( begin, end - begin - 1 ) );

      //for ( const auto& item: vkv ) {
      //  BOOST_LOG_TRIVIAL(trace) << "pump trigger sub " << item;
      //}

      // sample message:
      // zwave/1/13/66/0/state {"time":1776620420521,"value":0,"nodeName":"thermostat03","nodeLocation":"family room"}

      if ( 4 == vkv.size() ) {
        const std::string_view svValue( vkv[ 1 ] );
        if ( 0 == svValue.find( "\"value\":" ) ) {
          const char value( svValue[ svValue.size() - 1 ] );
          if ( '0' == value ) {
            if ( 0 == m_cntHeatCall ) {}
            else {
              m_cntHeatCall--;
            }
            if ( ( 0 == m_cntHeatCall ) && ( !m_pPumpState->State() ) ) {
              //BOOST_LOG_TRIVIAL(trace) << "pump trigger - enable off";
            }
          }
          else {
            if ( '1' == value ) {
              m_cntHeatCall++;
              if ( m_pPumpState->State() ) { // relay is on, pump is idle
                // force pump on
                BOOST_LOG_TRIVIAL(trace) << "pump trigger - turn on";
                m_pPumpState->RelayOff();
                m_fHysteresis_jump = [this]( uint16_t value ){ Hysteresis_gt( value ); };
              }

            }
            else {
              bOk = false;
              BOOST_LOG_TRIVIAL(error) << "pump trigger value is not 0 or 1 (" << value << ")";
            }
          }
        }
        else {
          bOk = false;
          BOOST_LOG_TRIVIAL(error) << "pump trigger - can not find value (" << svValue << ")";
        }
      }
      else {
        bOk = false;
        BOOST_LOG_TRIVIAL(error) << "pump trigger - not four entries (" << vkv.size() << ")";
      }

    }
    else {
      bOk = false;
      BOOST_LOG_TRIVIAL(error) << "pump trigger - no open bracket found";
    }
  }
  else {
    bOk = false;
    BOOST_LOG_TRIVIAL(error) << "pump trigger - message too short";
  }

  if ( !bOk ) {
    BOOST_LOG_TRIVIAL(error) << "pump trigger error: " << svMessage;
  }
}

