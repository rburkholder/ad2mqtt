# ad2mqtt
Communicate Beagle analog values to MQTT & act as thermostat for hydronic furnace gas valve

Packages to be installed:
```
sudo apt install --no-install-recommends libpaho-mqtt-dev
sudo apt install --no-install-recommends libboost-log-dev
sudo apt install --no-install-recommends libboost-program-options-dev
sudo apt install --no-install-recommends cmake
sudo apt install --no-install-recommends ninja-build
```

To Build statically linked application:
```
git clone https://github.com/rburkholder/ad2mqtt.git
cd ad2mqtt
mkdir build
cd build
cmake ..
make
cd ..
ln -s build/src/ad2mqtt ad2mqtt
# create & populate ad2mqtt.cfg with example
# then run:
./ad2mqtt
```

# TODO
Create an example systemd control file can be found in systemd/nut2mqtt.service.  Change working directory & executable to suit.

Configuration File template (change usernames, passwords, and addresses):
```
$ cat x64/debug/ad2mqtt.cfg
mqtt_id = iad1
mqtt_host = 127.0.0.1
mqtt_username = admin
mqtt_password = password
mqtt_topic = ad

# milliseconds
poll_interval = 1000
report_interval = 1

# valid values: 0 - 6
analog_in_ix = 0
analog_in_ix = 1
analog_in_ix = 2

ain_temperature_ix = 1

# relay port 1
gas_gpio = P9_17 [spi0_cs0]
gas_upper = 1883
gas_lower = 1860

# relay port 4
pump_gpio = P9_11 [uart4_rxd]
pump_upper = 400
pump_lower = 200

```

