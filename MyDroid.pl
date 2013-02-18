#!/usr/bin/perl

use Device::SerialPort;

my $arduino = Device::SerialPort->new("/dev/ttyACM0");

$arduino->baudrate(9600);
$arduino->databits(8);
$arduino->parity("none");
$arduino->stopbits(1);

# RESET
#$arduino->write(0);
#$arduino->write("|");
#$arduino->write(0);
#$arduino->write("|");

# HAPPY
#$arduino->write(2);
#$arduino->write("|");
#$arduino->write(0);
#$arduino->write("|");

# ANGRY
$arduino->write(3);
$arduino->write("|");
$arduino->write(0);
$arduino->write("|");

# SCARY
#$arduino->write(4);
#$arduino->write("|");
#$arduino->write(0);
#$arduino->write("|");

exit 0;
