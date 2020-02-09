#include "checkcontent.h"

void CheckContent::checkContent(std::string &content) const {
    for (char &c: content) {
	bool correct = false;
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
	    correct = true;
	} else if (c >= '0' && c <= '9') {
	    correct = true;
	} else if (c == ' ') {
	    correct = true;
	}
	if (!correct) {
	    for (int i = 0; specialCharacters[i] != '\0'; ++i) {
		if (specialCharacters[i] == c) {
		    correct = true;
		    break;
		}
	    }
	}
	if (!correct){
	    throw CorruptContent();
	}
    }
}


