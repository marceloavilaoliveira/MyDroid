#!/usr/bin/perl

use Device::SerialPort;

my $arduino = Device::SerialPort->new("/dev/ttyACM0");

$arduino->baudrate(9600);
$arduino->databits(8);
$arduino->parity("none");
$arduino->stopbits(1);
$arduino->write_settings();

$arduino->write(23);
$arduino->write("|");
$arduino->write(0);
$arduino->write("|");

exit 0;
