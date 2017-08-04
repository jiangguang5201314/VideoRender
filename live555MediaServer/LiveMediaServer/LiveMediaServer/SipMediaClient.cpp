/* Company :       Nequeo Pty Ltd, http://www.nequeo.com.au/
*  Copyright :     Copyright � Nequeo Pty Ltd 2016 http://www.nequeo.com.au/
*
*  File :          SipMediaClient.cpp
*  Purpose :       Sip Client class.
*
*/

/*
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#include "SipMediaClient.h"

using namespace Live::Media;

/// <summary>
/// Session initiation protocol client.
/// </summary>
SipClient::SipClient() :
	_sipClient(nullptr),
	_scheduler(nullptr),
	_env(nullptr),
	_disposed(false)
{
	// Begin by setting up our usage environment:
	_scheduler = std::unique_ptr<TaskScheduler>(BasicTaskScheduler::createNew());
	_env = std::unique_ptr<BasicUsageEnvironment>(BasicUsageEnvironment::createNew(*_scheduler));
}

/// <summary>
/// This destructor. Call release to cleanup resources.
/// </summary>
SipClient::~SipClient()
{
	if (!_disposed)
	{
		_disposed = true;

		_env = nullptr;
		_scheduler = nullptr;
		_sipClient = nullptr;
	}
}
