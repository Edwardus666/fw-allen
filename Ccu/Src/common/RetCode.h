#ifndef __RET_CODE_H_
#define __RET_CODE_H_

#ifdef __cplusplus
extern "C"{
#endif

typedef enum _RetCode
{
	 RC_SUCCESS 		= 0
	,RC_FAILED 			= 1
	,RC_PENDING 		= 2
	,RC_BAD_PARAMETER 	= 3
	,RC_UNKNOWN 		= 4
	,RC_DONOTHING 		= 5
	,RC_ABORTED 		= 6
	,RC_WRONG_PHASE 	= 7
	,RC_NO_RESOURCE 	= 8
	,RC_BUSY 			= 9
	,RC_DONE 			= 10
}RetCode;

#ifdef __cplusplus
}
#endif

#endif


