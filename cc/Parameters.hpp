#ifndef VSSS_Parameters_H
#define VSSS_Parameters_H

#include <lib/json.hpp>

using json = nlohmann::json;

class Parameters {
    public:
    json parameter_json;
    template<typename TYPE>
    TYPE get(std::string name, const TYPE& default_value) {
        if (parameter_json.find(name) != parameter_json.end()) {
			return parameter_json[name];
		} else {
            return default_value;
        }
    }
};

#endif //VSSS_Parameters_H