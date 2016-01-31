#include <wiringPi.h>
#include <iostream>		// cout + cerr + endl
#include <unistd.h>		// setuid
#include <stdio.h>		// perror
#include <vector>		// vector
#include <bitset>		// bitset
#include <sys/time.h>	// gettimeofday

using namespace std;

#define version "1.0.0"

#define MAX_PIN		16

#define LOW_STATE	0
#define HIGH_STATE	1

#define PULSE_LENGTH_USEC					650
#define IGNORE_CHANGE_BELOW_USEC			PULSE_LENGTH_USEC * 0.92
#define PULSE_LENGTH_TOLERANCE				1.7
#define ONE_PULSE_LENGTH_WITH_TOLERANCE		1 * ( PULSE_LENGTH_USEC * PULSE_LENGTH_TOLERANCE )
#define TWO_PULSE_LENGTH_WITH_TOLERANCE		2 * ( PULSE_LENGTH_USEC * PULSE_LENGTH_TOLERANCE )
#define TEN_PULSE_LENGTH					10 * PULSE_LENGTH_USEC
#define ELEVEN_PULSE_LENGTH					11 * PULSE_LENGTH_USEC
#define BITS_NBR							32
#define CODE_HIGH_POSITION					BITS_NBR - 1

static volatile int previousState = LOW_STATE;
static struct timeval previousTime;
static volatile int previousPulseLength = -1;
static volatile int howManyCorrectPulses = 0;
static volatile int positionInCode = CODE_HIGH_POSITION;
static bitset<BITS_NBR> code = bitset<BITS_NBR>(0);

static int pin = -1;

void usage( char ** argv )
{
  cout << "Usage: " << argv[0] << " v" << version << ": <pin number>" << endl
  << " <pin number> wiringPi pin number" << endl
  << endl;
}

void handleInterrupt( void )
{
	struct timeval currentTime;
	unsigned long timeSinceLastChange;

	int currentState = ! digitalRead( pin );

	gettimeofday( & currentTime , NULL );

	// Time difference in usec
	timeSinceLastChange = (currentTime.tv_sec * 1000000 + currentTime.tv_usec) - (previousTime.tv_sec * 1000000 + previousTime.tv_usec);

	// Avoid noise
	if( timeSinceLastChange > IGNORE_CHANGE_BELOW_USEC )
	{
		// Define pulse length
		int currentPulseLength=2;

		// It's a real interrupt
		if( timeSinceLastChange <= TWO_PULSE_LENGTH_WITH_TOLERANCE )
		{
			howManyCorrectPulses++;

			// Interrupt is only pulse length
			if( timeSinceLastChange < ONE_PULSE_LENGTH_WITH_TOLERANCE )	currentPulseLength = 1;

			int bitValue = -1;

			// It's a 0 value
			if( previousState == HIGH_STATE && previousPulseLength == 1
				&& currentState == LOW_STATE && currentPulseLength == 2 )
			{
				bitValue = LOW_STATE;
			}
			// It's a 1 value
			else if(	previousState == HIGH_STATE && previousPulseLength == 2
						&& currentState == LOW_STATE && currentPulseLength == 1 )
			{
				bitValue = HIGH_STATE;
			}

			// The two last pulses are valid
			if( howManyCorrectPulses == 2 )
			{
				// Reset
				howManyCorrectPulses = 0;

				// Manage a real bit
				if( bitValue >= 0 )
				{
					// Update code with this bit
					code[ positionInCode ] = bitValue;
					// Change position in code to set
					positionInCode--;
				}
				// Reset bit to set in code
				else	positionInCode = CODE_HIGH_POSITION;
			}
		}
		// Long low pulse
		else if( timeSinceLastChange > TEN_PULSE_LENGTH && timeSinceLastChange < ELEVEN_PULSE_LENGTH )
		{
			// It's the two validation pulses
			if(	previousState == HIGH_STATE && previousPulseLength == 1
				&& currentState == LOW_STATE
				&& positionInCode < 0 )
			{
				printf("%u\n" , code.to_ulong() );
			}

			// Reset
			positionInCode = CODE_HIGH_POSITION;
			howManyCorrectPulses = 0;
		}
		else
		{
			// Reset
			positionInCode = CODE_HIGH_POSITION;
			howManyCorrectPulses = 0;
			previousPulseLength = -1;
		}

		// Update previous state and pulse length
		previousState = currentState;
		previousPulseLength = currentPulseLength;
	}

	// Update previous interrupt time
	previousTime = currentTime;
}

int main( int argc , char * argv[] )
{
	//
	// Check input arguments
	// Only 1 is managed
	//
	if( argc != 2 )
	{
		usage( argv );
		return 1;
	}

	pin = atoi( argv[1] );

	//
	// Check input parameters
	//
	bool canContinue = true;

	if( pin < 0 || pin > MAX_PIN )
	{
		cerr << "Invalid argument: pin number must be between 0 and " << MAX_PIN << endl;
		canContinue = false;
	}

	// Check if program can continue
	if( ! canContinue )
	{
		usage( argv );
		return 2;
	}

	// Check if program has been run as root
	if( setuid( 0 ) )
	{
		perror( "setuid" );
		cerr << argv[0] << " must be run as root" << endl;
		return 3;
	}

	//
	// Initialize wiringPi library
	// Print instructions if necessary
	//
	if( wiringPiSetup() == -1 )
	{
		cerr << "WiringPi library not found, please install it:" << endl
		<< " mkdir -p ~/src && cd ~/src" << endl
		<< " git clone git://git.drogon.net/wiringPi" << endl
		<< " cd ~/src/wiringpi" << endl
		<< " ./build" << endl;
		return 4;
	}

	// Get current time
	gettimeofday( & previousTime , NULL );

	// Bind pin to interrupt function
	wiringPiISR( pin , INT_EDGE_BOTH , & handleInterrupt );

	// Avoid to end program
	while( 1 )	sleep( 60 );
}
