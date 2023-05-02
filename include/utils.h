#ifndef _COMM_UTILS_H_
#define _COMM_UTILS_H_

#ifdef THROW_EXCEPTION_ON_ERROR
#define HANDLE_ERROR(x, err, retval)	\
	do																	\
	{																		\
		if(x)															\
			throw std::runtime_error(err);	\
	}while(0)
#else		// THROW_EXCEPTION_ON_ERROR
#define HANDLE_ERROR(x, err, retval)	\
	do																	\
	{																		\
		if(x)															\
			return retval;									\
	}while(0)
#endif	// THROW_EXCEPTION_ON_ERROR


enum ServerInitFlags
{
	SS_PROTOCOLTCP  = 0x01,
	SS_PROTOCOLUDP  = 0x02,
	SS_NOREUSE      = 0x00,
	SS_REUSEADDR    = 0x04,
	SS_REUSEPORT    = 0x08,

	SS_END					= 0x10000000
};


#endif // _COMM_UTILS_H_
