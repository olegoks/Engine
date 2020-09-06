#pragma once
#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <string>
#include "__file_formats_/Obj.h"
#include "__logic_engine_/LogicEngine.h"
#include <vector>

class DataBase {
private:

	static unsigned int last_model_id_;
	std::vector<const char*> objFilesList;
	DataInfo data_info_;
	bool LoadObjFile(const char* fileName);

public:

	class LoadFilesException {
	public:

		const char* file_name;
		int error_code;

		LoadFilesException(const char* file_name, int error_code) {

			this->error_code = error_code;
			this->file_name = file_name;

		}
		
	};

	explicit DataBase();
	~DataBase();
	bool LoadObjFiles();
	inline DataInfo GetDataInfo()const noexcept { return this->data_info_; }
	void AddFileName(const char* file_name);

};

#endif