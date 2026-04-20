#pragma once
#include "globals.h"

#define INT_PARAM(name,min,max,def) (Parameter{PT_INT,name,(double)(min),(double)(max),(double)(def)})
#define FLOAT_PARAM(name,min,max,def) (Parameter{PT_FLOAT,name,min,max,def})
#define COLORRGB_PARAM(name,def) (Parameter{PT_COLOR_RGB,name,0,0,0,def})
#define COLORL_PARAM(name) (Parameter{PT_COLOR_L,name,0,255,127})
#define BOOL_PARAM(name,def) (Parameter{PT_BOOL,name,0,1,def})
#define INT_RANGE_PARAM(name,min,max,defl,defm,col) (Parameter{PT_INT_RANGE,name,min,max,defl,col,defm})

enum ParameterType {
    PT_INT = 0,
    PT_FLOAT = 1,
    PT_COLOR_RGB = 2,
    PT_COLOR_L = 3,
    PT_BOOL = 4,
    PT_INT_RANGE = 5,
    PT_STRING = 6,
};

struct Parameter {
    ParameterType paramType;
    std::string name;
    double numMinValue = 0;
    double numMaxValue = 1;

    double vNum = 0.5;
    u32 vU32 = 0;
    double vNum2 = 0.5;
    std::string vStr = "";

    static Parameter StringParam(std::string name, std::string defaultValue) {
        return Parameter{
            .paramType = PT_STRING,
            .name = name,
            .vStr = defaultValue
        };
    }
};
#define ParamList std::vector<Parameter>
inline Parameter PARAM_INVALID = Parameter((ParameterType)-1, "INVALID PARAMETER");

class ParameterStore {
public:
    ParamList parameters;

    ParameterStore() {}
    ParameterStore(ParamList params)
        : parameters(params) {}

    ParameterStore copy() {
        return ParameterStore(parameters);
    }

    bool hasParam(std::string key);

    Parameter& getParam(std::string key);
    int addOrGetParamIndex(Parameter p);

    bool getBool(std::string key) { return getParam(key).vNum == 1; }
    float getFloat(std::string key) { return getParam(key).vNum; }
    int getInt(std::string key) { return (int)getParam(key).vNum; }
    u32 getColorRGB(std::string key) { return getParam(key).vU32; }
    u8 getColorL(std::string key) { return (int)getParam(key).vNum; }
    std::pair<int, int> getIntRange(std::string key) { return { (int)getParam(key).vNum, (int)getParam(key).vNum2 }; };
    std::string getString(std::string key) { return getParam(key).vStr; }

    Panel* generateVerticalUI(std::function<void()> onChangedCallback, ParamList* paramList = NULL, std::string locKeyPrefix = "");
    Panel* generateHorizontalUI(std::function<void()> onChangedCallback);

    std::map<std::string, std::string> buildParameterMap();
    void setParametersFromParameterMap(std::map<std::string, std::string> paramMap);
    std::string serialize();
    void deserialize(std::string s);
};