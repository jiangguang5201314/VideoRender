/* Company :       Nequeo Pty Ltd, http://www.nequeo.com.au/
*  Copyright :     Copyright � Nequeo Pty Ltd 2016 http://www.nequeo.com.au/
*
*  File :          SipMediaClient.h
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

#pragma once

#include "MediaGlobal.h"

#include "SIPClient.hh"
#include "BasicUsageEnvironment.hh"

namespace Live {
	namespace Media
	{
		/// <summary>
		/// Session initiation protocol client.
		/// </summary>
		class EXPORT_LIVE_MEDIA_SERVER_API SipClient
		{
		public:
			/// <summary>
			/// Session initiation protocol client.
			/// </summary>
			SipClient();

			/// <summary>
			/// This destructor. Call release to cleanup resources.
			/// </summary>
			virtual ~SipClient();

		private:
			bool _disposed;

			std::unique_ptr<SIPClient> _sipClient;
			std::unique_ptr<TaskScheduler> _scheduler;
			std::unique_ptr<BasicUsageEnvironment> _env;
		};
	}
}