#pragma once

//#include "IndexManager.h"
#include <stdexcept>
#include "base.h"
using namespace std;

enum IndexErrorCode{
	DEFAULT,
	INDEX_FILE_ERROR,
	BLOCK_ALLOC_ERROR,
	INSERT_ERROR,
	INSERT_FAILED,
	DELETE_FAILED,
	OUT_OF_RANGE,
	KEY_NOT_UNIQUE,
	KEY_NOT_EXIST,
	//...
};

class IndexException:public TableException {
private:
	int ErrorCode;
public:
	IndexException(
		int Errorcode,
		const char* msg)
		:TableException(msg), ErrorCode(Errorcode){}

	int getErrorCode() {
		return ErrorCode;
	}
};

