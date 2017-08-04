/* Company :       Nequeo Pty Ltd, http://www.nequeo.com.au/
*  Copyright :     Copyright � Nequeo Pty Ltd 2016 http://www.nequeo.com.au/
*
*  File :          MP4TransportFileServerMedia.h
*  Purpose :       MP4 Media Server class.
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

#pragma once

#include "MediaGlobal.h"

#include <liveMedia.hh>

/// <summary>
/// MP4 live media server.
/// </summary>
class MP4TransportFileServerMedia
{
public:
	/// <summary>
	/// MP4 live media server.
	/// </summary>
	MP4TransportFileServerMedia();

	/// <summary>
	/// Create the MP4 SMS.
	/// </summary>
	/// <param name="sms">The current session.</param>
	/// <param name="env">The user environment.</param>
	/// <param name="fileName">The filename.</param>
	/// <param name="reuseSource">The resource reuse.</param>
	static void createNewMP4SMS(ServerMediaSession* sms, UsageEnvironment& env, char const* fileName, Boolean reuseSource);
};