#include "LogicEngine.h"



LogicEngine::LogicEngine()noexcept :
	engine_is_running_(false),
	camera_(nullptr),
	data_info_{ nullptr, nullptr, nullptr, nullptr, 0, 0, 0, 0 }
{

	

}

void LogicEngine::AddModelName(const char* model_name) {

	this->models_names_.push_back(model_name);

}

void LogicEngine::UpdateWorldLogic() {

	unsigned int number_of_keystrokes = keystroke_buffer_.size();

	unsigned int index = 0;
	logic_function();
	//there is the bag: size returned 2, but real size is 1? and that is why Range out exception in vector.
	
	while ( (index < number_of_keystrokes) && (keystroke_buffer_[index].time_point < this->previous_keyboard_process_ + 20)) {

		special_function_(keystroke_buffer_[index].key_type);
		keystroke_buffer_.pop_back();
		number_of_keystrokes--;
		index++;

	}

	previous_keyboard_process_ += 20;

}



