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
 * File:    Loop.hpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2025/12/11 21:10:03
 */

#include <memory>
#include <string>

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include "AnalogIn.hpp"
#include "GasValve.hpp"

namespace config {
  class Values;
}

namespace ou {
  class Mqtt;
}

namespace asio = boost::asio; // from <boost/asio/context.hpp>

class Loop {
public:
  Loop( const config::Values&, asio::io_context& );
  ~Loop();
protected:
private:

  const config::Values& m_choices;
  asio::io_context& m_io_context;

  AnalogChannels m_AnalogChannels;

  std::unique_ptr<ou::Mqtt> m_pMqtt;

  using work_guard_t = asio::executor_work_guard<asio::io_context::executor_type>;
  using pWorkGuard_t = std::unique_ptr<work_guard_t>;

  pWorkGuard_t m_pWorkGuard;

  using setName_t = std::set<std::string>;
  using vValue_t = std::vector<std::string>;

  setName_t m_setDeviceNames;

  asio::signal_set m_signals;
  asio::steady_timer m_timerPollInterval;

  uint16_t m_cntReportTrigger; // m_choices.nReportInterval

  GasValve m_gas_valve;

  void Poll();
  void Signals( const boost::system::error_code&, int );

};

