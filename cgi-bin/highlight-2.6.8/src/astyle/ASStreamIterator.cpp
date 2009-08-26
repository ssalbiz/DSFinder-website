
#include "astyle.h"

#include <iostream>
#include <fstream>

using namespace astyle;


ASStreamIterator::ASStreamIterator(istream *in)
{
	inStream = in;
	buffer.reserve(200);
	inStreamEOF = false;
}

ASStreamIterator::~ASStreamIterator()
{
}

bool ASStreamIterator::hasMoreLines() const
{
	return !inStreamEOF;
}

/**
 * read the input stream, delete any end of line characters,
 *     and build a string that contains the input line.
 *
 * @return        string containing the next input line minus any end of line characters
 */

string ASStreamIterator::nextLine()
{
	char ch;
	char LF = '\n';
	char CR = '\r';
	inStream->get(ch);
	buffer.clear();

	while (!inStream->eof() && ch != LF && ch != CR)
	{
		buffer.append(1, ch);
		inStream->get(ch);
	}

	if (inStream->eof())
	{
		inStreamEOF = true;
		return buffer;
	}

	int peekch = inStream->peek();

	if (ch == CR)		// CR/LF is windows otherwise Mac OS 9
	{
		if (peekch == LF)
		{
			inStream->get();
			eolWindows++;
		}
		else
			eolMacOld++;
	}
	else				// LF is Linux, allow for improbable LF/CR
	{
		if (peekch == CR)
		{
			inStream->get();
			eolWindows++;
		}
		else
			eolLinux++;
	}

	// set output end of line character
	if (eolWindows >= eolLinux)
		if (eolWindows >= eolMacOld)
			strcpy(outputEOL, "\r\n");  // Windows (CRLF)
		else
			strcpy(outputEOL, "\r");    // MacOld (CR)
	else
		if (eolLinux >= eolMacOld)
			strcpy(outputEOL, "\n");    // Linux (LF)
		else
			strcpy(outputEOL, "\r");    // MacOld (CR)

	return buffer;
}


