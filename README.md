# ad2mqtt
Communicate Beagle analog values to MQTT

Packages to be installed (all platforms):
```
sudo apt install libpaho-mqtt-dev
```

Packages (beaglebone) simply use the 'debian generic' section below, the other two sections are here as a reference:
```
# debian version 12.2 (not available yet)
sudo apt install libboost-filesystem1.81-dev
sudo apt install libboost-log1.81-dev
sudo apt install libboost-program-options1.81-dev
sudo apt install libboost-thread1.81-dev
# debian version 11.7 (as of 2023/10/08)
sudo apt install libboost-filesystem1.74-dev
sudo apt install libboost-log1.74-dev
sudo apt install libboost-program-options1.74-dev
sudo apt install libboost-thread1.74-dev
# debian generic
sudo apt install libboost-filesystem-dev
sudo apt install libboost-log-dev
sudo apt install libboost-program-options-dev
sudo apt install libboost-thread-dev
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

poll_interval = 1

# valud values: 0 - 6
analog_in_ix = 0
analog_in_ix = 1
analog_in_ix = 2

```

