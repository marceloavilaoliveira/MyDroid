#!/usr/bin/perl
#------------------------------------------------------------------------------#
# Pathname    : MyDroid.pl                                                     #
# Description : Android Figure Automation                                      #
# Version     : 2.4.0                                                          #
# Author      : Marcelo Avila de Oliveira <marceloavilaoliveira@gmail.com>     #
#------------------------------------------------------------------------------#

#------------------------------------------------------------------------------#
# LIBRARIES                                                                    #
#------------------------------------------------------------------------------#

use strict;
use Getopt::Std;
use File::Basename;
use Device::SerialPort;

#------------------------------------------------------------------------------#
# VARIABLES                                                                    #
#------------------------------------------------------------------------------#

my $SCRIPT_PATHNAME = `readlink -nf "$0"`;
my $SCRIPT_NAME = basename($SCRIPT_PATHNAME);

my $ARDUINO = Device::SerialPort->new("/dev/ttyACM0");
if (defined $ARDUINO) {
    $ARDUINO->baudrate(9600);
    $ARDUINO->databits(8);
    $ARDUINO->parity("none");
    $ARDUINO->stopbits(1);
    $ARDUINO->write_settings();
}

my $OPTION = "";
my $PARAMETER = "";

#------------------------------------------------------------------------------#
# PARAMETERS                                                                   #
#------------------------------------------------------------------------------#

my $usage = "\nDescription:\n";
$usage   .= "  Android Figure Automation\n";
$usage   .= "\n";
$usage   .= "Usage:\n";
$usage   .= "  $SCRIPT_NAME [-h] [-o option] [-p parameter]\n";
$usage   .= "\n";
$usage   .= "Options:\n";
$usage   .= "\n";
$usage   .= "  -h           : Help\n";
$usage   .= "  -o option    : MyDroid option\n";
$usage   .= "  -p parameter : MyDroid parameter\n";
$usage   .= "\n";

my %args;
getopts('ho:p:', \%args) or die $usage;

# HELP
if (defined $args{h}) {
    die $usage;
}

# OPTION
if (defined $args{o}) {
    $OPTION = $args{o};
}

# PARAMETER
if (defined $args{p}) {
    $PARAMETER = $args{p};
}

#------------------------------------------------------------------------------#
# FUNCTIONS                                                                    #
#------------------------------------------------------------------------------#

sub error {
    my ($descr, $exit_code) = @_;

    print "\nERROR: $descr\n\n";

    exit $exit_code;
}

sub read_cmd {
    if ($OPTION eq "") {
        print "\nOption\n";
        print "> ";

        $OPTION = <STDIN>;
        chomp($OPTION);

        if ($OPTION eq "") {
            &error("Option must be provided", 2);
        }
    }

    if ($PARAMETER eq "") {
        print "\nParameter\n";
        print "> ";

        $PARAMETER = <STDIN>;
        chomp($PARAMETER);

        if ($PARAMETER eq "") {
            &error("Parameter must be provided", 2);
        }
    }
}

sub show_info {
    print "\n";
    print "Option    = $OPTION\n";
    print "Parameter = $PARAMETER\n";
}

sub write_cmd {
    print "\n=> Writing command\n\n";

    $ARDUINO->write($OPTION);
    $ARDUINO->write("|");
    $ARDUINO->write($PARAMETER);
    $ARDUINO->write("|");
}

#------------------------------------------------------------------------------#
# MAIN                                                                         #
#------------------------------------------------------------------------------#

if (! defined $ARDUINO) {
    &error("Device not found on /dev/ttyACM0", 1);
}

&read_cmd();
&show_info();
&write_cmd();

exit 0;
